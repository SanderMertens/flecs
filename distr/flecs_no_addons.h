// Comment out this line when using as DLL
#define flecs_STATIC

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
 * @ingroup c
 * Core ECS functionality (entities, storage, queries).
 *
 * @{
 */

/**
 * @defgroup options API defines
 * Defines for customizing compile-time features.
 *
 * @{
 */

/* Flecs version macros */
#define FLECS_VERSION_MAJOR 4  /**< Flecs major version. */
#define FLECS_VERSION_MINOR 1  /**< Flecs minor version. */
#define FLECS_VERSION_PATCH 6  /**< Flecs patch version. */

/** Flecs version. */
#define FLECS_VERSION FLECS_VERSION_IMPL(\
    FLECS_VERSION_MAJOR, FLECS_VERSION_MINOR, FLECS_VERSION_PATCH)

/** @def FLECS_CONFIG_HEADER
 * Allows for including a user-customizable header that specifies compile-time 
 * features. */
#ifdef FLECS_CONFIG_HEADER
#include "flecs_config.h"
#endif

/** @def ecs_float_t
 * Customizable precision for floating-point operations. */
#ifndef ecs_float_t
#define ecs_float_t float
#endif

/** @def ecs_ftime_t
 * Customizable precision for scalar time values. Change to double precision for
 * processes that can run for a long time (e.g., longer than a day). */
#ifndef ecs_ftime_t
#define ecs_ftime_t ecs_float_t
#endif

/** @def FLECS_ACCURATE_COUNTERS
 * Define to ensure that global counters used for statistics (such as the
 * allocation counters in the OS API) are accurate in multithreaded
 * applications, at the cost of increased overhead.
 */
// #define FLECS_ACCURATE_COUNTERS

/** @def FLECS_DISABLE_COUNTERS
 * Disables counters used for statistics. Improves performance, but
 * will prevent some features that rely on statistics from working,
 * like the statistics pages in the explorer.
 */
// #define FLECS_DISABLE_COUNTERS

/* Make sure provided configuration is valid */
#if defined(FLECS_DEBUG) && defined(FLECS_NDEBUG)
#warning "invalid configuration: cannot both define FLECS_DEBUG and FLECS_NDEBUG"
#endif
#if defined(FLECS_DEBUG) && defined(NDEBUG)
#warning "invalid configuration: cannot both define FLECS_DEBUG and NDEBUG"
#endif

/** @def FLECS_DEBUG
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

/** @def FLECS_SANITIZE
 * Enables expensive checks that can detect issues early. Recommended for
 * running tests or when debugging issues. This will severely slow down code.
 */
#ifdef FLECS_SANITIZE
#ifndef FLECS_DEBUG
#define FLECS_DEBUG /* If sanitized mode is enabled, so is debug mode */
#endif
#endif

/** @def FLECS_DEBUG_INFO
 * Adds additional debug information to internal data structures. Necessary when
 * using natvis.
 */
#ifdef FLECS_DEBUG
#ifndef FLECS_DEBUG_INFO
#define FLECS_DEBUG_INFO
#endif
#endif

/* Tip: if you see weird behavior that you think might be a bug, make sure to
 * test with the FLECS_DEBUG or FLECS_SANITIZE flags enabled. There's a good
 * chance that this gives you more information about the issue! */

/** @def FLECS_SOFT_ASSERT
 * Define to not abort for recoverable errors, like invalid parameters. An error
 * is still thrown to the console. This is recommended for when running inside a
 * third-party runtime, such as the Unreal editor.
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

/** @def FLECS_KEEP_ASSERT
 * By default, asserts are disabled in release mode, when either FLECS_NDEBUG or
 * NDEBUG is defined. Defining FLECS_KEEP_ASSERT ensures that asserts are not
 * disabled. This define can be combined with FLECS_SOFT_ASSERT.
 */
// #define FLECS_KEEP_ASSERT

/** @def FLECS_DEFAULT_TO_UNCACHED_QUERIES 
 * When set, this will cause queries with the EcsQueryCacheDefault policy
 * to default to EcsQueryCacheNone. This can reduce the memory footprint of
 * applications at the cost of performance. Queries that use features which
 * require caching, such as group_by and order_by, will still use caching.
 */
// #define FLECS_DEFAULT_TO_UNCACHED_QUERIES

/** @def FLECS_CREATE_MEMBER_ENTITIES 
 * By default, Flecs does not create entities for component members when they 
 * are registered with reflection. Define this addon if an application relies
 * on members being defined as entities. Features that require members to be
 * defined as entities are:
 *  - Member queries
 *  - Metrics
 *  - Alerts
 * 
 * Member entities can also be created on a per-type basis by setting 
 * ecs_struct_desc_t::create_member_entities to true.
 */
// #define FLECS_CREATE_MEMBER_ENTITIES

/** @def FLECS_CPP_NO_AUTO_REGISTRATION
 * When set, the C++ API will require that components are registered before they
 * are used. This is useful in multithreaded applications, where components need
 * to be registered beforehand, and to catch issues in projects where component 
 * registration is mandatory. Disabling automatic component registration also
 * slightly improves performance.
 * The C API is not affected by this feature.
 */
// #define FLECS_CPP_NO_AUTO_REGISTRATION

/** @def FLECS_CPP_NO_ENUM_REFLECTION 
 * When set, the C++ API will not attempt to discover and register enum 
 * constants for registered enum components. This will cause C++ APIs that 
 * accept enum constants to not work.
 * Disabling this feature can significantly improve compile times and reduce
 * the RAM footprint of an application.
 */
// #define FLECS_CPP_NO_ENUM_REFLECTION

/** @def FLECS_NO_ALWAYS_INLINE
 * When set, this will prevent functions from being annotated with always_inline,
 * which can improve performance at the cost of increased binary footprint.
 */
// #define FLECS_NO_ALWAYS_INLINE

/** @def FLECS_CUSTOM_BUILD
 * This macro lets you customize which addons to build Flecs with.
 * Without any addons, Flecs is just a minimal ECS storage, but addons add
 * features such as systems, scheduling, and reflection. If an addon is disabled,
 * it is excluded from the build, so that it consumes no resources. By default,
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
 * automatically enable their dependencies. To see the list of addons that were
 * compiled in a build, enable tracing before creating the world by doing:
 *
 * @code
 * ecs_log_set_level(0);
 * @endcode
 *
 * which outputs the full list of addons Flecs was compiled with.
 */
// #define FLECS_CUSTOM_BUILD

#ifndef FLECS_CUSTOM_BUILD
// #define FLECS_C           /**< C API convenience macros, always enabled. */
// #define FLECS_EXCLUSIVE_ACCESS /**< Enable exclusive world access checks. */
// #define FLECS_JOURNAL     /**< Journaling addon. */
#define FLECS_OS_API_IMPL    /**< Default implementation for OS API. */
// #define FLECS_PERF_TRACE  /**< Enable performance tracing. */
// #define FLECS_SCRIPT_MATH /**< Math functions for Flecs script (may require linking with libm). */
#endif // ifndef FLECS_CUSTOM_BUILD

/** @def FLECS_LOW_FOOTPRINT
 * Set a number of constants to values that decrease memory footprint, at the
 * cost of decreased performance. */
// #define FLECS_LOW_FOOTPRINT
#ifdef FLECS_LOW_FOOTPRINT
#define FLECS_HI_COMPONENT_ID 16
#define FLECS_HI_ID_RECORD_ID 16
#define FLECS_ENTITY_PAGE_BITS 6
#define FLECS_USE_OS_ALLOC
#define FLECS_DEFAULT_TO_UNCACHED_QUERIES
#endif

/** @def FLECS_HI_COMPONENT_ID
 * This constant can be used to balance between performance and memory
 * utilization. The constant is used in two ways:
 * - Entity IDs 0..FLECS_HI_COMPONENT_ID are reserved for component IDs.
 * - Used as the lookup array size in table edges.
 *
 * Increasing this value increases the size of the lookup array, which allows
 * fast table traversal, which improves performance of ECS add/remove
 * operations. Component IDs that fall outside of this range use a regular map
 * lookup, which is slower but more memory efficient. 
 * 
 * This value must be set to a power of 2. Setting it to a value that is not a
 * power of 2 will degrade performance.
 */
#ifndef FLECS_HI_COMPONENT_ID
#define FLECS_HI_COMPONENT_ID 256
#endif

/** @def FLECS_HI_ID_RECORD_ID
 * This constant can be used to balance between performance and memory
 * utilization. The constant is used to determine the size of the component record
 * lookup array. ID values that fall outside of this range use a regular map
 * lookup, which is slower but more memory efficient.
 */
#ifndef FLECS_HI_ID_RECORD_ID
#define FLECS_HI_ID_RECORD_ID 1024
#endif

/** @def FLECS_SPARSE_PAGE_BITS
 * This constant is used to determine the number of bits of an ID that is used
 * to determine the page index when used with a sparse set. The number of bits
 * determines the page size, which is (1 << bits).
 * Lower values decrease memory utilization, at the cost of more allocations. */
#ifndef FLECS_SPARSE_PAGE_BITS
#define FLECS_SPARSE_PAGE_BITS 6
#endif

/** @def FLECS_ENTITY_PAGE_BITS
 * Same as FLECS_SPARSE_PAGE_BITS, but for the entity index. */
#ifndef FLECS_ENTITY_PAGE_BITS
#define FLECS_ENTITY_PAGE_BITS 10
#endif

/** @def FLECS_USE_OS_ALLOC
 * When enabled, Flecs will use the OS allocator provided in the OS API directly
 * instead of the built-in block allocator. This can decrease memory utilization
 * as memory will be freed more often, at the cost of decreased performance. */
// #define FLECS_USE_OS_ALLOC

/** @def FLECS_ID_DESC_MAX
 * Maximum number of IDs to add in ecs_entity_desc_t / ecs_bulk_desc_t. */
#ifndef FLECS_ID_DESC_MAX
#define FLECS_ID_DESC_MAX 32
#endif

/** @def FLECS_EVENT_DESC_MAX
 * Maximum number of events in ecs_observer_desc_t. */
#ifndef FLECS_EVENT_DESC_MAX
#define FLECS_EVENT_DESC_MAX 8
#endif

/** @def FLECS_VARIABLE_COUNT_MAX
 * Maximum number of query variables per query. */
#define FLECS_VARIABLE_COUNT_MAX 64

/** @def FLECS_TERM_COUNT_MAX 
 * Maximum number of terms in queries. Should not exceed 64. */
#ifndef FLECS_TERM_COUNT_MAX
#define FLECS_TERM_COUNT_MAX 32
#endif

/** @def FLECS_TERM_ARG_COUNT_MAX 
 * Maximum number of arguments for a term. */
#ifndef FLECS_TERM_ARG_COUNT_MAX
#define FLECS_TERM_ARG_COUNT_MAX 16
#endif

/** @def FLECS_QUERY_VARIABLE_COUNT_MAX
 * Maximum number of query variables per query. Should not exceed 128. */
#ifndef FLECS_QUERY_VARIABLE_COUNT_MAX
#define FLECS_QUERY_VARIABLE_COUNT_MAX 64
#endif

/** @def FLECS_QUERY_SCOPE_NESTING_MAX
 * Maximum nesting depth of query scopes. */
#ifndef FLECS_QUERY_SCOPE_NESTING_MAX
#define FLECS_QUERY_SCOPE_NESTING_MAX 8
#endif

/** @def FLECS_DAG_DEPTH_MAX
 * Maximum number of levels in a DAG (acyclic relationship graph). If a graph with a
 * depth larger than this is encountered, a CYCLE_DETECTED panic is thrown.
 */
#ifndef FLECS_DAG_DEPTH_MAX
#define FLECS_DAG_DEPTH_MAX 128
#endif

/** @def FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE
 * Size of the depth cache in the tree spawner component. Higher values speed up prefab
 * instantiation for deeper hierarchies, at the cost of slightly more memory.
 */
#define FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE (6)

/** @} */

#ifndef FLECS_API_DEFINES_H
#define FLECS_API_DEFINES_H

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
#define EcsWorldFrameInProgress       (1u << 8)

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
#define EcsEntityHasDontFragment      (1u << 28)

////////////////////////////////////////////////////////////////////////////////
//// ID flags (used by ecs_component_record_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIdOnDeleteRemove            (1u << 0)
#define EcsIdOnDeleteDelete            (1u << 1)
#define EcsIdOnDeletePanic             (1u << 2)
#define EcsIdOnDeleteMask\
    (EcsIdOnDeletePanic|EcsIdOnDeleteRemove|EcsIdOnDeleteDelete)

#define EcsIdOnDeleteTargetRemove      (1u << 3)
#define EcsIdOnDeleteTargetDelete      (1u << 4)
#define EcsIdOnDeleteTargetPanic       (1u << 5)
#define EcsIdOnDeleteTargetMask\
    (EcsIdOnDeleteTargetPanic|EcsIdOnDeleteTargetRemove|\
        EcsIdOnDeleteTargetDelete)

#define EcsIdOnInstantiateOverride     (1u << 6)
#define EcsIdOnInstantiateInherit      (1u << 7)
#define EcsIdOnInstantiateDontInherit  (1u << 8)
#define EcsIdOnInstantiateMask\
    (EcsIdOnInstantiateOverride|EcsIdOnInstantiateInherit|\
        EcsIdOnInstantiateDontInherit)

#define EcsIdExclusive                 (1u << 9)
#define EcsIdTraversable               (1u << 10)
#define EcsIdPairIsTag                 (1u << 11)
#define EcsIdWith                      (1u << 12)
#define EcsIdCanToggle                 (1u << 13)
#define EcsIdIsTransitive              (1u << 14)
#define EcsIdInheritable               (1u << 15)

#define EcsIdHasOnAdd                  (1u << 16) /* Same values as table flags. */
#define EcsIdHasOnRemove               (1u << 17) 
#define EcsIdHasOnSet                  (1u << 18)
#define EcsIdHasOnTableCreate          (1u << 19)
#define EcsIdHasOnTableDelete          (1u << 20)
#define EcsIdSparse                    (1u << 21)
#define EcsIdDontFragment              (1u << 22)
#define EcsIdMatchDontFragment         (1u << 23) /* For (*, T) wildcards. */
#define EcsIdOrderedChildren           (1u << 24)
#define EcsIdSingleton                 (1u << 25)
#define EcsIdEventMask\
    (EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|\
        EcsIdHasOnTableCreate|EcsIdHasOnTableDelete|EcsIdSparse|\
        EcsIdOrderedChildren)
#define EcsIdPrefabChildren            (1u << 26)

#define EcsIdMarkedForDelete           (1u << 30)

/* Utilities for converting from flags to delete policies and vice versa. */
#define ECS_ID_ON_DELETE(flags) \
    ((ecs_entity_t[]){0, EcsRemove, EcsDelete, 0, EcsPanic}\
        [((flags) & EcsIdOnDeleteMask)])
#define ECS_ID_ON_DELETE_TARGET(flags) ECS_ID_ON_DELETE(flags >> 3)
#define ECS_ID_ON_DELETE_FLAG(id) (1u << ((id) - EcsRemove))
#define ECS_ID_ON_DELETE_TARGET_FLAG(id) (1u << (3 + ((id) - EcsRemove)))

/* Utilities for converting from flags to instantiate policies and vice versa. */
#define ECS_ID_ON_INSTANTIATE(flags) \
    ((ecs_entity_t[]){EcsOverride, EcsOverride, EcsInherit, 0, EcsDontInherit}\
        [(((flags) & EcsIdOnInstantiateMask) >> 6)])
#define ECS_ID_ON_INSTANTIATE_FLAG(id) (1u << (6 + ((id) - EcsOverride)))

////////////////////////////////////////////////////////////////////////////////
//// Bits set in world->non_trivial array
////////////////////////////////////////////////////////////////////////////////

#define EcsNonTrivialIdSparse          (1u << 0)
#define EcsNonTrivialIdNonFragmenting  (1u << 1)
#define EcsNonTrivialIdInherit         (1u << 2)

////////////////////////////////////////////////////////////////////////////////
//// Iterator flags (used by ecs_iter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIterIsValid                 (1u << 0u)  /* Does the iterator contain a valid result. */
#define EcsIterNoData                  (1u << 1u)  /* Does the iterator provide (component) data. */
#define EcsIterNoResults               (1u << 2u)  /* Iterator has no results. */
#define EcsIterMatchEmptyTables        (1u << 3u)  /* Match empty tables. */
#define EcsIterIgnoreThis              (1u << 4u)  /* Only evaluate non-this terms. */
#define EcsIterTrivialChangeDetection  (1u << 5u)
#define EcsIterHasCondSet              (1u << 6u)  /* Does the iterator have conditionally set fields. */
#define EcsIterProfile                 (1u << 7u)  /* Profile iterator performance. */
#define EcsIterTrivialSearch           (1u << 8u)  /* Trivial iterator mode. */
#define EcsIterTrivialTest             (1u << 11u) /* Trivial test mode (constrained $this). */
#define EcsIterTrivialCached           (1u << 14u) /* Trivial search for cached query. */
#define EcsIterCached                  (1u << 15u) /* Cached query. */
#define EcsIterFixedInChangeComputed   (1u << 16u) /* Change detection for fixed-in terms is done. */
#define EcsIterFixedInChanged          (1u << 17u) /* Fixed-in terms changed. */
#define EcsIterSkip                    (1u << 18u) /* Result was skipped for change detection. */
#define EcsIterCppEach                 (1u << 19u) /* Uses C++ 'each' iterator. */
#define EcsIterImmutableCacheData      (1u << 21u) /* Internally used by the engine to indicate immutable arrays from the cache. */

/* Same as event flags. */
#define EcsIterTableOnly               (1u << 20u)  /* Result only populates the table. */

////////////////////////////////////////////////////////////////////////////////
//// Event flags (used by ecs_event_desc_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsEventTableOnly              (1u << 20u) /* Table event (no data, same as iter flags). */
#define EcsEventNoOnSet                (1u << 16u) /* Don't emit OnSet for inherited IDs. */

////////////////////////////////////////////////////////////////////////////////
//// Query flags (used by ecs_query_t::flags)
////////////////////////////////////////////////////////////////////////////////

/* Flags that can only be set by the query implementation. */
#define EcsQueryMatchThis             (1u << 11u) /* Query has terms with $this source. */
#define EcsQueryMatchOnlyThis         (1u << 12u) /* Query only has terms with $this source. */
#define EcsQueryMatchOnlySelf         (1u << 13u) /* Query has no terms with up traversal. */
#define EcsQueryMatchWildcards        (1u << 14u) /* Query matches wildcards. */
#define EcsQueryMatchNothing          (1u << 15u) /* Query matches nothing. */
#define EcsQueryHasCondSet            (1u << 16u) /* Query has conditionally set fields. */
#define EcsQueryHasPred               (1u << 17u) /* Query has equality predicates. */
#define EcsQueryHasScopes             (1u << 18u) /* Query has query scopes. */
#define EcsQueryHasRefs               (1u << 19u) /* Query has terms with static source. */
#define EcsQueryHasOutTerms           (1u << 20u) /* Query has [out] terms. */
#define EcsQueryHasNonThisOutTerms    (1u << 21u) /* Query has [out] terms with no $this source. */
#define EcsQueryHasChangeDetection    (1u << 22u) /* Query has a monitor for change detection. */
#define EcsQueryIsTrivial             (1u << 23u) /* Query can use trivial evaluation function. */
#define EcsQueryHasCacheable          (1u << 24u) /* Query has cacheable terms. */
#define EcsQueryIsCacheable           (1u << 25u) /* All terms of the query are cacheable. */
#define EcsQueryHasTableThisVar       (1u << 26u) /* Does the query have $this table var. */
#define EcsQueryCacheYieldEmptyTables (1u << 27u) /* Does the query cache empty tables. */
#define EcsQueryTrivialCache          (1u << 28u) /* Trivial cache (no wildcards, traversal, order_by, group_by, change detection). */
#define EcsQueryNested                (1u << 29u) /* Query created by a query (for observer, cache). */
#define EcsQueryCacheWithFilter       (1u << 30u)
#define EcsQueryValid                 (1u << 31u)

////////////////////////////////////////////////////////////////////////////////
//// Term flags (used by ecs_term_t::flags_)
////////////////////////////////////////////////////////////////////////////////

#define EcsTermMatchAny               (1u << 0)
#define EcsTermMatchAnySrc            (1u << 1)
#define EcsTermTransitive             (1u << 2)
#define EcsTermReflexive              (1u << 3)
#define EcsTermIdInherited            (1u << 4)
#define EcsTermIsTrivial              (1u << 5)
#define EcsTermIsCacheable            (1u << 6)
#define EcsTermIsScope                (1u << 7)
#define EcsTermIsMember               (1u << 8)
#define EcsTermIsToggle               (1u << 9)
#define EcsTermIsSparse               (1u << 10)
#define EcsTermIsOr                   (1u << 11)
#define EcsTermDontFragment           (1u << 12)
#define EcsTermNonFragmentingChildOf  (1u << 13)

////////////////////////////////////////////////////////////////////////////////
//// Observer flags (used by ecs_observer_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsObserverMatchPrefab         (1u << 1u)  /* Same as query. */
#define EcsObserverMatchDisabled       (1u << 2u)  /* Same as query. */
#define EcsObserverIsMulti             (1u << 3u)  /* Does the observer have multiple terms. */
#define EcsObserverIsMonitor           (1u << 4u)  /* Is the observer a monitor. */
#define EcsObserverIsDisabled          (1u << 5u)  /* Is the observer entity disabled. */
#define EcsObserverIsParentDisabled    (1u << 6u)  /* Is the module parent of the observer disabled. */
#define EcsObserverBypassQuery         (1u << 7u)  /* Don't evaluate query for multi-component observer. */
#define EcsObserverYieldOnCreate       (1u << 8u)  /* Yield matching entities when creating observer. */
#define EcsObserverYieldOnDelete       (1u << 9u)  /* Yield matching entities when deleting observer. */
#define EcsObserverKeepAlive           (1u << 11u) /* Observer keeps component alive (same value as EcsTermKeepAlive). */

////////////////////////////////////////////////////////////////////////////////
//// Table flags (used by ecs_table_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsTableHasBuiltins            (1u << 0u)  /* Does the table have built-in components. */
#define EcsTableIsPrefab               (1u << 1u)  /* Does the table store prefabs. */
#define EcsTableHasIsA                 (1u << 2u)  /* Does the table have IsA relationship. */
#define EcsTableHasMultiIsA            (1u << 3u)  /* Does the table have multiple IsA pairs. */
#define EcsTableHasChildOf             (1u << 4u)  /* Does the table type have ChildOf relationship. */
#define EcsTableHasParent              (1u << 5u)  /* Does the table type have Parent component. */
#define EcsTableHasName                (1u << 6u)  /* Does the table type have (Identifier, Name). */
#define EcsTableHasPairs               (1u << 7u)  /* Does the table type have pairs. */
#define EcsTableHasModule              (1u << 8u)  /* Does the table have module data. */
#define EcsTableIsDisabled             (1u << 9u)  /* Does the table type have EcsDisabled. */
#define EcsTableNotQueryable           (1u << 10u)  /* Table should never be returned by queries. */
#define EcsTableHasCtors               (1u << 11u)
#define EcsTableHasDtors               (1u << 12u)
#define EcsTableHasCopy                (1u << 13u)
#define EcsTableHasMove                (1u << 14u)
#define EcsTableHasToggle              (1u << 15u)

#define EcsTableHasOnAdd               (1u << 16u) /* Same values as ID flags. */
#define EcsTableHasOnRemove            (1u << 17u)
#define EcsTableHasOnSet               (1u << 18u)
#define EcsTableHasOnTableCreate       (1u << 19u)
#define EcsTableHasOnTableDelete       (1u << 20u)
#define EcsTableHasSparse              (1u << 21u)
#define EcsTableHasDontFragment        (1u << 22u)
#define EcsTableOverrideDontFragment   (1u << 23u)
#define EcsTableHasOrderedChildren     (1u << 24u)
#define EcsTableHasOverrides           (1u << 25u)

#define EcsTableHasTraversable         (1u << 27u)
#define EcsTableEdgeReparent           (1u << 28u)
#define EcsTableMarkedForDelete        (1u << 29u)

/* Composite table flags */
#define EcsTableHasLifecycle     (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex        (EcsTableHasLifecycle | EcsTableHasToggle | EcsTableHasSparse)
#define EcsTableHasAddActions    (EcsTableHasIsA | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet)
#define EcsTableHasRemoveActions (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove)
#define EcsTableEdgeFlags        (EcsTableHasOnAdd | EcsTableHasOnRemove | EcsTableHasSparse)
#define EcsTableAddEdgeFlags     (EcsTableHasOnAdd | EcsTableHasSparse)
#define EcsTableRemoveEdgeFlags  (EcsTableHasOnRemove | EcsTableHasSparse | EcsTableHasOrderedChildren)

////////////////////////////////////////////////////////////////////////////////
//// Aperiodic action flags (used by ecs_run_aperiodic())
////////////////////////////////////////////////////////////////////////////////

#define EcsAperiodicComponentMonitors  (1u << 2u)  /* Process component monitors. */
#define EcsAperiodicEmptyQueries       (1u << 4u)  /* Process empty queries. */

#ifdef __cplusplus
}
#endif

#endif

#if defined(_WIN32) || defined(_MSC_VER)
#define ECS_TARGET_WINDOWS
#elif defined(__COSMOCC__)
#define ECS_TARGET_POSIX
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

/* Map between clang and Apple clang versions, as version 13 has a difference in
 * the format of __PRETTY_FUNCTION__, which enum reflection depends on. */
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

/* Define noreturn attribute only for GCC or Clang. */
#if defined(ECS_TARGET_GNU) || defined(ECS_TARGET_CLANG)
    #define ECS_NORETURN __attribute__((noreturn))
#else
    #define ECS_NORETURN
#endif

/* Ignored warnings */
#if defined(ECS_TARGET_CLANG)
/* Ignore unknown options so we don't have to care about the compiler version. */
#pragma clang diagnostic ignored "-Wunknown-warning-option"
/* Warns for double or redundant semicolons. There are legitimate cases where a
 * semicolon after an empty statement is useful, for example, after a macro that
 * is replaced with a code block. With this warning enabled, semicolons would 
 * only have to be added after macros that are not code blocks, which in some
 * cases isn't possible as the implementation of a macro can be different in
 * debug/release mode. */
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
/* This is valid in C99, and Flecs must be compiled as C99. */
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
/* Clang attribute to detect fallthrough isn't supported on older versions. 
 * Implicit fallthrough is still detected by gcc and ignored with "fall through"
 * comments. */
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
/* This warning prevents adding a default case when all enum constants are part
 * of the switch. In C, however, an enum type can assume any value in the range of
 * the type, and this warning makes it harder to catch invalid enum values. */
#pragma clang diagnostic ignored "-Wcovered-switch-default"
/* This warning prevents some casts of function results to a different kind of
 * type, e.g., casting an int result to double. Not very useful in practice, as
 * it just forces the code to assign to a variable first, then cast. */
#pragma clang diagnostic ignored "-Wbad-function-cast"
/* Format strings can be passed down from other functions. */
#pragma clang diagnostic ignored "-Wformat-nonliteral"
/* Useful, but not reliable enough. It can incorrectly flag macros as unused
 * in standalone builds. */
#pragma clang diagnostic ignored "-Wunused-macros"
/* This warning gets thrown by clang even when the code is handling all case
 * values but not all cases (for example, when the switch contains a LastValue
 * case). Adding a "default" case fixes the warning, but silences future 
 * warnings about unhandled cases, which is worse. */
#pragma clang diagnostic ignored "-Wswitch-default"
#if __clang_major__ == 13
/* clang 13 can throw this warning for a macro in ctype.h. */
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
/* Filenames aren't consistent across targets as they can use different casing 
 * (e.g., WinSock2 vs. winsock2). */
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
/* Very difficult to work around this warning in C, especially for an ECS. */
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
/* This warning gets thrown when trying to cast a pointer returned from dlproc. */
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
/* This warning can get thrown for expressions that evaluate to constants
 * in debug/release mode. */
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
/* With soft asserts enabled, the code won't abort, which in some cases means
 * code paths are reached where values are uninitialized. */
#ifdef FLECS_SOFT_ASSERT
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#endif

/* Allows for enum reflection support on legacy compilers. */
#if __clang_major__ < 16
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
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
 * only require partial initialization, so this warning isn't useful.
 * It doesn't introduce any safety issues (fields are guaranteed to be 0 
 * initialized), and later versions of gcc (>=11) seem to no longer throw this 
 * warning. */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
/* Produces false positives in addons/cpp/delegate.hpp. */
#pragma GCC diagnostic ignored "-Warray-bounds"
/* Produces false positives in queries/src/cache.c. */
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Wrestrict"

#elif defined(ECS_TARGET_MSVC)
/* Recursive on all control paths, the function will cause runtime stack overflow.
 * This warning is incorrectly thrown on enum reflection code. */
#pragma warning(disable: 4717)
#endif

/* Allows for enum reflection support on legacy compilers. */
#if defined(__GNUC__) && __GNUC__ <= 10
#pragma GCC diagnostic ignored "-Wconversion"
#endif

/* Standard library dependencies */
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/* Non-standard but required. If not provided by the platform, add manually. */
#include <stdint.h>

/* Contains macros for importing and exporting symbols. */
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

/* Some symbols are only exported when building in a debug build, to enable
 * white-box testing of internal data structures. */
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

/* The API uses the native bool type in C++, or a custom one in C. */
#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#undef bool
#undef true
#undef false
typedef char bool;
#define false 0
#define true !false
#endif

/* Utility types to indicate usage as a bitmask. */
typedef uint8_t ecs_flags8_t;
typedef uint16_t ecs_flags16_t;
typedef uint32_t ecs_flags32_t;
typedef uint64_t ecs_flags64_t;

/* Bitmask type with compile-time-defined size. */
#define ecs_flagsn_t_(bits) ecs_flags##bits##_t
#define ecs_flagsn_t(bits) ecs_flagsn_t_(bits)

/* Bitset type that can store exactly as many bits as there are terms. */
#define ecs_termset_t ecs_flagsn_t(FLECS_TERM_COUNT_MAX)

/* Utility macros for setting/clearing termset bits. */
#define ECS_TERMSET_SET(set, flag) ((set) |= (ecs_termset_t)(flag))
#define ECS_TERMSET_CLEAR(set, flag) ((set) &= (ecs_termset_t)~(flag))
#define ECS_TERMSET_COND(set, flag, cond) ((cond) \
    ? (ECS_TERMSET_SET(set, flag)) \
    : (ECS_TERMSET_CLEAR(set, flag)))

/* Keep unsigned integers out of the codebase as they do more harm than good. */
typedef int32_t ecs_size_t;

/* Allocator type. */
typedef struct ecs_allocator_t ecs_allocator_t;

#define ECS_SIZEOF(T) ECS_CAST(ecs_size_t, sizeof(T))

/* Alignment macros:
 * - Use alignof in C++
 * - Use _Alignof in C11 and above
 * - Use __alignof__ on 64 bit platforms on clang/gcc
 * - Use struct trick on other compilers/32 bit
 * 
 * The reason the code doesn't use the struct trick everywhere is because this
 * can cause ASAN errors (runtime error: member access within null pointer).
 * 
 * The reason why the clang/gcc __alignof__ feature is not used on 32 bit, is
 * because its behavior is different than C++:
 * 
 * __alignof__(uint64_t) == 8 on 32-bit platforms
 * alignof(uint64_t) == 4 on 32-bit platforms
 *
 * typedef struct {
 *   uint64_t value;
 * } Foo;
 * 
 * __alignof__(Foo) == 4 on 32-bit platforms
 * alignof(Foo) == 4 on 32-bit platforms
 */
#ifdef __cplusplus
#define ECS_ALIGNOF(T) static_cast<int64_t>(alignof(T))
#elif defined(ECS_TARGET_MSVC)
#define ECS_ALIGNOF(T) (int64_t)__alignof(T)
#else
#define ECS_ALIGNOF(T) ((int64_t)(size_t)&((struct { char c; T d; } *)0)->d)
#if defined(ECS_TARGET_GNU) || defined(ECS_TARGET_CLANG)
#if __SIZEOF_POINTER__ == 8
#undef ECS_ALIGNOF
#define ECS_ALIGNOF(T) (int64_t)__alignof__(T)
#endif
#endif
#endif

#ifndef FLECS_NO_ALWAYS_INLINE
    #if defined(ECS_TARGET_CLANG) || defined(ECS_TARGET_GCC)
        #define FLECS_ALWAYS_INLINE __attribute__((always_inline))
    #elif defined(ECS_TARGET_MSVC)
        #define FLECS_ALWAYS_INLINE
    #else
        #define FLECS_ALWAYS_INLINE
    #endif
#else
    #define FLECS_ALWAYS_INLINE
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

/* Simple utility for determining the max of two values. */
#define ECS_MAX(a, b) (((a) > (b)) ? a : b)
#define ECS_MIN(a, b) (((a) < (b)) ? a : b)

/* Abstraction on top of C-style casts so that C functions can be used in C++
 * code without producing warnings. */
#ifndef __cplusplus
#define ECS_CAST(T, V) ((T)(V))
#else
#define ECS_CAST(T, V) (static_cast<T>(V))
#endif

/* Utility macro for doing const casts without warnings. */
#ifndef __cplusplus
#define ECS_CONST_CAST(type, value) ((type)(uintptr_t)(value))
#else
#define ECS_CONST_CAST(type, value) (const_cast<type>(value))
#endif

/* Utility macro for doing pointer casts without warnings. */
#ifndef __cplusplus
#define ECS_PTR_CAST(type, value) ((type)(uintptr_t)(value))
#else
#define ECS_PTR_CAST(type, value) (reinterpret_cast<type>(value))
#endif

/* Utility macros to do bitwise comparisons between floats without warnings. */
#define ECS_EQ(a, b) (ecs_os_memcmp(&(a), &(b), sizeof(a)) == 0)
#define ECS_NEQ(a, b) (!ECS_EQ(a, b))
#define ECS_EQZERO(a) ECS_EQ(a, (uint64_t){0})
#define ECS_NEQZERO(a) ECS_NEQ(a, (uint64_t){0})

/* Utilities to convert Flecs version to a string. */
#define FLECS_VERSION_IMPLSTR(major, minor, patch) #major "." #minor "." #patch
#define FLECS_VERSION_IMPL(major, minor, patch) \
    FLECS_VERSION_IMPLSTR(major, minor, patch)

#define ECS_CONCAT(a, b) a ## b

////////////////////////////////////////////////////////////////////////////////
//// Magic numbers for sanity checking
////////////////////////////////////////////////////////////////////////////////

/* Magic number to identify the type of the object. */
#define ecs_world_t_magic     (0x65637377)
#define ecs_stage_t_magic     (0x65637373)
#define ecs_query_t_magic     (0x65637375)
#define ecs_observer_t_magic  (0x65637362)

////////////////////////////////////////////////////////////////////////////////
//// Entity ID macros
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
#define ECS_IS_VALUE_PAIR(id)         (((id) & ECS_ID_FLAGS_MASK) == ECS_VALUE_PAIR)
#define ECS_IS_PAIR(id)               ((((id) & ECS_ID_FLAGS_MASK) == ECS_PAIR) || ECS_IS_VALUE_PAIR(id))
#define ECS_PAIR_FIRST(e)             (ecs_entity_t_hi(e & ECS_COMPONENT_MASK))
#define ECS_PAIR_SECOND(e)            (ecs_entity_t_lo(e))
#define ECS_HAS_RELATION(e, rel)      (ECS_HAS_ID_FLAG(e, PAIR) && (ECS_PAIR_FIRST(e) == rel))

#define ECS_TERM_REF_FLAGS(ref)       ((ref)->id & EcsTermRefFlags)
#define ECS_TERM_REF_ID(ref)          ((ref)->id & ~EcsTermRefFlags)

////////////////////////////////////////////////////////////////////////////////
//// Convert between C typenames and variables
////////////////////////////////////////////////////////////////////////////////

/** Translate a C type to an ID. */
#define ecs_id(T) FLECS_ID##T##ID_

////////////////////////////////////////////////////////////////////////////////
//// Utilities for working with pair identifiers
////////////////////////////////////////////////////////////////////////////////

#define ecs_entity_t_lo(value) ECS_CAST(uint32_t, value)
#define ecs_entity_t_hi(value) ECS_CAST(uint32_t, (value) >> 32)
#define ecs_entity_t_comb(lo, hi) ((ECS_CAST(uint64_t, hi) << 32) + ECS_CAST(uint32_t, lo))

#define ecs_pair(rel, tgt) (ECS_PAIR | ecs_entity_t_comb(tgt, rel))
#define ecs_pair_t(rel, tgt) (ECS_PAIR | ecs_entity_t_comb(tgt, ecs_id(rel)))
#define ecs_pair_first(world, pair) ecs_get_alive(world, ECS_PAIR_FIRST(pair))
#define ecs_pair_second(world, pair) ecs_get_alive(world, ECS_PAIR_SECOND(pair))
#define ecs_pair_relation ecs_pair_first
#define ecs_pair_target ecs_pair_second

#define ecs_value_pair(rel, val) (ECS_VALUE_PAIR | ecs_entity_t_comb(val, rel))

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
//// Convenience macros for ctor, dtor, move, and copy
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
        type *field_data = ecs_field(_it, type, 0);\
        for (int32_t i = 0; i < _it->count; i ++) {\
            ecs_entity_t entity = _it->entities[i];\
            type *var = &field_data[i];\
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
 * @defgroup core_types Core API Types
 * Types for core API objects.
 *
 * @{
 */

/** IDs are the things that can be added to an entity.
 * An ID can be an entity or pair, and can have optional ID flags. */
typedef uint64_t ecs_id_t;

/** An entity identifier.
 * Entity IDs consist of a number unique to the entity in the lower 32 bits,
 * and a counter used to track entity liveliness in the upper 32 bits. When an
 * ID is recycled, its generation count is increased. This causes recycled IDs
 * to be very large (>4 billion), which is normal. */
typedef ecs_id_t ecs_entity_t;

/** A type is a list of (component) IDs.
 * Types are used to communicate the "type" of an entity. In most type systems, a
 * typeof operation returns a single type. In ECS, however, an entity can have
 * multiple components, which is why an ECS type consists of a vector of IDs.
 *
 * The component IDs of a type are sorted, which ensures that it doesn't matter
 * in which order components are added to an entity. For example, if adding
 * Position then Velocity would result in type [Position, Velocity], first
 * adding Velocity then Position would also result in type [Position, Velocity].
 *
 * Entities are grouped together by type in the ECS storage in tables. The
 * storage has exactly one table per unique type that is created by the
 * application that stores all entities and components for that type. This is
 * also referred to as an archetype.
 */
typedef struct {
    ecs_id_t *array;    /**< Array with IDs. */
    int32_t count;      /**< Number of elements in array. */
} ecs_type_t;

/** A world is the container for all ECS data and supporting features.
 * Applications can have multiple worlds, though in most cases will only need
 * one. Worlds are isolated from each other, and can have separate sets of
 * systems, components, modules, etc.
 *
 * If an application has multiple worlds with overlapping components, it is
 * common (though not strictly required) to use the same component IDs across
 * worlds, which can be achieved by declaring a global component ID variable.
 * To do this in the C API, see the entities/fwd_component_decl example. The
 * C++ API automatically synchronizes component IDs between worlds.
 *
 * Component ID conflicts between worlds can occur when a world has already used
 * an ID for something else. There are a few ways to avoid this:
 *
 * - Ensure to register the same components in each world, in the same order.
 * - Create a dummy world in which all components are preregistered, which
 *   initializes the global ID variables.
 *
 * In some use cases, typically when writing tests, multiple worlds are created
 * and deleted with different components, registered in different order. To
 * ensure isolation between tests, the C++ API has a `flecs::reset` function
 * that forces the API to ignore the old component IDs. */
typedef struct ecs_world_t ecs_world_t;

/** A stage enables modification while iterating and from multiple threads. */
typedef struct ecs_stage_t ecs_stage_t;

/** A table stores entities and components for a specific type. */
typedef struct ecs_table_t ecs_table_t;

/** A term is a single element in a query. */
typedef struct ecs_term_t ecs_term_t;

/** A query returns entities matching a list of constraints. */
typedef struct ecs_query_t ecs_query_t;

/** An observer is a system that is invoked when an event matches its query.
 * Observers allow applications to respond to specific events, such as adding or
 * removing a component. Observers are created by specifying both a query and
 * a list of event kinds that should be listened for. An example of an observer
 * that triggers when a Position component is added to an entity (in C++):
 *
 * @code
 * world.observer<Position>()
 *   .event(flecs::OnAdd)
 *   .each([](Position& p) {
 *     // called when Position is added to an entity
 *   });
 * @endcode
 *
 * Observers only trigger when the source of the event matches the full observer 
 * query. For example, an OnAdd observer for Position, Velocity will only 
 * trigger after both components have been added to the entity. */
typedef struct ecs_observer_t ecs_observer_t;

/** An observable produces events that can be listened for by an observer.
 * Currently only the world is observable. In the future, queries will become
 * observable objects as well. */
typedef struct ecs_observable_t ecs_observable_t;

/** Type used for iterating iterable objects.
 * Iterators are objects that provide applications with information
 * about the currently iterated result, and store any state required for the
 * iteration. */
typedef struct ecs_iter_t ecs_iter_t;

/** A ref is a fast way to fetch a component for a specific entity.
 * Refs are a faster alternative to repeatedly calling ecs_get() for the same
 * entity/component combination. When comparing the performance of getting a ref
 * to calling ecs_get(), a ref is typically 3-5x faster.
 *
 * Refs achieve this performance by caching internal data structures associated
 * with the entity and component on the ecs_ref_t object that otherwise would
 * have to be looked up. */
typedef struct ecs_ref_t ecs_ref_t;

/** Type hooks are callbacks associated with component lifecycle events.
 * Typical examples of lifecycle events are construction, destruction, copying,
 * and moving of components. */
typedef struct ecs_type_hooks_t ecs_type_hooks_t;

/** Type information.
 * Contains information about a (component) type, such as its size,
 * alignment, and type hooks. */
typedef struct ecs_type_info_t ecs_type_info_t;

/** Information about an entity, like its table and row. */
typedef struct ecs_record_t ecs_record_t;

/** Information about a (component) ID, such as type info and tables with the ID. */
typedef struct ecs_component_record_t ecs_component_record_t;

/** A poly object.
 * A poly (short for polymorph) object is an object that has a variable list of
 * capabilities, determined by a mixin table. This is the current list of types
 * in the Flecs API that can be used as an ecs_poly_t:
 *
 * - ecs_world_t
 * - ecs_stage_t
 * - ecs_query_t
 *
 * Functions that accept an ecs_poly_t argument can accept objects of these
 * types. If the object does not have the requested mixin, the API will throw an
 * assert.
 *
 * The poly/mixin framework enables partially overlapping features to be
 * implemented once, and enables objects of different types to interact with
 * each other depending on what mixins they have, rather than their type
 * (in some ways it's like a mini-ECS). Additionally, each poly object has a
 * header that enables the API to do sanity checking on the input arguments.
 */
typedef void ecs_poly_t;

/** Type that stores poly mixins. */
typedef struct ecs_mixins_t ecs_mixins_t;

/** Header for ecs_poly_t objects. */
typedef struct ecs_header_t {
    int32_t type;               /**< Magic number indicating which type of Flecs object. */
    int32_t refcount;           /**< Refcount, to enable RAII handles. */
    ecs_mixins_t *mixins;       /**< Table with offsets to (optional) mixins. */
} ecs_header_t;

/** Opaque type for table record. */
typedef struct ecs_table_record_t ecs_table_record_t;

/** @} */

#ifndef FLECS_VEC_H
#define FLECS_VEC_H

#ifdef __cplusplus
extern "C" {
#endif

/** A component column. */
typedef struct ecs_vec_t {
    void *array;              /**< Pointer to the element array. */
    int32_t count;            /**< Number of elements in the vector. */
    int32_t size;             /**< Allocated capacity in number of elements. */
#ifdef FLECS_SANITIZE
    ecs_size_t elem_size;     /**< Size of each element in bytes (sanitize only). */
    const char *type_name;    /**< Type name string for debugging (sanitize only). */
#endif
} ecs_vec_t;

/** Initialize a vector.
 *
 * @param allocator Allocator to use for memory management.
 * @param vec The vector to initialize.
 * @param size Size of each element in bytes.
 * @param elem_count Initial number of elements to allocate.
 */
FLECS_API
void ecs_vec_init(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Initialize a vector with debug info.
 *
 * @param allocator Allocator to use for memory management.
 * @param vec The vector to initialize.
 * @param size Size of each element in bytes.
 * @param elem_count Initial number of elements to allocate.
 * @param type_name Type name string for debugging.
 */
FLECS_API
void ecs_vec_init_w_dbg_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const char *type_name);

/** Type-safe vector initialization.
 *
 * @param allocator Allocator to use for memory management.
 * @param vec The vector to initialize.
 * @param T The element type.
 * @param elem_count Initial number of elements to allocate.
 */
#define ecs_vec_init_t(allocator, vec, T, elem_count) \
    ecs_vec_init_w_dbg_info(allocator, vec, ECS_SIZEOF(T), elem_count, "vec<"#T">")

/** Initialize a vector if it is not already initialized.
 *
 * @param vec The vector to initialize.
 * @param size Size of each element in bytes.
 */
FLECS_API
void ecs_vec_init_if(
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe conditional vector initialization.
 *
 * @param vec The vector to initialize.
 * @param T The element type.
 */
#define ecs_vec_init_if_t(vec, T) \
    ecs_vec_init_if(vec, ECS_SIZEOF(T))

/** Deinitialize a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to deinitialize.
 * @param size Size of each element in bytes.
 */
FLECS_API
void ecs_vec_fini(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector deinitialization.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to deinitialize.
 * @param T The element type.
 */
#define ecs_vec_fini_t(allocator, vec, T) \
    ecs_vec_fini(allocator, vec, ECS_SIZEOF(T))

/** Reset a vector. Keeps allocated memory for reuse.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reset.
 * @param size Size of each element in bytes.
 * @return Pointer to the reset vector.
 */
FLECS_API
ecs_vec_t* ecs_vec_reset(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector reset.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reset.
 * @param T The element type.
 */
#define ecs_vec_reset_t(allocator, vec, T) \
    ecs_vec_reset(allocator, vec, ECS_SIZEOF(T))

/** Clear a vector. Sets count to zero without freeing memory.
 *
 * @param vec The vector to clear.
 */
FLECS_API
void ecs_vec_clear(
    ecs_vec_t *vec);

/** Append a new element to the vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to append to.
 * @param size Size of each element in bytes.
 * @return Pointer to the newly appended element.
 */
FLECS_API
void* ecs_vec_append(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe element append.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to append to.
 * @param T The element type.
 * @return Typed pointer to the newly appended element.
 */
#define ecs_vec_append_t(allocator, vec, T) \
    ECS_CAST(T*, ecs_vec_append(allocator, vec, ECS_SIZEOF(T)))

/** Remove an element by swapping with the last element.
 *
 * @param vec The vector to remove from.
 * @param size Size of each element in bytes.
 * @param elem Index of the element to remove.
 */
FLECS_API
void ecs_vec_remove(
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem);

/** Type-safe element removal (swap with last).
 *
 * @param vec The vector to remove from.
 * @param T The element type.
 * @param elem Index of the element to remove.
 */
#define ecs_vec_remove_t(vec, T, elem) \
    ecs_vec_remove(vec, ECS_SIZEOF(T), elem)

/** Remove an element while preserving order.
 *
 * @param v The vector to remove from.
 * @param size Size of each element in bytes.
 * @param index Index of the element to remove.
 */
FLECS_API
void ecs_vec_remove_ordered(
    ecs_vec_t *v,
    ecs_size_t size,
    int32_t index);

/** Type-safe ordered element removal.
 *
 * @param vec The vector to remove from.
 * @param T The element type.
 * @param elem Index of the element to remove.
 */
#define ecs_vec_remove_ordered_t(vec, T, elem) \
    ecs_vec_remove_ordered(vec, ECS_SIZEOF(T), elem)

/** Remove the last element from the vector.
 *
 * @param vec The vector to remove from.
 */
FLECS_API
void ecs_vec_remove_last(
    ecs_vec_t *vec);

/** Copy a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param size Size of each element in bytes.
 * @return A new vector containing copies of all elements.
 */
FLECS_API
ecs_vec_t ecs_vec_copy(
    struct ecs_allocator_t *allocator,
    const ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector copy.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param T The element type.
 */
#define ecs_vec_copy_t(allocator, vec, T) \
    ecs_vec_copy(allocator, vec, ECS_SIZEOF(T))

/** Copy a vector and shrink to fit.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param size Size of each element in bytes.
 * @return A new vector with capacity shrunk to its count.
 */
FLECS_API
ecs_vec_t ecs_vec_copy_shrink(
    struct ecs_allocator_t *allocator,
    const ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector copy and shrink.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param T The element type.
 */
#define ecs_vec_copy_shrink_t(allocator, vec, T) \
    ecs_vec_copy_shrink(allocator, vec, ECS_SIZEOF(T))

/** Reclaim unused memory. Shrinks the vector's allocation to fit its count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reclaim memory from.
 * @param size Size of each element in bytes.
 */
FLECS_API
void ecs_vec_reclaim(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe memory reclaim.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reclaim memory from.
 * @param T The element type.
 */
#define ecs_vec_reclaim_t(allocator, vec, T) \
    ecs_vec_reclaim(allocator, vec, ECS_SIZEOF(T))

/** Set the capacity of a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param size Size of each element in bytes.
 * @param elem_count Desired capacity in number of elements.
 */
FLECS_API
void ecs_vec_set_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set capacity.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param T The element type.
 * @param elem_count Desired capacity in number of elements.
 */
#define ecs_vec_set_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_size(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the minimum capacity of a vector. Does not shrink.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum capacity in number of elements.
 */
FLECS_API
void ecs_vec_set_min_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set minimum capacity.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param T The element type.
 * @param elem_count Minimum capacity in number of elements.
 */
#define ecs_vec_set_min_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_size(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the minimum capacity using type info for lifecycle management.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum capacity in number of elements.
 * @param ti Type info for lifecycle callbacks.
 */
FLECS_API
void ecs_vec_set_min_size_w_type_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const ecs_type_info_t *ti);

/** Set the minimum count. Increases count if smaller than elem_count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum element count.
 */
FLECS_API
void ecs_vec_set_min_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set minimum count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param T The element type.
 * @param elem_count Minimum element count.
 */
#define ecs_vec_set_min_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_count(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the minimum count and zero-initialize new elements.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum element count.
 */
FLECS_API
void ecs_vec_set_min_count_zeromem(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set minimum count with zero-initialization.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param T The element type.
 * @param elem_count Minimum element count.
 */
#define ecs_vec_set_min_count_zeromem_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_count_zeromem(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the element count of a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Desired element count.
 */
FLECS_API
void ecs_vec_set_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set element count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param T The element type.
 * @param elem_count Desired element count.
 */
#define ecs_vec_set_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_count(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the element count using type info for lifecycle management.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Desired element count.
 * @param ti Type info for lifecycle callbacks.
 */
FLECS_API
void ecs_vec_set_count_w_type_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const ecs_type_info_t *ti);

/** Set the minimum count using type info for lifecycle management.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum element count.
 * @param ti Type info for lifecycle callbacks.
 */
FLECS_API
void ecs_vec_set_min_count_w_type_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const ecs_type_info_t *ti);

/** Grow the vector by a number of elements.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to grow.
 * @param size Size of each element in bytes.
 * @param elem_count Number of elements to grow by.
 * @return Pointer to the first newly added element.
 */
FLECS_API
void* ecs_vec_grow(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe vector grow.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to grow.
 * @param T The element type.
 * @param elem_count Number of elements to grow by.
 */
#define ecs_vec_grow_t(allocator, vec, T, elem_count) \
    ecs_vec_grow(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Return the number of elements in the vector.
 *
 * @param vec The vector.
 * @return The number of elements.
 */
FLECS_API
int32_t ecs_vec_count(
    const ecs_vec_t *vec);

/** Return the allocated capacity of the vector.
 *
 * @param vec The vector.
 * @return The allocated capacity in number of elements.
 */
FLECS_API
int32_t ecs_vec_size(
    const ecs_vec_t *vec);

/** Get a pointer to an element at the given index.
 *
 * @param vec The vector.
 * @param size Size of each element in bytes.
 * @param index Index of the element to retrieve.
 * @return Pointer to the element.
 */
FLECS_API
void* ecs_vec_get(
    const ecs_vec_t *vec,
    ecs_size_t size,
    int32_t index);

/** Type-safe element access by index.
 *
 * @param vec The vector.
 * @param T The element type.
 * @param index Index of the element to retrieve.
 * @return Typed pointer to the element.
 */
#define ecs_vec_get_t(vec, T, index) \
    ECS_CAST(T*, ecs_vec_get(vec, ECS_SIZEOF(T), index))

/** Get a pointer to the first element.
 *
 * @param vec The vector.
 * @return Pointer to the first element, or NULL if empty.
 */
FLECS_API
void* ecs_vec_first(
    const ecs_vec_t *vec);

/** Type-safe access to the first element.
 *
 * @param vec The vector.
 * @param T The element type.
 * @return Typed pointer to the first element, or NULL if empty.
 */
#define ecs_vec_first_t(vec, T) \
    ECS_CAST(T*, ecs_vec_first(vec))

/** Get a pointer to the last element.
 *
 * @param vec The vector.
 * @param size Size of each element in bytes.
 * @return Pointer to the last element, or NULL if empty.
 */
FLECS_API
void* ecs_vec_last(
    const ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe access to the last element.
 *
 * @param vec The vector.
 * @param T The element type.
 * @return Typed pointer to the last element, or NULL if empty.
 */
#define ecs_vec_last_t(vec, T) \
    ECS_CAST(T*, ecs_vec_last(vec, ECS_SIZEOF(T)))

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H

#ifdef __cplusplus
extern "C" {
#endif

/** The number of elements in a single page. */
#define FLECS_SPARSE_PAGE_SIZE (1 << FLECS_SPARSE_PAGE_BITS)

/** Compute the page index from an ID by stripping the first 12 bits. */
#define FLECS_SPARSE_PAGE(index) ((int32_t)((uint32_t)index >> FLECS_SPARSE_PAGE_BITS))

/** Compute the offset of an index inside a page. */
#define FLECS_SPARSE_OFFSET(index) ((int32_t)index & (FLECS_SPARSE_PAGE_SIZE - 1))

/** A page in the sparse set containing a sparse-to-dense mapping and data. */
typedef struct ecs_sparse_page_t {
    int32_t *sparse;            /**< Sparse array with indices to dense array. */
    void *data;                 /**< Store data in sparse array to reduce
                                 *   indirection and provide stable pointers. */
} ecs_sparse_page_t;

/** A sparse set data structure for O(1) access with stable pointers. */
typedef struct ecs_sparse_t {
    ecs_vec_t dense;            /**< Dense array with indices to sparse array. The
                                 *   dense array stores both alive and not alive
                                 *   sparse indices. The 'count' member keeps
                                 *   track of which indices are alive. */
    ecs_vec_t pages;            /**< Chunks with sparse arrays and data. */
    ecs_size_t size;            /**< Element size in bytes. */
    int32_t count;              /**< Number of alive entries. */
    uint64_t max_id;            /**< Local max index (if no global is set). */
    struct ecs_allocator_t *allocator;       /**< Allocator for general allocations. */
    struct ecs_block_allocator_t *page_allocator; /**< Allocator for page allocations. */
} ecs_sparse_t;

/** Initialize a sparse set.
 *
 * @param result The sparse set to initialize.
 * @param allocator Allocator for general memory management.
 * @param page_allocator Block allocator for page allocations.
 * @param size Size of each element in bytes.
 */
FLECS_DBG_API
void flecs_sparse_init(
    ecs_sparse_t *result,
    struct ecs_allocator_t *allocator,
    struct ecs_block_allocator_t *page_allocator,
    ecs_size_t size);

/** Typed sparse set initialization.
 *
 * @param result The sparse set to initialize.
 * @param allocator Allocator for general memory management.
 * @param page_allocator Block allocator for page allocations.
 * @param T The element type.
 */
#define flecs_sparse_init_t(result, allocator, page_allocator, T)\
    flecs_sparse_init(result, allocator, page_allocator, ECS_SIZEOF(T))

/** Deinitialize a sparse set.
 *
 * @param sparse The sparse set to deinitialize.
 */
FLECS_DBG_API
void flecs_sparse_fini(
    ecs_sparse_t *sparse);

/** Remove all elements from a sparse set.
 *
 * @param sparse The sparse set to clear.
 */
FLECS_DBG_API
void flecs_sparse_clear(
    ecs_sparse_t *sparse);

/** Add an element to a sparse set. This generates or recycles an ID.
 *
 * @param sparse The sparse set to add to.
 * @param elem_size Size of each element in bytes.
 * @return Pointer to the newly added element.
 */
FLECS_DBG_API
void* flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

/** Typed element add.
 *
 * @param sparse The sparse set to add to.
 * @param T The element type.
 * @return Typed pointer to the newly added element.
 */
#define flecs_sparse_add_t(sparse, T)\
    ECS_CAST(T*, flecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get the last issued ID.
 *
 * @param sparse The sparse set.
 * @return The last issued ID.
 */
FLECS_DBG_API
uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Generate or recycle a new ID.
 *
 * @param sparse The sparse set.
 * @return A new or recycled ID.
 */
FLECS_DBG_API
uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse);

/** Remove an element.
 *
 * @param sparse The sparse set to remove from.
 * @param size Size of each element in bytes.
 * @param id The ID of the element to remove.
 * @return True if the element was found and removed.
 */
FLECS_DBG_API
bool flecs_sparse_remove(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id);

/** Typed element removal.
 *
 * @param sparse The sparse set to remove from.
 * @param T The element type.
 * @param id The ID of the element to remove.
 */
#define flecs_sparse_remove_t(sparse, T, id)\
    flecs_sparse_remove(sparse, ECS_SIZEOF(T), id)

/** Remove an element and increase the generation.
 *
 * @param sparse The sparse set to remove from.
 * @param size Size of each element in bytes.
 * @param id The ID of the element to remove.
 * @return True if the element was found and removed.
 */
bool flecs_sparse_remove_w_gen(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id);

/** Typed element removal with generation increase.
 *
 * @param sparse The sparse set to remove from.
 * @param T The element type.
 * @param id The ID of the element to remove.
 */
#define flecs_sparse_remove_w_gen_t(sparse, T, id)\
    flecs_sparse_remove_w_gen(sparse, ECS_SIZEOF(T), id)

/** Test if an ID is alive, which requires the generation count to match.
 *
 * @param sparse The sparse set to check.
 * @param id The ID to test for liveness.
 * @return True if the ID is alive.
 */
FLECS_DBG_API
bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get a value from a sparse set by dense ID. This function is useful in
 * combination with flecs_sparse_count() for iterating all values in the set.
 *
 * @param sparse The sparse set to retrieve from.
 * @param elem_size Size of each element in bytes.
 * @param index Dense index of the element.
 * @return Pointer to the element at the given dense index.
 */
FLECS_DBG_API
void* flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

/** Typed get by dense index.
 *
 * @param sparse The sparse set to retrieve from.
 * @param T The element type.
 * @param index Dense index of the element.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_get_dense_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

/** Get the number of alive elements in the sparse set.
 *
 * @param sparse The sparse set.
 * @return The number of alive elements.
 */
FLECS_DBG_API
int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Check if a sparse set has an ID.
 *
 * @param sparse The sparse set to check.
 * @param id The ID to look for.
 * @return True if the sparse set contains the ID.
 */
bool flecs_sparse_has(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get element by sparse ID, regardless of whether the element is alive or not.
 *
 * @param sparse The sparse set to retrieve from.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID of the element.
 * @return Pointer to the element, regardless of liveness.
 */
FLECS_DBG_API
void* flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed get by sparse ID.
 *
 * @param sparse The sparse set to retrieve from.
 * @param T The element type.
 * @param index The sparse ID of the element.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_get_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get(sparse, ECS_SIZEOF(T), index))

/** Create an element by (sparse) ID.
 *
 * @param sparse The sparse set to insert into.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID for the new element.
 * @return Pointer to the newly created element.
 */
FLECS_DBG_API
void* flecs_sparse_insert(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed insert by sparse ID.
 *
 * @param sparse The sparse set to insert into.
 * @param T The element type.
 * @param index The sparse ID for the new element.
 * @return Typed pointer to the newly created element.
 */
#define flecs_sparse_insert_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_insert(sparse, ECS_SIZEOF(T), index))

/** Get or create an element by (sparse) ID.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID to get or create.
 * @param is_new Output parameter set to true if a new element was created.
 * @return Pointer to the existing or newly created element.
 */
FLECS_DBG_API
void* flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id,
    bool *is_new);

/** Typed get or create by sparse ID.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index The sparse ID to get or create.
 * @param is_new Output parameter set to true if a new element was created.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_ensure_t(sparse, T, index, is_new)\
    ECS_CAST(T*, flecs_sparse_ensure(sparse, ECS_SIZEOF(T), index, is_new))

/** Fast version of ensure with no liveness checking.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID to get or create.
 * @return Pointer to the element.
 */
FLECS_DBG_API
void* flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed fast ensure without liveness checking.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index The sparse ID to get or create.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_ensure_fast_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_ensure_fast(sparse, ECS_SIZEOF(T), index))

/** Get a pointer to IDs (alive and not alive). Use with flecs_sparse_count().
 *
 * @param sparse The sparse set.
 * @return Pointer to the dense array of IDs.
 */
FLECS_DBG_API
const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse);

/** Shrink sparse set memory to fit current usage.
 *
 * @param sparse The sparse set to shrink.
 */
FLECS_DBG_API
void flecs_sparse_shrink(
    ecs_sparse_t *sparse);

/** Exposed but unstable APIs.
 * These APIs are visible in the header but not part of the stable public API,
 * and as a result may change without notice. */

/** Initialize a public sparse set.
 *
 * @param sparse The sparse set to initialize.
 * @param elem_size Size of each element in bytes.
 */
FLECS_API
void ecs_sparse_init(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

/** Typed public sparse set initialization.
 *
 * @param sparse The sparse set to initialize.
 * @param T The element type.
 */
#define ecs_sparse_init_t(sparse, T)\
    ecs_sparse_init(sparse, ECS_SIZEOF(T))

/** Add an element to a public sparse set.
 *
 * @param sparse The sparse set to add to.
 * @param elem_size Size of each element in bytes.
 * @return Pointer to the newly added element.
 */
FLECS_API
void* ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

/** Typed public sparse set add.
 *
 * @param sparse The sparse set to add to.
 * @param T The element type.
 * @return Typed pointer to the newly added element.
 */
#define ecs_sparse_add_t(sparse, T)\
    ECS_CAST(T*, ecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get the last issued ID from a public sparse set.
 *
 * @param sparse The sparse set.
 * @return The last issued ID.
 */
FLECS_API
uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Get the number of alive elements in a public sparse set.
 *
 * @param sparse The sparse set.
 * @return The number of alive elements.
 */
FLECS_API
int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Get a value from a public sparse set by dense index.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param index Dense index of the element.
 * @return Pointer to the element.
 */
FLECS_API
void* ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

/** Typed public get by dense index.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index Dense index of the element.
 * @return Typed pointer to the element.
 */
#define ecs_sparse_get_dense_t(sparse, T, index)\
    ECS_CAST(T*, ecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

/** Get a value from a public sparse set by sparse ID.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID of the element.
 * @return Pointer to the element.
 */
FLECS_API
void* ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed public get by sparse ID.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index The sparse ID of the element.
 * @return Typed pointer to the element.
 */
#define ecs_sparse_get_t(sparse, T, index)\
    ECS_CAST(T*, ecs_sparse_get(sparse, ECS_SIZEOF(T), index))

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_BLOCK_ALLOCATOR_H
#define FLECS_BLOCK_ALLOCATOR_H

/** Forward declaration of map type. */
typedef struct ecs_map_t ecs_map_t;

/** A block of memory managed by the block allocator. */
typedef struct ecs_block_allocator_block_t {
    void *memory; /**< Pointer to the block memory. */
    struct ecs_block_allocator_block_t *next; /**< Next block in the list. */
} ecs_block_allocator_block_t;

/** Header for a free chunk in the block allocator free list. */
typedef struct ecs_block_allocator_chunk_header_t {
    struct ecs_block_allocator_chunk_header_t *next; /**< Next free chunk. */
} ecs_block_allocator_chunk_header_t;

/** Block allocator that returns fixed-size memory blocks. */
typedef struct ecs_block_allocator_t {
    int32_t data_size; /**< Size of each allocation. */
#ifndef FLECS_USE_OS_ALLOC
    int32_t chunk_size; /**< Aligned chunk size including header. */
    int32_t chunks_per_block; /**< Number of chunks per block. */
    int32_t block_size; /**< Total size of each allocated block. */
    ecs_block_allocator_chunk_header_t *head; /**< Head of the free chunk list. */
    ecs_block_allocator_block_t *block_head; /**< Head of the allocated block list. */
#ifdef FLECS_SANITIZE
    int32_t alloc_count; /**< Number of outstanding allocations (sanitizer only). */
    ecs_map_t *outstanding; /**< Map of outstanding allocations (sanitizer only). */
#endif
#endif
} ecs_block_allocator_t;

/** Initialize a block allocator.
 *
 * @param ba The block allocator to initialize.
 * @param size The size of each allocation.
 */
FLECS_API
void flecs_ballocator_init(
    ecs_block_allocator_t *ba,
    ecs_size_t size);

/** Initialize a block allocator for type T. */
#define flecs_ballocator_init_t(ba, T)\
    flecs_ballocator_init(ba, ECS_SIZEOF(T))

/** Initialize a block allocator for count elements of type T. */
#define flecs_ballocator_init_n(ba, T, count)\
    flecs_ballocator_init(ba, ECS_SIZEOF(T) * count)

/** Create a new block allocator on the heap.
 *
 * @param size The size of each allocation.
 * @return The new block allocator.
 */
FLECS_API
ecs_block_allocator_t* flecs_ballocator_new(
    ecs_size_t size);

/** Create a new block allocator for type T. */
#define flecs_ballocator_new_t(T)\
    flecs_ballocator_new(ECS_SIZEOF(T))

/** Create a new block allocator for count elements of type T. */
#define flecs_ballocator_new_n(T, count)\
    flecs_ballocator_new(ECS_SIZEOF(T) * count)

/** Deinitialize a block allocator.
 *
 * @param ba The block allocator to deinitialize.
 */
FLECS_API
void flecs_ballocator_fini(
    ecs_block_allocator_t *ba);

/** Free a block allocator created with flecs_ballocator_new().
 *
 * @param ba The block allocator to free.
 */
FLECS_API
void flecs_ballocator_free(
    ecs_block_allocator_t *ba);

/** Allocate a block of memory.
 *
 * @param allocator The block allocator.
 * @return Pointer to the allocated memory.
 */
FLECS_API
void* flecs_balloc(
    ecs_block_allocator_t *allocator);

/** Allocate a block of memory with debug type name info.
 *
 * @param allocator The block allocator.
 * @param type_name The type name for debug tracking.
 * @return Pointer to the allocated memory.
 */
FLECS_API
void* flecs_balloc_w_dbg_info(
    ecs_block_allocator_t *allocator,
    const char *type_name);

/** Allocate a zeroed block of memory.
 *
 * @param allocator The block allocator.
 * @return Pointer to the zeroed memory.
 */
FLECS_API
void* flecs_bcalloc(
    ecs_block_allocator_t *allocator);

/** Allocate a zeroed block of memory with debug type name info.
 *
 * @param allocator The block allocator.
 * @param type_name The type name for debug tracking.
 * @return Pointer to the zeroed memory.
 */
FLECS_API
void* flecs_bcalloc_w_dbg_info(
    ecs_block_allocator_t *allocator,
    const char *type_name);

/** Free a block of memory.
 *
 * @param allocator The block allocator.
 * @param memory The memory to free.
 */
FLECS_API
void flecs_bfree(
    ecs_block_allocator_t *allocator,
    void *memory);

/** Free a block of memory with debug type name info.
 *
 * @param allocator The block allocator.
 * @param memory The memory to free.
 * @param type_name The type name for debug tracking.
 */
FLECS_API
void flecs_bfree_w_dbg_info(
    ecs_block_allocator_t *allocator,
    void *memory,
    const char *type_name);

/** Reallocate a block from one block allocator to another.
 *
 * @param dst The destination block allocator.
 * @param src The source block allocator.
 * @param memory The memory to reallocate.
 * @return Pointer to the reallocated memory.
 */
FLECS_API
void* flecs_brealloc(
    ecs_block_allocator_t *dst,
    ecs_block_allocator_t *src,
    void *memory);

/** Reallocate a block with debug type name info.
 *
 * @param dst The destination block allocator.
 * @param src The source block allocator.
 * @param memory The memory to reallocate.
 * @param type_name The type name for debug tracking.
 * @return Pointer to the reallocated memory.
 */
FLECS_API
void* flecs_brealloc_w_dbg_info(
    ecs_block_allocator_t *dst,
    ecs_block_allocator_t *src,
    void *memory,
    const char *type_name);

/** Duplicate a block of memory.
 *
 * @param ba The block allocator.
 * @param memory The memory to duplicate.
 * @return Pointer to the duplicated memory.
 */
FLECS_API
void* flecs_bdup(
    ecs_block_allocator_t *ba,
    void *memory);

#endif

#ifndef FLECS_STACK_ALLOCATOR_H
#define FLECS_STACK_ALLOCATOR_H

/** A page of memory in the stack allocator. */
typedef struct ecs_stack_page_t {
    void *data; /**< Pointer to the page data. */
    struct ecs_stack_page_t *next; /**< Next page in the list. */
    int16_t sp; /**< Current stack pointer within the page. */
    uint32_t id; /**< Page identifier. */
} ecs_stack_page_t;

/** Cursor that marks a position in the stack allocator for later restoration. */
typedef struct ecs_stack_cursor_t {
    struct ecs_stack_cursor_t *prev; /**< Previous cursor in the stack. */
    struct ecs_stack_page_t *page; /**< Page at the cursor position. */
    int16_t sp; /**< Stack pointer at the cursor position. */
    bool is_free; /**< Whether this cursor has been freed. */
#ifdef FLECS_DEBUG
    struct ecs_stack_t *owner; /**< Stack allocator that owns this cursor (debug only). */
#endif
} ecs_stack_cursor_t;

/** Stack allocator for quick allocation of small temporary values. */
typedef struct ecs_stack_t {
    ecs_stack_page_t *first; /**< First page in the stack. */
    ecs_stack_page_t *tail_page; /**< Current tail page. */
    ecs_stack_cursor_t *tail_cursor; /**< Current tail cursor. */
#ifdef FLECS_DEBUG
    int32_t cursor_count; /**< Number of active cursors (debug only). */
#endif
} ecs_stack_t;

/** Offset of usable data within a stack page (aligned to 16 bytes). */
#define FLECS_STACK_PAGE_OFFSET ECS_ALIGN(ECS_SIZEOF(ecs_stack_page_t), 16)

/** Size of usable data within a stack page. */
#define FLECS_STACK_PAGE_SIZE (1024 - FLECS_STACK_PAGE_OFFSET)

/** Initialize a stack allocator.
 *
 * @param stack The stack allocator to initialize.
 */
FLECS_DBG_API
void flecs_stack_init(
    ecs_stack_t *stack);

/** Deinitialize a stack allocator.
 *
 * @param stack The stack allocator to deinitialize.
 */
FLECS_DBG_API
void flecs_stack_fini(
    ecs_stack_t *stack);

/** Allocate memory from the stack.
 *
 * @param stack The stack allocator.
 * @param size The allocation size.
 * @param align The required alignment.
 * @return Pointer to the allocated memory.
 */
FLECS_DBG_API
void* flecs_stack_alloc(
    ecs_stack_t *stack,
    ecs_size_t size,
    ecs_size_t align);

/** Allocate memory for a single element of type T from the stack. */
#define flecs_stack_alloc_t(stack, T)\
    flecs_stack_alloc(stack, ECS_SIZEOF(T), ECS_ALIGNOF(T))

/** Allocate memory for count elements of type T from the stack. */
#define flecs_stack_alloc_n(stack, T, count)\
    flecs_stack_alloc(stack, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

/** Allocate zeroed memory from the stack.
 *
 * @param stack The stack allocator.
 * @param size The allocation size.
 * @param align The required alignment.
 * @return Pointer to the zeroed memory.
 */
FLECS_DBG_API
void* flecs_stack_calloc(
    ecs_stack_t *stack,
    ecs_size_t size,
    ecs_size_t align);

/** Allocate zeroed memory for a single element of type T from the stack. */
#define flecs_stack_calloc_t(stack, T)\
    flecs_stack_calloc(stack, ECS_SIZEOF(T), ECS_ALIGNOF(T))

/** Allocate zeroed memory for count elements of type T from the stack. */
#define flecs_stack_calloc_n(stack, T, count)\
    flecs_stack_calloc(stack, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

/** Free memory allocated from the stack.
 *
 * @param ptr The pointer to free.
 * @param size The size of the allocation.
 */
FLECS_DBG_API
void flecs_stack_free(
    void *ptr,
    ecs_size_t size);

/** Free memory for a single element of type T from the stack. */
#define flecs_stack_free_t(ptr, T)\
    flecs_stack_free(ptr, ECS_SIZEOF(T))

/** Free memory for count elements of type T from the stack. */
#define flecs_stack_free_n(ptr, T, count)\
    flecs_stack_free(ptr, ECS_SIZEOF(T) * count)

/** Reset the stack allocator.
 *
 * @param stack The stack allocator to reset.
 */
void flecs_stack_reset(
    ecs_stack_t *stack);

/** Get a cursor marking the current position in the stack.
 *
 * @param stack The stack allocator.
 * @return A cursor that can be used to restore the stack.
 */
FLECS_DBG_API
ecs_stack_cursor_t* flecs_stack_get_cursor(
    ecs_stack_t *stack);

/** Restore the stack to a previously saved cursor position.
 *
 * @param stack The stack allocator.
 * @param cursor The cursor to restore to.
 */
FLECS_DBG_API
void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    ecs_stack_cursor_t *cursor);

#endif

#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/** Data type for map key-value storage. */
typedef uint64_t ecs_map_data_t;

/** Map key type. */
typedef ecs_map_data_t ecs_map_key_t;

/** Map value type. */
typedef ecs_map_data_t ecs_map_val_t;

/** A single entry in a map bucket (linked list node). */
typedef struct ecs_bucket_entry_t {
    ecs_map_key_t key;                /**< Key of the entry. */
    ecs_map_val_t value;              /**< Value of the entry. */
    struct ecs_bucket_entry_t *next;  /**< Next entry in the bucket chain. */
} ecs_bucket_entry_t;

/** A bucket in the map hash table. */
typedef struct ecs_bucket_t {
    ecs_bucket_entry_t *first;        /**< First entry in this bucket. */
} ecs_bucket_t;

/** A hashmap data structure. */
struct ecs_map_t {
    ecs_bucket_t *buckets;            /**< Array of hash buckets. */
    int32_t bucket_count;             /**< Total number of buckets. */
    unsigned count : 26;              /**< Number of elements in the map. */
    unsigned bucket_shift : 6;        /**< Bit shift for bucket index computation. */
    struct ecs_allocator_t *allocator; /**< Allocator used for memory management. */
#ifdef FLECS_DEBUG
    int32_t change_count;             /**< Track modifications while iterating. */
    ecs_map_key_t last_iterated;      /**< Currently iterated element. */
#endif
};

/** Iterator for traversing map contents. */
typedef struct ecs_map_iter_t {
    const ecs_map_t *map;             /**< The map being iterated. */
    ecs_bucket_t *bucket;             /**< Current bucket. */
    ecs_bucket_entry_t *entry;        /**< Current entry in the bucket. */
    ecs_map_data_t *res;              /**< Pointer to current key-value pair. */
#ifdef FLECS_DEBUG
    int32_t change_count;             /**< Change count at iterator creation for modification detection. */
#endif
} ecs_map_iter_t;

/** Function and macro postfix meanings:
 *   - _ptr:    Access ecs_map_val_t as void*.
 *   - _ref:    Access ecs_map_val_t* as T**.
 *   - _deref:  Dereference a _ref.
 *   - _alloc:  If _ptr is NULL, alloc.
 *   - _free:   If _ptr is not NULL, free.
 */

/** Initialize a new map.
 *
 * @param map The map to initialize.
 * @param allocator Allocator to use for memory management.
 */
FLECS_API
void ecs_map_init(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

/** Initialize a new map if uninitialized, leave as is otherwise.
 *
 * @param map The map to initialize.
 * @param allocator Allocator to use for memory management.
 */
FLECS_API
void ecs_map_init_if(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

/** Reclaim map memory.
 *
 * @param map The map to reclaim memory from.
 */
FLECS_API
void ecs_map_reclaim(
    ecs_map_t *map);

/** Deinitialize a map.
 *
 * @param map The map to deinitialize.
 */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Get an element for a key. Returns NULL if the key doesn't exist.
 *
 * @param map The map to search.
 * @param key The key to look up.
 * @return Pointer to the value, or NULL if the key was not found.
 */
FLECS_API
ecs_map_val_t* ecs_map_get(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get element as pointer (auto-dereferences _ptr).
 *
 * @param map The map to search.
 * @param key The key to look up.
 * @return Dereferenced pointer value, or NULL if the key was not found.
 */
FLECS_API
void* ecs_map_get_deref_(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert an element for a key.
 *
 * @param map The map to get or insert into.
 * @param key The key to look up or insert.
 * @return Pointer to the existing or newly inserted value.
 */
FLECS_API
ecs_map_val_t* ecs_map_ensure(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert a pointer element for a key. Allocate if the pointer is NULL.
 *
 * @param map The map to get or insert into.
 * @param elem_size Size of the element to allocate.
 * @param key The key to look up or insert.
 * @return Pointer to the existing or newly allocated element.
 */
FLECS_API
void* ecs_map_ensure_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Insert an element for a key.
 *
 * @param map The map to insert into.
 * @param key The key for the new element.
 * @param value The value to insert.
 */
FLECS_API
void ecs_map_insert(
    ecs_map_t *map,
    ecs_map_key_t key,
    ecs_map_val_t value);

/** Insert a pointer element for a key, populate with a new allocation.
 *
 * @param map The map to insert into.
 * @param elem_size Size of the element to allocate.
 * @param key The key for the new element.
 * @return Pointer to the newly allocated element.
 */
FLECS_API
void* ecs_map_insert_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Remove a key from the map.
 *
 * @param map The map to remove from.
 * @param key The key to remove.
 * @return The removed value.
 */
FLECS_API
ecs_map_val_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove a pointer element. Free if not NULL.
 *
 * @param map The map to remove from.
 * @param key The key to remove and free.
 */
FLECS_API
void ecs_map_remove_free(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove all elements from the map.
 *
 * @param map The map to clear.
 */
FLECS_API
void ecs_map_clear(
    ecs_map_t *map);

/** Return the number of elements in the map. */
#define ecs_map_count(map) ((map) ? (map)->count : 0)

/** Is the map initialized? */
#define ecs_map_is_init(map) ((map) ? (map)->bucket_shift != 0 : false)

/** Return an iterator to map contents.
 *
 * @param map The map to iterate.
 * @return A new iterator positioned before the first element.
 */
FLECS_API
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

/** Return whether the map iterator is valid.
 *
 * @param iter The iterator to check.
 * @return True if the iterator is valid.
 */
FLECS_API
bool ecs_map_iter_valid(
    ecs_map_iter_t *iter);

/** Obtain the next element in the map from the iterator.
 *
 * @param iter The iterator to advance.
 * @return True if a next element was found, false if iteration is done.
 */
FLECS_API
bool ecs_map_next(
    ecs_map_iter_t *iter);

/** Copy a map.
 *
 * @param dst The destination map.
 * @param src The source map to copy from.
 */
FLECS_API
void ecs_map_copy(
    ecs_map_t *dst,
    const ecs_map_t *src);

/** Get value as a typed reference (T**). */
#define ecs_map_get_ref(m, T, k) ECS_CAST(T**, ecs_map_get(m, k))

/** Get value as a typed dereferenced pointer (T*). */
#define ecs_map_get_deref(m, T, k) ECS_CAST(T*, ecs_map_get_deref_(m, k))

/** Get value as a void pointer. */
#define ecs_map_get_ptr(m, k) ECS_CAST(void*, ecs_map_get_deref_(m, k))

/** Ensure a typed reference (T**) for the key. */
#define ecs_map_ensure_ref(m, T, k) ECS_CAST(T**, ecs_map_ensure(m, k))

/** Insert a pointer value into the map. */
#define ecs_map_insert_ptr(m, k, v) ecs_map_insert(m, k, ECS_CAST(ecs_map_val_t, ECS_PTR_CAST(uintptr_t, v)))

/** Type-safe insert with allocation. */
#define ecs_map_insert_alloc_t(m, T, k) ECS_CAST(T*, ecs_map_insert_alloc(m, ECS_SIZEOF(T), k))

/** Type-safe ensure with allocation. */
#define ecs_map_ensure_alloc_t(m, T, k) ECS_PTR_CAST(T*, (uintptr_t)ecs_map_ensure_alloc(m, ECS_SIZEOF(T), k))

/** Remove a pointer element from the map. */
#define ecs_map_remove_ptr(m, k) (ECS_PTR_CAST(void*, ECS_CAST(uintptr_t, (ecs_map_remove(m, k)))))

/** Get the key from an iterator. */
#define ecs_map_key(it) ((it)->res[0])

/** Get the value from an iterator. */
#define ecs_map_value(it) ((it)->res[1])

/** Get the value from an iterator as a void pointer. */
#define ecs_map_ptr(it) ECS_PTR_CAST(void*, ECS_CAST(uintptr_t, ecs_map_value(it)))

/** Get the value from an iterator as a typed reference (T**). */
#define ecs_map_ref(it, T) (ECS_CAST(T**, &((it)->res[1])))

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_ALLOCATOR_H
#define FLECS_ALLOCATOR_H

FLECS_DBG_API extern int64_t ecs_block_allocator_alloc_count; /**< Block allocator allocation count. */
FLECS_DBG_API extern int64_t ecs_block_allocator_free_count; /**< Block allocator free count. */
FLECS_DBG_API extern int64_t ecs_stack_allocator_alloc_count; /**< Stack allocator allocation count. */
FLECS_DBG_API extern int64_t ecs_stack_allocator_free_count; /**< Stack allocator free count. */

/** General purpose allocator that manages block allocators for different sizes. */
struct ecs_allocator_t {
#ifndef FLECS_USE_OS_ALLOC
    ecs_block_allocator_t chunks; /**< Block allocator for chunk storage. */
    struct ecs_sparse_t sizes; /**< Sparse set mapping size to block allocator. */
#else
    bool dummy; /**< Unused member for OS allocator fallback. */
#endif
};

/** Initialize an allocator.
 *
 * @param a The allocator to initialize.
 */
FLECS_API
void flecs_allocator_init(
    ecs_allocator_t *a);

/** Deinitialize an allocator.
 *
 * @param a The allocator to deinitialize.
 */
FLECS_API
void flecs_allocator_fini(
    ecs_allocator_t *a);

/** Get or create a block allocator for the specified size.
 *
 * @param a The allocator.
 * @param size The allocation size.
 * @return The block allocator for the given size.
 */
FLECS_API
ecs_block_allocator_t* flecs_allocator_get(
    ecs_allocator_t *a,
    ecs_size_t size);

/** Duplicate a string using the allocator.
 *
 * @param a The allocator.
 * @param str The string to duplicate.
 * @return The duplicated string.
 */
FLECS_API
char* flecs_strdup(
    ecs_allocator_t *a,
    const char* str);

/** Free a string previously allocated with flecs_strdup().
 *
 * @param a The allocator.
 * @param str The string to free.
 */
FLECS_API
void flecs_strfree(
    ecs_allocator_t *a,
    char* str);

/** Duplicate a memory block using the allocator.
 *
 * @param a The allocator.
 * @param size The size of the memory block.
 * @param src The source memory to duplicate.
 * @return Pointer to the duplicated memory.
 */
FLECS_API
void* flecs_dup(
    ecs_allocator_t *a,
    ecs_size_t size,
    const void *src);

#ifndef FLECS_USE_OS_ALLOC

/** Get the dynamic allocator from an object. */
#define flecs_allocator(obj) (&obj->allocators.dyn)

/** Allocate memory of a given size. */
#define flecs_alloc(a, size) flecs_balloc(flecs_allocator_get(a, size))

/** Allocate memory with debug type name info. */
#define flecs_alloc_w_dbg_info(a, size, type_name) flecs_balloc_w_dbg_info(flecs_allocator_get(a, size), type_name)

/** Allocate memory for a single element of type T. */
#define flecs_alloc_t(a, T) flecs_alloc_w_dbg_info(a, ECS_SIZEOF(T), #T)

/** Allocate memory for count elements of type T. */
#define flecs_alloc_n(a, T, count) flecs_alloc_w_dbg_info(a, ECS_SIZEOF(T) * (count), #T)

/** Allocate zeroed memory of a given size. */
#define flecs_calloc(a, size) flecs_bcalloc(flecs_allocator_get(a, size))

/** Allocate zeroed memory with debug type name info. */
#define flecs_calloc_w_dbg_info(a, size, type_name) flecs_bcalloc_w_dbg_info(flecs_allocator_get(a, size), type_name)

/** Allocate zeroed memory for a single element of type T. */
#define flecs_calloc_t(a, T) flecs_calloc_w_dbg_info(a, ECS_SIZEOF(T), #T)

/** Allocate zeroed memory for count elements of type T. */
#define flecs_calloc_n(a, T, count) flecs_calloc_w_dbg_info(a, ECS_SIZEOF(T) * (count), #T)

/** Free memory of a given size. */
#define flecs_free(a, size, ptr)\
    flecs_bfree((ptr) ? flecs_allocator_get(a, size) : NULL, ptr)

/** Free memory for a single element of type T. */
#define flecs_free_t(a, T, ptr)\
    flecs_bfree_w_dbg_info((ptr) ? flecs_allocator_get(a, ECS_SIZEOF(T)) : NULL, ptr, #T)

/** Free memory for count elements of type T. */
#define flecs_free_n(a, T, count, ptr)\
    flecs_bfree_w_dbg_info((ptr) ? flecs_allocator_get(a, ECS_SIZEOF(T) * (count)) : NULL\
        , ptr, #T)

/** Reallocate memory from one size to another. */
#define flecs_realloc(a, size_dst, size_src, ptr)\
    flecs_brealloc(flecs_allocator_get(a, size_dst),\
    flecs_allocator_get(a, size_src),\
    ptr)

/** Reallocate memory with debug type name info. */
#define flecs_realloc_w_dbg_info(a, size_dst, size_src, ptr, type_name)\
    flecs_brealloc_w_dbg_info(flecs_allocator_get(a, size_dst),\
    flecs_allocator_get(a, size_src),\
    ptr,\
    type_name)

/** Reallocate memory for count elements of type T. */
#define flecs_realloc_n(a, T, count_dst, count_src, ptr)\
    flecs_realloc(a, ECS_SIZEOF(T) * (count_dst), ECS_SIZEOF(T) * (count_src), ptr)

/** Duplicate count elements of type T. */
#define flecs_dup_n(a, T, count, ptr) flecs_dup(a, ECS_SIZEOF(T) * (count), ptr)

#else

/** Allocate memory of a given size (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param size The allocation size.
 * @return Pointer to the allocated memory.
 */
void* flecs_alloc(
    ecs_allocator_t *a,
    ecs_size_t size);

/** Allocate zeroed memory of a given size (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param size The allocation size.
 * @return Pointer to the zeroed memory.
 */
void* flecs_calloc(
    ecs_allocator_t *a,
    ecs_size_t size);

/** Reallocate memory from one size to another (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param dst_size The new allocation size.
 * @param src_size The old allocation size.
 * @param ptr The pointer to reallocate.
 * @return Pointer to the reallocated memory.
 */
void* flecs_realloc(
    ecs_allocator_t *a,
    ecs_size_t dst_size,
    ecs_size_t src_size,
    void *ptr);

/** Free memory of a given size (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param size The allocation size.
 * @param ptr The pointer to free.
 */
void flecs_free(
    ecs_allocator_t *a,
    ecs_size_t size,
    void *ptr);

/** Allocate memory with debug type name info (OS allocator fallback). */
#define flecs_alloc_w_dbg_info(a, size, type_name) flecs_alloc(a, size)

/** Allocate memory for a single element of type T (OS allocator fallback). */
#define flecs_alloc_t(a, T) flecs_alloc(a, ECS_SIZEOF(T))

/** Allocate memory for count elements of type T (OS allocator fallback). */
#define flecs_alloc_n(a, T, count) flecs_alloc(a, ECS_SIZEOF(T) * (count))

/** Allocate zeroed memory with debug type name info (OS allocator fallback). */
#define flecs_calloc_w_dbg_info(a, size, type_name) flecs_calloc(a, size)

/** Allocate zeroed memory for a single element of type T (OS allocator fallback). */
#define flecs_calloc_t(a, T) flecs_calloc(a, ECS_SIZEOF(T))

/** Allocate zeroed memory for count elements of type T (OS allocator fallback). */
#define flecs_calloc_n(a, T, count) flecs_calloc(a, ECS_SIZEOF(T) * (count))

/** Free memory for a single element of type T (OS allocator fallback). */
#define flecs_free_t(a, T, ptr) flecs_free(a, ECS_SIZEOF(T), ptr)

/** Free memory for count elements of type T (OS allocator fallback). */
#define flecs_free_n(a, T, count, ptr) flecs_free(a, ECS_SIZEOF(T) * (count), ptr)

/** Reallocate memory with debug type name info (OS allocator fallback). */
#define flecs_realloc_w_dbg_info(a, size_dst, size_src, ptr, type_name)\
    flecs_realloc(a, size_dst, size_src, ptr)

/** Reallocate memory for count elements of type T (OS allocator fallback). */
#define flecs_realloc_n(a, T, count_dst, count_src, ptr)\
    flecs_realloc(a, ECS_SIZEOF(T) * count_dst, ECS_SIZEOF(T) * count_src, ptr)

/** Duplicate count elements of type T (OS allocator fallback). */
#define flecs_dup_n(a, T, count, ptr) flecs_dup(a, ECS_SIZEOF(T) * (count), ptr)

#endif

#endif

#ifndef FLECS_STRBUF_H_
#define FLECS_STRBUF_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Initializer for ecs_strbuf_t. */
#ifdef __cplusplus
/* Fixes missing field initializer warning on g++ */
#define ECS_STRBUF_INIT (ecs_strbuf_t){}
#else
#define ECS_STRBUF_INIT (ecs_strbuf_t){0}
#endif

/** Size of the small string optimization buffer. */
#define ECS_STRBUF_SMALL_STRING_SIZE (512)

/** Maximum nesting depth for list operations. */
#define ECS_STRBUF_MAX_LIST_DEPTH (32)

/** Element tracking for nested list appends. */
typedef struct ecs_strbuf_list_elem {
    int32_t count;            /**< Number of elements appended to the list. */
    const char *separator;    /**< Separator string inserted between elements. */
} ecs_strbuf_list_elem;

/** A string buffer for efficient string construction. */
typedef struct ecs_strbuf_t {
    char *content;            /**< Pointer to the heap-allocated string content. */
    ecs_size_t length;        /**< Current length of the string in bytes. */
    ecs_size_t size;          /**< Allocated capacity of the content buffer. */

    ecs_strbuf_list_elem list_stack[ECS_STRBUF_MAX_LIST_DEPTH]; /**< Stack of nested list states. */
    int32_t list_sp;          /**< Current list stack pointer (nesting depth). */

    char small_string[ECS_STRBUF_SMALL_STRING_SIZE]; /**< Inline buffer for small string optimization. */
} ecs_strbuf_t;

/** Append a format string to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param fmt The format string.
 */
FLECS_API
void ecs_strbuf_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/** Append a format string with an argument list to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_strbuf_vappend(
    ecs_strbuf_t *buffer,
    const char *fmt,
    va_list args);

/** Append a string to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param str The string to append.
 */
FLECS_API
void ecs_strbuf_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

/** Append a character to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param ch The character to append.
 */
FLECS_API
void ecs_strbuf_appendch(
    ecs_strbuf_t *buffer,
    char ch);

/** Append an int to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param v The integer value to append.
 */
FLECS_API
void ecs_strbuf_appendint(
    ecs_strbuf_t *buffer,
    int64_t v);

/** Append a float to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param v The float value to append.
 * @param nan_delim The delimiter to use for NaN values.
 */
FLECS_API
void ecs_strbuf_appendflt(
    ecs_strbuf_t *buffer,
    double v,
    char nan_delim);

/** Append a boolean to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param v The boolean value to append.
 */
FLECS_API
void ecs_strbuf_appendbool(
    ecs_strbuf_t *buffer,
    bool v);

/** Append a source buffer to a destination buffer.
 *
 * @param dst_buffer The destination buffer.
 * @param src_buffer The source buffer to append.
 */
FLECS_API
void ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer);

/** Append n characters to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param str The string to append from.
 * @param n The number of characters to append.
 */
FLECS_API
void ecs_strbuf_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

/** Return the result string.
 *
 * @param buffer The buffer to get the string from.
 * @return The result string, or NULL if empty.
 */
FLECS_API
char* ecs_strbuf_get(
    ecs_strbuf_t *buffer);

/** Return the small string from the first element (appends \\0).
 *
 * @param buffer The buffer to get the string from.
 * @return The small string.
 */
FLECS_API
char* ecs_strbuf_get_small(
    ecs_strbuf_t *buffer);

/** Reset a buffer without returning a string.
 *
 * @param buffer The buffer to reset.
 */
FLECS_API
void ecs_strbuf_reset(
    ecs_strbuf_t *buffer);

/** Push a list.
 *
 * @param buffer The buffer.
 * @param list_open The string used to open the list.
 * @param separator The separator string inserted between elements.
 */
FLECS_API
void ecs_strbuf_list_push(
    ecs_strbuf_t *buffer,
    const char *list_open,
    const char *separator);

/** Pop a list.
 *
 * @param buffer The buffer.
 * @param list_close The string used to close the list.
 */
FLECS_API
void ecs_strbuf_list_pop(
    ecs_strbuf_t *buffer,
    const char *list_close);

/** Insert a new element in the list.
 *
 * @param buffer The buffer.
 */
FLECS_API
void ecs_strbuf_list_next(
    ecs_strbuf_t *buffer);

/** Append a character as a new element in the list.
 *
 * @param buffer The buffer.
 * @param ch The character to append.
 */
FLECS_API
void ecs_strbuf_list_appendch(
    ecs_strbuf_t *buffer,
    char ch);

/** Append a formatted string as a new element in the list.
 *
 * @param buffer The buffer.
 * @param fmt The format string.
 */
FLECS_API
void ecs_strbuf_list_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/** Append a string as a new element in the list.
 *
 * @param buffer The buffer.
 * @param str The string to append.
 */
FLECS_API
void ecs_strbuf_list_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

/** Append n characters as a new element in the list.
 *
 * @param buffer The buffer.
 * @param str The string to append from.
 * @param n The number of characters to append.
 */
FLECS_API
void ecs_strbuf_list_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

/** Return the number of bytes written to the buffer. */
FLECS_API
int32_t ecs_strbuf_written(
    const ecs_strbuf_t *buffer);

/** Append a string literal to a buffer. Uses sizeof to determine length. */
#define ecs_strbuf_appendlit(buf, str)\
    ecs_strbuf_appendstrn(buf, str, (int32_t)(sizeof(str) - 1))

/** Append a string literal as a new element in a list. Uses sizeof to determine length. */
#define ecs_strbuf_list_appendlit(buf, str)\
    ecs_strbuf_list_appendstrn(buf, str, (int32_t)(sizeof(str) - 1))

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_OS_API_H
#define FLECS_OS_API_H

/**
 * @defgroup c_os_api OS API
 * @ingroup c
 * Interface for providing OS-specific functionality.
 *
 * @{
 */

#if defined(ECS_TARGET_WINDOWS)
#include <malloc.h>
#elif defined(ECS_TARGET_FREEBSD)
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Time type. */
typedef struct ecs_time_t {
    uint32_t sec;                                 /**< Second part. */
    uint32_t nanosec;                             /**< Nanosecond part. */
} ecs_time_t;

/** Allocation counters. */
FLECS_API extern int64_t ecs_os_api_malloc_count;  /**< malloc count. */
FLECS_API extern int64_t ecs_os_api_realloc_count; /**< realloc count. */
FLECS_API extern int64_t ecs_os_api_calloc_count;  /**< calloc count. */
FLECS_API extern int64_t ecs_os_api_free_count;    /**< free count. */

/* Enabling this flag will add a header to each allocation that allows the code
 * to track exactly how much memory has been allocated. Increases memory 
 * utilization by 16 bytes per allocation, and is not thread-safe. */
// #define FLECS_TRACK_OS_ALLOC
#ifdef FLECS_TRACK_OS_ALLOC
FLECS_API extern ecs_size_t ecs_os_allocated_bytes;
#endif

/** Use handle types that _at least_ can store pointers. */
typedef uintptr_t ecs_os_thread_t;                 /**< OS thread. */
typedef uintptr_t ecs_os_cond_t;                   /**< OS cond. */
typedef uintptr_t ecs_os_mutex_t;                  /**< OS mutex. */
typedef uintptr_t ecs_os_dl_t;                     /**< OS dynamic library. */
typedef uintptr_t ecs_os_sock_t;                   /**< OS socket. */

/** 64-bit thread ID. */
typedef uint64_t ecs_os_thread_id_t;

/** Generic function pointer type. */
typedef void (*ecs_os_proc_t)(void);

/** OS API init. */
typedef
void (*ecs_os_api_init_t)(void);

/** OS API deinit. */
typedef
void (*ecs_os_api_fini_t)(void);

/** OS API malloc function type. */
typedef
void* (*ecs_os_api_malloc_t)(
    ecs_size_t size);

/** OS API free function type. */
typedef
void (*ecs_os_api_free_t)(
    void *ptr);

/** OS API realloc function type. */
typedef
void* (*ecs_os_api_realloc_t)(
    void *ptr,
    ecs_size_t size);

/** OS API calloc function type. */
typedef
void* (*ecs_os_api_calloc_t)(
    ecs_size_t size);

/** OS API strdup function type. */
typedef
char* (*ecs_os_api_strdup_t)(
    const char *str);

/** OS API thread_callback function type. */
typedef
void* (*ecs_os_thread_callback_t)(
    void*);

/** OS API thread_new function type. */
typedef
ecs_os_thread_t (*ecs_os_api_thread_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

/** OS API thread_join function type. */
typedef
void* (*ecs_os_api_thread_join_t)(
    ecs_os_thread_t thread);

/** OS API thread_self function type. */
typedef
ecs_os_thread_id_t (*ecs_os_api_thread_self_t)(void);

/** OS API task_new function type. */
typedef
ecs_os_thread_t (*ecs_os_api_task_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

/** OS API task_join function type. */
typedef
void* (*ecs_os_api_task_join_t)(
    ecs_os_thread_t thread);

/** Atomic increment and decrement. */
/** OS API ainc function type. */
typedef
int32_t (*ecs_os_api_ainc_t)(
    int32_t *value);

/** OS API lainc function type. */
typedef
int64_t (*ecs_os_api_lainc_t)(
    int64_t *value);

/** Mutex. */
/** OS API mutex_new function type. */
typedef
ecs_os_mutex_t (*ecs_os_api_mutex_new_t)(
    void);

/** OS API mutex_lock function type. */
typedef
void (*ecs_os_api_mutex_lock_t)(
    ecs_os_mutex_t mutex);

/** OS API mutex_unlock function type. */
typedef
void (*ecs_os_api_mutex_unlock_t)(
    ecs_os_mutex_t mutex);

/** OS API mutex_free function type. */
typedef
void (*ecs_os_api_mutex_free_t)(
    ecs_os_mutex_t mutex);

/** Condition variable. */
/** OS API cond_new function type. */
typedef
ecs_os_cond_t (*ecs_os_api_cond_new_t)(
    void);

/** OS API cond_free function type. */
typedef
void (*ecs_os_api_cond_free_t)(
    ecs_os_cond_t cond);

/** OS API cond_signal function type. */
typedef
void (*ecs_os_api_cond_signal_t)(
    ecs_os_cond_t cond);

/** OS API cond_broadcast function type. */
typedef
void (*ecs_os_api_cond_broadcast_t)(
    ecs_os_cond_t cond);

/** OS API cond_wait function type. */
typedef
void (*ecs_os_api_cond_wait_t)(
    ecs_os_cond_t cond,
    ecs_os_mutex_t mutex);

/** OS API sleep function type. */
typedef
void (*ecs_os_api_sleep_t)(
    int32_t sec,
    int32_t nanosec);

/** OS API enable_high_timer_resolution function type. */
typedef
void (*ecs_os_api_enable_high_timer_resolution_t)(
    bool enable);

/** OS API get_time function type. */
typedef
void (*ecs_os_api_get_time_t)(
    ecs_time_t *time_out);

/** OS API now function type. */
typedef
uint64_t (*ecs_os_api_now_t)(void);

/** OS API log function type.
 *
 * @param level The logging level.
 * @param file The file where the message was logged.
 * @param line The line where it was logged.
 * @param msg The log message.
 */
typedef
void (*ecs_os_api_log_t)(
    int32_t level,
    const char *file,
    int32_t line,
    const char *msg);

/** OS API abort function type. */
typedef
void (*ecs_os_api_abort_t)(
    void);

/** OS API dlopen function type. */
typedef
ecs_os_dl_t (*ecs_os_api_dlopen_t)(
    const char *libname);

/** OS API dlproc function type. */
typedef
ecs_os_proc_t (*ecs_os_api_dlproc_t)(
    ecs_os_dl_t lib,
    const char *procname);

/** OS API dlclose function type. */
typedef
void (*ecs_os_api_dlclose_t)(
    ecs_os_dl_t lib);

/** OS API module_to_path function type. */
typedef
char* (*ecs_os_api_module_to_path_t)(
    const char *module_id);

/** OS API fopen function type. */
typedef
FILE* (*ecs_os_api_fopen_t)(
    const char *file,
    const char *mode);

/** OS API fclose function type. */
typedef
void (*ecs_os_api_fclose_t)(
    FILE *file);

/** OS API performance tracing function type.
 *
 * @param filename The source file name.
 * @param line The source line number.
 * @param name The name of the trace region.
 */
typedef void (*ecs_os_api_perf_trace_t)(
    const char *filename,
    size_t line,
    const char *name);

/* Prefix members of the struct with 'ecs_' as some system headers may define
 * macros for functions like "strdup", "log", or "_free". */

/** OS API interface. */
typedef struct ecs_os_api_t {
    /* API init and deinit */
    ecs_os_api_init_t init_;                       /**< init callback. */
    ecs_os_api_fini_t fini_;                       /**< fini callback. */

    /* Memory management */
    ecs_os_api_malloc_t malloc_;                   /**< malloc callback. */
    ecs_os_api_realloc_t realloc_;                 /**< realloc callback. */
    ecs_os_api_calloc_t calloc_;                   /**< calloc callback. */
    ecs_os_api_free_t free_;                       /**< free callback. */

    /* Strings */
    ecs_os_api_strdup_t strdup_;                   /**< strdup callback. */

    /* Threads */
    ecs_os_api_thread_new_t thread_new_;           /**< thread_new callback. */
    ecs_os_api_thread_join_t thread_join_;         /**< thread_join callback. */
    ecs_os_api_thread_self_t thread_self_;         /**< thread_self callback. */

    /* Tasks */
    ecs_os_api_thread_new_t task_new_;             /**< task_new callback. */
    ecs_os_api_thread_join_t task_join_;           /**< task_join callback. */

    /* Atomic increment and decrement */
    ecs_os_api_ainc_t ainc_;                       /**< ainc callback. */
    ecs_os_api_ainc_t adec_;                       /**< adec callback. */
    ecs_os_api_lainc_t lainc_;                     /**< lainc callback. */
    ecs_os_api_lainc_t ladec_;                     /**< ladec callback. */

    /* Mutex */
    ecs_os_api_mutex_new_t mutex_new_;             /**< mutex_new callback. */
    ecs_os_api_mutex_free_t mutex_free_;           /**< mutex_free callback. */
    ecs_os_api_mutex_lock_t mutex_lock_;           /**< mutex_lock callback. */
    ecs_os_api_mutex_lock_t mutex_unlock_;         /**< mutex_unlock callback. */

    /* Condition variable */
    ecs_os_api_cond_new_t cond_new_;               /**< cond_new callback. */
    ecs_os_api_cond_free_t cond_free_;             /**< cond_free callback. */
    ecs_os_api_cond_signal_t cond_signal_;         /**< cond_signal callback. */
    ecs_os_api_cond_broadcast_t cond_broadcast_;   /**< cond_broadcast callback. */
    ecs_os_api_cond_wait_t cond_wait_;             /**< cond_wait callback. */

    /* Time */
    ecs_os_api_sleep_t sleep_;                     /**< sleep callback. */
    ecs_os_api_now_t now_;                         /**< now callback. */
    ecs_os_api_get_time_t get_time_;               /**< get_time callback. */

    /* Logging */
    ecs_os_api_log_t log_; /**< log callback.
                            * The level should be interpreted as:
                            * >0: Debug tracing. Only enabled in debug builds.
                            *  0: Tracing. Enabled in debug and release builds.
                            * -2: Warning. An issue occurred, but the operation was successful.
                            * -3: Error. An issue occurred, and the operation was unsuccessful.
                            * -4: Fatal. An issue occurred, and the application must quit. */

    /* Application termination */
    ecs_os_api_abort_t abort_;                     /**< abort callback. */

    /* Dynamic library loading */
    ecs_os_api_dlopen_t dlopen_;                   /**< dlopen callback. */
    ecs_os_api_dlproc_t dlproc_;                   /**< dlproc callback. */
    ecs_os_api_dlclose_t dlclose_;                 /**< dlclose callback. */

    /* Overridable function that translates from a logical module ID to a
     * shared library filename. */
    ecs_os_api_module_to_path_t module_to_dl_;     /**< module_to_dl callback. */

    /* Overridable function that translates from a logical module ID to a
     * path that contains module-specific resources or assets. */
    ecs_os_api_module_to_path_t module_to_etc_;    /**< module_to_etc callback. */

    /* File I/O */
    ecs_os_api_fopen_t fopen_;                     /**< fopen callback. */
    ecs_os_api_fclose_t fclose_;                   /**< fclose callback. */

    /* Performance tracing */
    ecs_os_api_perf_trace_t perf_trace_push_; /**< perf_trace_push callback. */
    ecs_os_api_perf_trace_t perf_trace_pop_;  /**< perf_trace_pop callback. */

    int32_t log_level_;                            /**< Tracing level. */
    int32_t log_indent_;                           /**< Tracing indentation level. */
    int32_t log_last_error_;                       /**< Last logged error code. */
    int64_t log_last_timestamp_;                   /**< Last logged timestamp. */

    ecs_flags32_t flags_;                          /**< OS API flags. */

    void *log_out_;                                /**< File used for logging output (type is FILE*)
                                                    * (hint: log_ decides where to write). */
} ecs_os_api_t;

/** Static OS API variable with configured callbacks. */
FLECS_API
extern ecs_os_api_t ecs_os_api;

/** Initialize the OS API.
 * This operation is not usually called by an application. To override callbacks
 * of the OS API, use the following pattern:
 * 
 * @code
 * ecs_os_set_api_defaults();
 * ecs_os_api_t os_api = ecs_os_get_api();
 * os_api.abort_ = my_abort;
 * ecs_os_set_api(&os_api);
 * @endcode
 */
FLECS_API
void ecs_os_init(void);

/** Deinitialize the OS API.
 * This operation is not usually called by an application.
 */
FLECS_API
void ecs_os_fini(void);

/** Override the OS API.
 * This overrides the OS API struct with new values for callbacks. See 
 * ecs_os_init() for how to use the function.
 * 
 * @param os_api Pointer to a struct with values to set.
 */
FLECS_API
void ecs_os_set_api(
    ecs_os_api_t *os_api);

/** Get the OS API.
 * 
 * @return A value with the current OS API callbacks.
 * @see ecs_os_init()
 */
FLECS_API
ecs_os_api_t ecs_os_get_api(void);

/** Set default values for the OS API.
 * This initializes the OS API struct with default values for callbacks like
 * malloc and free.
 * 
 * @see ecs_os_init()
 */
FLECS_API
void ecs_os_set_api_defaults(void);

/** Macro utilities 
 * \cond
 */

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

#define ecs_os_realloc_t(ptr, T) ECS_CAST(T*, ecs_os_realloc(ptr, ECS_SIZEOF(T)))
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
#define ecs_os_memcpy_n(ptr1, ptr2, T, count) ecs_os_memcpy(ptr1, ptr2, ECS_SIZEOF(T) * (size_t)count)
#define ecs_os_memcmp_t(ptr1, ptr2, T) ecs_os_memcmp(ptr1, ptr2, ECS_SIZEOF(T))

#define ecs_os_memmove_t(ptr1, ptr2, T) ecs_os_memmove(ptr1, ptr2, ECS_SIZEOF(T))
#define ecs_os_memmove_n(ptr1, ptr2, T, count) ecs_os_memmove(ptr1, ptr2, ECS_SIZEOF(T) * (size_t)count)

#define ecs_os_strcmp(str1, str2) strcmp(str1, str2)
#define ecs_os_memset_t(ptr, value, T) ecs_os_memset(ptr, value, ECS_SIZEOF(T))
#define ecs_os_memset_n(ptr, value, T, count) ecs_os_memset(ptr, value, ECS_SIZEOF(T) * (size_t)count)
#define ecs_os_zeromem(ptr) ecs_os_memset(ptr, 0, ECS_SIZEOF(*ptr))

#define ecs_os_memdup_t(ptr, T) ecs_os_memdup(ptr, ECS_SIZEOF(T))
#define ecs_os_memdup_n(ptr, T, count) ecs_os_memdup(ptr, ECS_SIZEOF(T) * count)

#define ecs_offset(ptr, T, index)\
    ECS_CAST(T*, ECS_OFFSET(ptr, ECS_SIZEOF(T) * index))

#if !defined(ECS_TARGET_POSIX) && !defined(ECS_TARGET_MINGW)
#define ecs_os_strcat(str1, str2) strcat_s(str1, INT_MAX, str2)
#define ecs_os_snprintf(ptr, len, ...) sprintf_s(ptr, ECS_CAST(size_t, len), __VA_ARGS__)
#define ecs_os_vsnprintf(ptr, len, fmt, args) vsnprintf(ptr, ECS_CAST(size_t, len), fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy_s(str1, INT_MAX, str2)
#define ecs_os_strncpy(str1, str2, len) strncpy_s(str1, INT_MAX, str2, ECS_CAST(size_t, len))
#else
#define ecs_os_strcat(str1, str2) strcat(str1, str2)
#define ecs_os_snprintf(ptr, len, ...) snprintf(ptr, ECS_CAST(size_t, len), __VA_ARGS__)
#define ecs_os_vsnprintf(ptr, len, fmt, args) vsnprintf(ptr, ECS_CAST(size_t, len), fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy(str1, str2)
#define ecs_os_strncpy(str1, str2, len) strncpy(str1, str2, ECS_CAST(size_t, len))
#endif

/* Files */
#define ecs_os_fopen(file, mode) ecs_os_api.fopen_(file, mode)
#define ecs_os_fclose(file) ecs_os_api.fclose_(file)

/* Threads */
#define ecs_os_thread_new(callback, param) ecs_os_api.thread_new_(callback, param)
#define ecs_os_thread_join(thread) ecs_os_api.thread_join_(thread)
#define ecs_os_thread_self() ecs_os_api.thread_self_()

/* Tasks */
#define ecs_os_task_new(callback, param) ecs_os_api.task_new_(callback, param)
#define ecs_os_task_join(thread) ecs_os_api.task_join_(thread)

/* Atomic increment and decrement */
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

#ifndef FLECS_DISABLE_COUNTERS
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
#else
#define ecs_os_inc(v)
#define ecs_os_linc(v)
#define ecs_os_dec(v)
#define ecs_os_ldec(v)
#endif

#ifdef ECS_TARGET_MINGW
/* mingw bug: without this, a conversion error is thrown, but isnan/isinf should
 * accept float, double, and long double. */
#define ecs_os_isnan(val) (isnan((float)val))
#define ecs_os_isinf(val) (isinf((float)val))
#else
#define ecs_os_isnan(val) (isnan(val))
#define ecs_os_isinf(val) (isinf(val))
#endif

/* Application termination */
#define ecs_os_abort() ecs_os_api.abort_()

/* Dynamic libraries */
#define ecs_os_dlopen(libname) ecs_os_api.dlopen_(libname)
#define ecs_os_dlproc(lib, procname) ecs_os_api.dlproc_(lib, procname)
#define ecs_os_dlclose(lib) ecs_os_api.dlclose_(lib)

/* Module ID translation */
#define ecs_os_module_to_dl(lib) ecs_os_api.module_to_dl_(lib)
#define ecs_os_module_to_etc(lib) ecs_os_api.module_to_etc_(lib)

/** Macro utilities 
 * \endcond
 */

/* Logging */

/** Log at debug level.
 * 
 * @param file The source file.
 * @param line The source line.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_dbg(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at trace level.
 * 
 * @param file The source file.
 * @param line The source line.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_trace(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at warning level.
 * 
 * @param file The source file.
 * @param line The source line.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_warn(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at error level.
 * 
 * @param file The source file.
 * @param line The source line.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_err(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at fatal level.
 * 
 * @param file The source file.
 * @param line The source line.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_fatal(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Convert errno to a string.
 * 
 * @param err The error number.
 * @return A string describing the error.
 */
FLECS_API
const char* ecs_os_strerror(
    int err);

/** A utility for assigning strings.
 * This operation frees an existing string and duplicates the input string.
 * 
 * @param str Pointer to a string value.
 * @param value The string value to assign.
 */
FLECS_API
void ecs_os_strset(
    char **str, 
    const char *value);

/* Profile tracing */
#define ecs_os_perf_trace_push(name)
#define ecs_os_perf_trace_pop(name)

/** Push a performance trace region.
 *
 * @param file The source file name.
 * @param line The source line number.
 * @param name The name of the trace region.
 */
void ecs_os_perf_trace_push_(
    const char *file,
    size_t line,
    const char *name);

/** Pop a performance trace region.
 *
 * @param file The source file name.
 * @param line The source line number.
 * @param name The name of the trace region.
 */
void ecs_os_perf_trace_pop_(
    const char *file,
    size_t line,
    const char *name);

/** Sleep with floating-point time. 
 * 
 * @param t The time in seconds.
 */
FLECS_API
void ecs_sleepf(
    double t);

/** Measure time since the provided timestamp. 
 * Use with a time value initialized to 0 to obtain the number of seconds since
 * the epoch. The operation will write the current timestamp into start.
 * 
 * Usage:
 * @code
 * ecs_time_t t = {};
 * ecs_time_measure(&t);
 * // code
 * double elapsed = ecs_time_measure(&t);
 * @endcode
 * 
 * @param start The starting timestamp.
 * @return The time elapsed since start.
 */
FLECS_API
double ecs_time_measure(
    ecs_time_t *start);

/** Calculate the difference between two timestamps. 
 * 
 * @param t1 The first timestamp.
 * @param t2 The second timestamp.
 * @return The difference between timestamps.
 */
FLECS_API
ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2);

/** Convert a time value to a double. 
 * 
 * @param t The timestamp.
 * @return The timestamp converted to a double.
 */
FLECS_API
double ecs_time_to_double(
    ecs_time_t t);

/** Return newly allocated memory that contains a copy of src. 
 * 
 * @param src The source pointer.
 * @param size The number of bytes to copy.
 * @return The duplicated memory.
 */
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
 * Public API types.
 *
 * @{
 */

/**
 * @defgroup function_types Function types.
 * Function callback types.
 *
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

/** Function prototype for iterating an iterator.
 * Stored inside initialized iterators. This allows an application to iterate
 * an iterator without needing to know what created it.
 *
 * @param it The iterator to iterate.
 * @return True if iterator has more results, false if not.
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

/** Callback used for comparing components. */
typedef int (*ecs_order_by_action_t)(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2);

/** Callback used for sorting the entire table of components. */
typedef void (*ecs_sort_table_action_t)(
    ecs_world_t* world,
    ecs_table_t* table,
    ecs_entity_t* entities,
    void* ptr,
    int32_t size,
    int32_t lo,
    int32_t hi,
    ecs_order_by_action_t order_by);

/** Callback used for grouping tables in a query. */
typedef uint64_t (*ecs_group_by_action_t)(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t group_id,
    void *ctx);

/** Callback invoked when a query creates a new group. */
typedef void* (*ecs_group_create_action_t)(
    ecs_world_t *world,
    uint64_t group_id,
    void *group_by_ctx); /* from ecs_query_desc_t */

/** Callback invoked when a query deletes an existing group. */
typedef void (*ecs_group_delete_action_t)(
    ecs_world_t *world,
    uint64_t group_id,
    void *group_ctx,     /* return value from ecs_group_create_action_t */
    void *group_by_ctx); /* from ecs_query_desc_t */

/** Initialization action for modules. */
typedef void (*ecs_module_action_t)(
    ecs_world_t *world);

/** Action callback on world exit. */
typedef void (*ecs_fini_action_t)(
    ecs_world_t *world,
    void *ctx);

/** Function to clean up context data. */
typedef void (*ecs_ctx_free_t)(
    void *ctx);

/** Callback used for sorting values. */
typedef int (*ecs_compare_action_t)(
    const void *ptr1,
    const void *ptr2);

/** Callback used for hashing values. */
typedef uint64_t (*ecs_hash_value_action_t)(
    const void *ptr);

/** Constructor/destructor callback. */
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

/** Compare hook to compare component instances. */
typedef int (*ecs_cmp_t)(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info);

/** Equals operator hook. */
typedef bool (*ecs_equals_t)(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info);

/** Destructor function for poly objects. */
typedef void (*flecs_poly_dtor_t)(
    ecs_poly_t *poly);

/** @} */

/**
 * @defgroup query_types Query descriptor types.
 * Types used to describe queries.
 *
 * @{
 */

/** Specify read/write access for term. */
typedef enum ecs_inout_kind_t {
    EcsInOutDefault,  /**< InOut for regular terms, In for shared terms. */
    EcsInOutNone,     /**< Term is neither read nor written. */
    EcsInOutFilter,   /**< Same as InOutNone + prevents term from triggering observers. */
    EcsInOut,         /**< Term is both read and written. */
    EcsIn,            /**< Term is only read. */
    EcsOut,           /**< Term is only written. */
} ecs_inout_kind_t;

/** Specify operator for term. */
typedef enum ecs_oper_kind_t {
    EcsAnd,           /**< The term must match. */
    EcsOr,            /**< One of the terms in an or chain must match. */
    EcsNot,           /**< The term must not match. */
    EcsOptional,      /**< The term may match. */
    EcsAndFrom,       /**< Term must match all components from term ID. */
    EcsOrFrom,        /**< Term must match at least one component from term ID. */
    EcsNotFrom,       /**< Term must match none of the components from term ID. */
} ecs_oper_kind_t;

/** Specify cache policy for query. */
typedef enum ecs_query_cache_kind_t {
    EcsQueryCacheDefault,   /**< Behavior determined by query creation context. */
    EcsQueryCacheAuto,      /**< Cache query terms that are cacheable. */
    EcsQueryCacheAll,       /**< Require that all query terms can be cached. */
    EcsQueryCacheNone,      /**< No caching. */
} ecs_query_cache_kind_t;

/** Term ID flags. */

/** Match on self.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsSelf                       (1llu << 63)

/** Match by traversing upwards.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsUp                         (1llu << 62)

/** Traverse relationship transitively.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsTrav                       (1llu << 61)

/** Sort results breadth-first.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsCascade                    (1llu << 60)

/** Iterate groups in descending order.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsDesc                       (1llu << 59)

/** Term ID is a variable.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsIsVariable                 (1llu << 58)

/** Term ID is an entity.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsIsEntity                   (1llu << 57)

/** Term ID is a name (don't attempt to look up as an entity).
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsIsName                     (1llu << 56)

/** All term traversal flags.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsTraverseFlags              (EcsSelf|EcsUp|EcsTrav|EcsCascade|EcsDesc)

/** All term reference kind flags.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsTermRefFlags               (EcsTraverseFlags|EcsIsVariable|EcsIsEntity|EcsIsName)

/** Type that describes a reference to an entity or variable in a term. */
typedef struct ecs_term_ref_t {
    ecs_entity_t id;            /**< Entity ID. If left to 0 and flags do not
                                 * specify whether the ID is an entity or a variable,
                                 * the ID will be initialized to #EcsThis.
                                 * To explicitly set the ID to 0, leave the ID
                                 * member to 0 and set #EcsIsEntity in flags. */

    const char *name;           /**< Name. This can be either the variable name
                                 * (when the #EcsIsVariable flag is set) or an
                                 * entity name. When ecs_term_t::move is true,
                                 * the API assumes ownership over the string and
                                 * will free it when the term is destroyed. */
} ecs_term_ref_t;

/** Type that describes a term (single element in a query). */
struct ecs_term_t {
    ecs_id_t id;                /**< Component ID to be matched by term. Can be
                                 * set directly, or will be populated from the
                                 * first/second members, which provide more
                                 * flexibility. */

    ecs_term_ref_t src;          /**< Source of term. */
    ecs_term_ref_t first;        /**< Component or first element of pair. */
    ecs_term_ref_t second;       /**< Second element of pair. */

    ecs_entity_t trav;          /**< Relationship to traverse when looking for the
                                 * component. The relationship must have
                                 * the `Traversable` property. Default is `IsA`. */

    int16_t inout;              /**< Access to contents matched by term. */
    int16_t oper;               /**< Operator of term. */

    int8_t field_index;         /**< Index of the field for the term in the iterator. */
    ecs_flags16_t flags_;       /**< Flags that help evaluation, set by ecs_query_init(). */
};

/** Queries are lists of constraints (terms) that match entities. 
 * Created with ecs_query_init().
 */
struct ecs_query_t {
    ecs_header_t hdr;           /**< Object header. */

    ecs_term_t *terms;          /**< Query terms. */
    int32_t *sizes;             /**< Component sizes. Indexed by field. */
    ecs_id_t *ids;              /**< Component ids. Indexed by field. */

    uint64_t bloom_filter;      /**< Bitmask used to quickly discard tables. */
    ecs_flags32_t flags;        /**< Query flags. */
    int8_t var_count;           /**< Number of query variables. */
    int8_t term_count;          /**< Number of query terms. */
    int8_t field_count;         /**< Number of fields returned by the query. */

    /** Bitmasks for quick field information lookups. */
    ecs_termset_t fixed_fields; /**< Fields with a fixed source. */
    ecs_termset_t var_fields;   /**< Fields with non-$this variable source. */
    ecs_termset_t static_id_fields; /**< Fields with a static (component) id. */
    ecs_termset_t data_fields;  /**< Fields that have data. */
    ecs_termset_t write_fields; /**< Fields that write data. */
    ecs_termset_t read_fields;  /**< Fields that read data. */
    ecs_termset_t row_fields;   /**< Fields that must be acquired with field_at. */
    ecs_termset_t shared_readonly_fields; /**< Fields that don't write shared data. */
    ecs_termset_t set_fields;   /**< Fields that will be set. */

    ecs_query_cache_kind_t cache_kind;  /**< Caching policy of the query. */

    char **vars;                /**< Array with variable names for the iterator. */

    void *ctx;                  /**< User context to pass to callback. */
    void *binding_ctx;          /**< Context to be used for language bindings. */

    ecs_entity_t entity;        /**< Entity associated with query (optional). */
    ecs_world_t *real_world;    /**< Actual world. */
    ecs_world_t *world;         /**< World or stage the query was created with. */

    int32_t eval_count;         /**< Number of times the query is evaluated. */
};

/** An observer reacts to events matching a query.
 * Created with ecs_observer_init().
 */
struct ecs_observer_t {
    ecs_header_t hdr;           /**< Object header. */

    ecs_query_t *query;         /**< Observer query. */

    /** Observer events. */
    ecs_entity_t events[FLECS_EVENT_DESC_MAX];
    int32_t event_count;        /**< Number of events. */

    ecs_iter_action_t callback; /**< See ecs_observer_desc_t::callback. */
    ecs_run_action_t run;       /**< See ecs_observer_desc_t::run. */

    void *ctx;                  /**< Observer context. */
    void *callback_ctx;         /**< Callback language binding context. */
    void *run_ctx;              /**< Run language binding context. */

    ecs_ctx_free_t ctx_free;    /**< Callback to free ctx. */
    ecs_ctx_free_t callback_ctx_free; /**< Callback to free callback_ctx. */
    ecs_ctx_free_t run_ctx_free; /**< Callback to free run_ctx. */

    ecs_observable_t *observable; /**< Observable for the observer. */

    ecs_world_t *world;         /**< The world. */
    ecs_entity_t entity;        /**< Entity associated with the observer. */
};

/** @} */

/** Type that contains component lifecycle callbacks.
 *
 * @ingroup components
 */

/* Flags that can be used to check which hooks a type has set */
#define ECS_TYPE_HOOK_CTOR                   ECS_CAST(ecs_flags32_t, 1 << 0)
#define ECS_TYPE_HOOK_DTOR                   ECS_CAST(ecs_flags32_t, 1 << 1)
#define ECS_TYPE_HOOK_COPY                   ECS_CAST(ecs_flags32_t, 1 << 2)
#define ECS_TYPE_HOOK_MOVE                   ECS_CAST(ecs_flags32_t, 1 << 3)
#define ECS_TYPE_HOOK_COPY_CTOR              ECS_CAST(ecs_flags32_t, 1 << 4)
#define ECS_TYPE_HOOK_MOVE_CTOR              ECS_CAST(ecs_flags32_t, 1 << 5)
#define ECS_TYPE_HOOK_CTOR_MOVE_DTOR         ECS_CAST(ecs_flags32_t, 1 << 6)
#define ECS_TYPE_HOOK_MOVE_DTOR              ECS_CAST(ecs_flags32_t, 1 << 7)
#define ECS_TYPE_HOOK_CMP                    ECS_CAST(ecs_flags32_t, 1 << 8)
#define ECS_TYPE_HOOK_EQUALS                 ECS_CAST(ecs_flags32_t, 1 << 9)

/* Flags that can be used to set/check which hooks of a type are invalid */
#define ECS_TYPE_HOOK_CTOR_ILLEGAL           ECS_CAST(ecs_flags32_t, 1 << 10)
#define ECS_TYPE_HOOK_DTOR_ILLEGAL           ECS_CAST(ecs_flags32_t, 1 << 12)
#define ECS_TYPE_HOOK_COPY_ILLEGAL           ECS_CAST(ecs_flags32_t, 1 << 13)
#define ECS_TYPE_HOOK_MOVE_ILLEGAL           ECS_CAST(ecs_flags32_t, 1 << 14)
#define ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL      ECS_CAST(ecs_flags32_t, 1 << 15)
#define ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL      ECS_CAST(ecs_flags32_t, 1 << 16)
#define ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL ECS_CAST(ecs_flags32_t, 1 << 17)
#define ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL      ECS_CAST(ecs_flags32_t, 1 << 18)
#define ECS_TYPE_HOOK_CMP_ILLEGAL            ECS_CAST(ecs_flags32_t, 1 << 19)
#define ECS_TYPE_HOOK_EQUALS_ILLEGAL         ECS_CAST(ecs_flags32_t, 1 << 20)

/* Internal debug flag that indicates type hooks have been invoked */
#define ECS_TYPE_HOOK_IN_USE                 ECS_CAST(ecs_flags32_t, 1 << 21)

/* All valid hook flags */
#define ECS_TYPE_HOOKS (ECS_TYPE_HOOK_CTOR|ECS_TYPE_HOOK_DTOR|\
    ECS_TYPE_HOOK_COPY|ECS_TYPE_HOOK_MOVE|ECS_TYPE_HOOK_COPY_CTOR|\
    ECS_TYPE_HOOK_MOVE_CTOR|ECS_TYPE_HOOK_CTOR_MOVE_DTOR|\
    ECS_TYPE_HOOK_MOVE_DTOR|ECS_TYPE_HOOK_CMP|ECS_TYPE_HOOK_EQUALS)

/* All invalid hook flags */
#define ECS_TYPE_HOOKS_ILLEGAL (ECS_TYPE_HOOK_CTOR_ILLEGAL|\
    ECS_TYPE_HOOK_DTOR_ILLEGAL|ECS_TYPE_HOOK_COPY_ILLEGAL|\
    ECS_TYPE_HOOK_MOVE_ILLEGAL|ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL|\
    ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL|ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL|\
    ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL|ECS_TYPE_HOOK_CMP_ILLEGAL|\
    ECS_TYPE_HOOK_EQUALS_ILLEGAL)
struct ecs_type_hooks_t {
    ecs_xtor_t ctor;            /**< ctor. */
    ecs_xtor_t dtor;            /**< dtor. */
    ecs_copy_t copy;            /**< copy assignment. */
    ecs_move_t move;            /**< move assignment. */

    /** Ctor + copy. */
    ecs_copy_t copy_ctor;

    /** Ctor + move. */
    ecs_move_t move_ctor;

    /** Ctor + move + dtor (or move_ctor + dtor).
     * This combination is typically used when a component is moved from one
     * location to a new location, like when it is moved to a new table. If
     * not set explicitly, it will be derived from other callbacks. */
    ecs_move_t ctor_move_dtor;

    /** Move + dtor.
     * This combination is typically used when a component is moved from one
     * location to an existing location, like what happens during a remove. If
     * not set explicitly, it will be derived from other callbacks. */
    ecs_move_t move_dtor;

    /** Compare hook. */
    ecs_cmp_t cmp;

    /** Equals hook. */
    ecs_equals_t equals;

    /** Hook flags.
     * Indicates which hooks are set for the type, and which hooks are illegal.
     * When an ILLEGAL flag is set when calling ecs_set_hooks(), a hook callback
     * will be set that panics when called. */
    ecs_flags32_t flags;
    
    /** Callback that is invoked when an instance of a component is added. This
     * callback is invoked before observers are invoked. */
    ecs_iter_action_t on_add;

    /** Callback that is invoked when an instance of the component is set. This
     * callback is invoked before observers are invoked, and enables the component
     * to respond to changes on itself before others can. */
    ecs_iter_action_t on_set;

    /** Callback that is invoked when an instance of the component is removed.
     * This callback is invoked after the observers are invoked, and before the
     * destructor is invoked. */
    ecs_iter_action_t on_remove;

    /** Callback that is invoked with the existing and new value before the
     * value is assigned. Invoked after on_add and before on_set. Registering
     * an on_replace hook prevents using operations that return a mutable 
     * pointer to the component, like get_mut(), ensure(), and emplace(). */
    ecs_iter_action_t on_replace;

    void *ctx;                         /**< User-defined context. */
    void *binding_ctx;                 /**< Language binding context. */
    void *lifecycle_ctx;               /**< Component lifecycle context (see meta addon). */

    ecs_ctx_free_t ctx_free;           /**< Callback to free ctx. */
    ecs_ctx_free_t binding_ctx_free;   /**< Callback to free binding_ctx. */
    ecs_ctx_free_t lifecycle_ctx_free; /**< Callback to free lifecycle_ctx. */
};

/** Type that contains component information (passed to ctors/dtors/...).
 *
 * @ingroup components
 */
struct ecs_type_info_t {
    ecs_size_t size;         /**< Size of the type. */
    ecs_size_t alignment;    /**< Alignment of the type. */
    ecs_type_hooks_t hooks;  /**< Type hooks. */
    ecs_entity_t component;  /**< Handle to component (do not set). */
    const char *name;        /**< Type name. */
};

#ifndef FLECS_API_TYPES_H
#define FLECS_API_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//// Opaque types
////////////////////////////////////////////////////////////////////////////////

/** Table data. */
typedef struct ecs_data_t ecs_data_t;

/* Cached query table data. */
typedef struct ecs_query_cache_match_t ecs_query_cache_match_t;

/* Cached query group. */
typedef struct ecs_query_cache_group_t ecs_query_cache_group_t;

////////////////////////////////////////////////////////////////////////////////
//// Non-opaque types
////////////////////////////////////////////////////////////////////////////////

/** All observers for a specific event. */
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
    ecs_event_record_t on_wildcard;
    ecs_sparse_t events;  /* sparse<event, ecs_event_record_t> */
    ecs_vec_t global_observers; /* vector<ecs_observable_t> */
    uint64_t last_observer_id;
};

/** Range in a table. */
typedef struct ecs_table_range_t {
    ecs_table_t *table;
    int32_t offset;       /* Leave both members at 0 to cover the entire table. */
    int32_t count;       
} ecs_table_range_t;

/** Value of a query variable. */
typedef struct ecs_var_t {
    ecs_table_range_t range; /* Set when variable stores a range of entities. */
    ecs_entity_t entity;     /* Set when variable stores a single entity. */

    /* Most entities can be stored as a range by setting range.count to 1, 
     * however in order to also be able to store empty entities in variables, 
     * a separate entity member is needed. Both range and entity may be set at
     * the same time, as long as they are consistent. */
} ecs_var_t;

/** Cached reference. */
struct ecs_ref_t {
    ecs_entity_t entity;         /* Entity. */
    uint64_t table_id;           /* Table ID for detecting ABA issues. */
    uint32_t table_version_fast; /* Fast change detection with false positives. */
    uint16_t table_version;      /* Change detection. */
    void *ptr;                   /* Cached component pointer. */
#ifdef FLECS_DEBUG
    ecs_entity_t id;             /* Component ID (debug only, used for asserts). */
#endif
};

/* Page-iterator-specific data. */
typedef struct ecs_page_iter_t {
    int32_t offset;
    int32_t limit;
    int32_t remaining;
} ecs_page_iter_t;

/* Worker-iterator-specific data. */
typedef struct ecs_worker_iter_t {
    int32_t index;
    int32_t count;
} ecs_worker_iter_t;

/* Convenience struct to iterate a table array for an ID. */
typedef struct ecs_table_cache_iter_t {
    const struct ecs_table_cache_hdr_t *cur, *next;
    bool iter_fill;
    bool iter_empty;
} ecs_table_cache_iter_t;

/** Each iterator. */
typedef struct ecs_each_iter_t {
    ecs_table_cache_iter_t it;

    /* Storage for iterator fields. */
    ecs_id_t ids;
    ecs_entity_t sources;
    ecs_size_t sizes;
    int16_t columns;
    const ecs_table_record_t* trs;
} ecs_each_iter_t;

typedef struct ecs_query_op_profile_t {
    int32_t count[2]; /* 0 = enter, 1 = redo */
} ecs_query_op_profile_t;

/** Query iterator. */
typedef struct ecs_query_iter_t {
    struct ecs_var_t *vars;                   /* Variable storage. */
    const struct ecs_query_var_t *query_vars; /* Query variable metadata. */
    const struct ecs_query_op_t *ops;         /* Query plan operations. */
    struct ecs_query_op_ctx_t *op_ctx;        /* Operation-specific state. */
    uint64_t *written;

    /* Cached iteration. */
    ecs_query_cache_group_t *group;           /* Currently iterated group. */
    ecs_vec_t *tables;                        /* Currently iterated table vector (vec<ecs_query_cache_match_t>). */
    ecs_vec_t *all_tables;                    /* Different from .tables if iterating wildcard matches (vec<ecs_query_cache_match_t>). */
    ecs_query_cache_match_t *elem;            /* Current cache entry. */
    int32_t cur, all_cur;                     /* Indices into tables and all_tables. */

    ecs_query_op_profile_t *profile;

    int16_t op;                               /* Currently iterated query plan operation (index into ops). */
    bool iter_single_group;
} ecs_query_iter_t;

/* Private iterator data. Used by iterator implementations to keep track of
 * progress and to provide built-in storage. */
typedef struct ecs_iter_private_t {
    union {
        ecs_query_iter_t query;
        ecs_page_iter_t page;
        ecs_worker_iter_t worker;
        ecs_each_iter_t each;
    } iter;                       /* Iterator-specific data. */

    void *entity_iter;            /* Query applied after matching a table. */
    ecs_stack_cursor_t *stack_cursor; /* Stack cursor to restore to. */
} ecs_iter_private_t;

/* Data structures that store the command queue. */
typedef struct ecs_commands_t {
    ecs_vec_t queue;
    ecs_stack_t stack;          /* Temp memory used by deferred commands. */
    ecs_sparse_t entries;       /* <entity, op_entry_t> - command batching. */
} ecs_commands_t;

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/** This is the largest possible component ID. Components, for the most part,
 * occupy the same ID range as entities, however they are not allowed to overlap
 * with (8) bits reserved for ID flags. */
#define ECS_MAX_COMPONENT_ID (~((uint32_t)(ECS_ID_FLAGS_MASK >> 32)))

/** The maximum number of nested function calls before the core will throw a
 * cycle-detected error. */
#define ECS_MAX_RECURSION (512)

/** Maximum length of a parser token (used by parser-related addons). */
#define ECS_MAX_TOKEN_SIZE (256)

/** Convert a C module name into a path.
 * This operation converts a PascalCase name to a path, for example, MyFooModule
 * into my.foo.module.
 * 
 * @param c_name The C module name.
 * @return The path.
*/
FLECS_API
char* flecs_module_path_from_c(
    const char *c_name);

/** Constructor that zero-initializes a component value.
 * 
 * @param ptr Pointer to the value.
 * @param count Number of elements to construct.
 * @param type_info Type info for the component.
 */
FLECS_API
void flecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *type_info);

/* Wrapper functions for invoking type hooks with fallback behavior. */
FLECS_API
bool flecs_type_info_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
bool flecs_type_info_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
void flecs_type_info_copy(
    void *dst,
    const void *src,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
void flecs_type_info_move(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
void flecs_type_info_copy_ctor(
    void *dst,
    const void *src,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
void flecs_type_info_move_ctor(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
void flecs_type_info_ctor_move_dtor(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
void flecs_type_info_move_dtor(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *type_info);

FLECS_API
int flecs_type_info_cmp(
    const void *a,
    const void *b,
    const ecs_type_info_t *type_info);

FLECS_API
bool flecs_type_info_equals(
    const void *a,
    const void *b,
    const ecs_type_info_t *type_info);

/** Create an allocated string from a format.
 * 
 * @param fmt The format string.
 * @param args Format arguments.
 * @return The formatted string.
 */
FLECS_DBG_API
char* flecs_vasprintf(
    const char *fmt,
    va_list args);

/** Create an allocated string from a format.
 * 
 * @param fmt The format string.
 * @return The formatted string.
 */
FLECS_API
char* flecs_asprintf(
    const char *fmt,
    ...);

/** Write an escaped character.
 * Write a character to an output string, inserting an escape character if necessary.
 *
 * @param out The string to write the character to.
 * @param in The input character.
 * @param delimiter The delimiter used (for example, '"').
 * @return Pointer to the character after the last one written.
 */
FLECS_API
char* flecs_chresc(
    char *out,
    char in,
    char delimiter);

/** Parse an escaped character.
 * Parse a character with a potential escape sequence.
 *
 * @param in Pointer to a character in the input string.
 * @param out Output string.
 * @return Pointer to the character after the last one read.
 */
const char* flecs_chrparse(
    const char *in,
    char *out);

/** Write an escaped string.
 * Write an input string to an output string, escaping characters where necessary.
 * To determine the size of the output string, call the operation with a NULL
 * argument for 'out', and use the returned size to allocate a string that is
 * large enough.
 *
 * @param out Pointer to output string (may be NULL).
 * @param size Maximum number of characters written to output.
 * @param delimiter The delimiter used (for example, '"').
 * @param in The input string.
 * @return The number of characters that (would) have been written.
 */
FLECS_API
ecs_size_t flecs_stresc(
    char *out,
    ecs_size_t size,
    char delimiter,
    const char *in);

/** Return an escaped string.
 * Same as flecs_stresc(), but returns an
 * allocated string of the right size.
 *
 * @param delimiter The delimiter used (for example, '"').
 * @param in The input string.
 * @return The escaped string.
 */
FLECS_API
char* flecs_astresc(
    char delimiter,
    const char *in);

/** Skip whitespace and newline characters.
 * This function skips whitespace characters.
 *
 * @param ptr Pointer to (potential) whitespace to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* flecs_parse_ws_eol(
    const char *ptr);

/** Parse a digit.
 * This function will parse until the first non-digit character is found. The
 * provided expression must contain at least one digit character.
 *
 * @param ptr The expression to parse.
 * @param token The output buffer.
 * @param token_size The size of the output buffer.
 * @return Pointer to the first non-digit character.
 */
FLECS_API
const char* flecs_parse_digit(
    const char *ptr,
    char *token,
    int32_t token_size);

/* Convert an identifier to snake case. */
FLECS_API
char* flecs_to_snake_case(
    const char *str);

/* Suspend and resume read-only state. To fully support implicit registration of
 * components, it should be possible to register components while the world is
 * in read-only mode. It is not uncommon that a component is used first from
 * within a system, which is often run while in read-only mode.
 *
 * Suspending read-only mode is only allowed when the world is not multithreaded.
 * When a world is multithreaded, it is not safe to (even temporarily) leave
 * read-only mode, so a multithreaded application should always explicitly
 * register components in advance.
 *
 * These operations also suspend deferred mode.
 *
 * Functions are public to support language bindings.
 */
typedef struct ecs_suspend_readonly_state_t {
    bool is_readonly;
    bool is_deferred;
    bool cmd_flushing;
    int32_t defer_count;
    ecs_entity_t scope;
    ecs_entity_t with;
    ecs_commands_t cmd_stack[2];
    ecs_commands_t *cmd;
    ecs_stage_t *stage;
} ecs_suspend_readonly_state_t;

FLECS_API
ecs_world_t* flecs_suspend_readonly(
    const ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

FLECS_API
void flecs_resume_readonly(
    ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

/** Return the number of observed entities in a table.
 * This operation is public to support test cases.
 *
 * @param table The table.
 * @return The number of observed entities.
 */
FLECS_DBG_API
int32_t flecs_table_observed_count(
    const ecs_table_t *table);

/** Print a backtrace to the specified stream.
 * 
 * @param stream The stream to use for printing the backtrace.
 */
FLECS_DBG_API
void flecs_dump_backtrace(
    void *stream);

/** Increase the refcount of a poly object.
 * 
 * @param poly The poly object.
 * @return The refcount after incrementing.
 */
FLECS_API
int32_t flecs_poly_claim_(
    ecs_poly_t *poly);

/** Decrease the refcount of a poly object.
 * 
 * @param poly The poly object.
 * @return The refcount after decrementing.
 */
FLECS_API
int32_t flecs_poly_release_(
    ecs_poly_t *poly);

#define flecs_poly_claim(poly) \
    flecs_poly_claim_(ECS_CONST_CAST(void*, reinterpret_cast<const void*>(poly)))

#define flecs_poly_release(poly) \
    flecs_poly_release_(ECS_CONST_CAST(void*, reinterpret_cast<const void*>(poly)))

/** Return the refcount of a poly object.
 * 
 * @param poly The poly object.
 * @return Refcount of the poly object.
 */
FLECS_API
int32_t flecs_poly_refcount(
    ecs_poly_t *poly);

/** Get an unused index for the static world-local component ID array.
 * This operation returns an unused index for the world-local component ID
 * array. This index can be used by language bindings to obtain a component ID.
 * 
 * @return Unused index for component ID array.
 */
FLECS_API
int32_t flecs_component_ids_index_get(void);

/** Get a world-local component ID.
 *
 * @param world The world.
 * @param index Component ID array index.
 * @return The component ID.
 */
FLECS_API
ecs_entity_t flecs_component_ids_get(
    const ecs_world_t *world, 
    int32_t index);

/** Get an alive world-local component ID.
 * Same as flecs_component_ids_get(), but returns 0 if the component is no
 * longer alive.
 *
 * @param world The world.
 * @param index Component ID array index.
 * @return The component ID.
 */
FLECS_API
ecs_entity_t flecs_component_ids_get_alive(
    const ecs_world_t *world, 
    int32_t index);

/** Set a world-local component ID.
 *
 * @param world The world.
 * @param index Component ID array index.
 * @param id The component ID.
 */
FLECS_API
void flecs_component_ids_set(
    ecs_world_t *world, 
    int32_t index,
    ecs_entity_t id);

/** Query iterator function for trivially cached queries.
 * This operation can be called if an iterator matches the conditions for
 * trivial iteration.
 * 
 * @param it The query iterator.
 * @return Whether the query has more results.
 */
FLECS_API
bool flecs_query_trivial_cached_next(
    ecs_iter_t *it);

#define flecs_check_exclusive_world_access_write(world)
#define flecs_check_exclusive_world_access_read(world)

/** End deferred mode (executes commands when stage->defer becomes 0). */
FLECS_API
bool flecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage);

/** Calculate an offset from an address. */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif
#define ECS_OFFSET_T(o, T) ECS_OFFSET(o, ECS_SIZEOF(T))

#define ECS_ELEM(ptr, size, index) ECS_OFFSET(ptr, (size) * (index))
#define ECS_ELEM_T(o, T, index) ECS_ELEM(o, ECS_SIZEOF(T), index)

/** Enable and disable bitsets. */
#define ECS_BIT_SET(flags, bit) (flags) |= (bit)
#define ECS_BIT_CLEAR(flags, bit) (flags) &= ~(bit) 
#define ECS_BIT_COND(flags, bit, cond) ((cond) \
    ? (ECS_BIT_SET(flags, bit)) \
    : (ECS_BIT_CLEAR(flags, bit)))

#define ECS_BIT_CLEAR16(flags, bit) (flags) &= (ecs_flags16_t)~(bit)   
#define ECS_BIT_COND16(flags, bit, cond) ((cond) \
    ? (ECS_BIT_SET(flags, bit)) \
    : (ECS_BIT_CLEAR16(flags, bit)))

#define ECS_BIT_IS_SET(flags, bit) ((flags) & (bit))

#define ECS_BIT_SETN(flags, n) ECS_BIT_SET(flags, 1llu << n)
#define ECS_BIT_CLEARN(flags, n) ECS_BIT_CLEAR(flags, 1llu << n)
#define ECS_BIT_CONDN(flags, n, cond) ECS_BIT_COND(flags, 1llu << n, cond)

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/** A bucket in the hashmap, storing parallel key and value vectors. */
typedef struct {
    ecs_vec_t keys; /**< Vector of keys. */
    ecs_vec_t values; /**< Vector of values. */
} ecs_hm_bucket_t;

/** A hashmap that supports variable-sized keys and values. */
typedef struct {
    ecs_hash_value_action_t hash; /**< Hash function for keys. */
    ecs_compare_action_t compare; /**< Compare function for keys. */
    ecs_size_t key_size; /**< Size of key type. */
    ecs_size_t value_size; /**< Size of value type. */
    ecs_map_t impl; /**< Underlying map implementation. */
} ecs_hashmap_t;

/** Iterator for a hashmap. */
typedef struct {
    ecs_map_iter_t it; /**< Underlying map iterator. */
    ecs_hm_bucket_t *bucket; /**< Current bucket. */
    int32_t index; /**< Current index within the bucket. */
} flecs_hashmap_iter_t;

/** Result of a hashmap ensure operation. */
typedef struct {
    void *key; /**< Pointer to the key. */
    void *value; /**< Pointer to the value. */
    uint64_t hash; /**< Hash value of the key. */
} flecs_hashmap_result_t;

/** Initialize a hashmap.
 *
 * @param hm The hashmap to initialize.
 * @param key_size The size of the key type.
 * @param value_size The size of the value type.
 * @param hash The hash function.
 * @param compare The compare function.
 * @param allocator The allocator.
 */
FLECS_DBG_API
void flecs_hashmap_init_(
    ecs_hashmap_t *hm,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare,
    ecs_allocator_t *allocator);

/** Type-safe hashmap initialization. */
#define flecs_hashmap_init(hm, K, V, hash, compare, allocator)\
    flecs_hashmap_init_(hm, ECS_SIZEOF(K), ECS_SIZEOF(V), hash, compare, allocator)

/** Deinitialize a hashmap.
 *
 * @param map The hashmap to deinitialize.
 */
FLECS_DBG_API
void flecs_hashmap_fini(
    ecs_hashmap_t *map);

/** Get a value from the hashmap.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to look up.
 * @param value_size The size of the value type.
 * @return Pointer to the value, or NULL if not found.
 */
FLECS_DBG_API
void* flecs_hashmap_get_(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

/** Type-safe hashmap get. */
#define flecs_hashmap_get(map, key, V)\
    (V*)flecs_hashmap_get_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

/** Ensure a key exists in the hashmap, inserting if necessary.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to ensure.
 * @param value_size The size of the value type.
 * @return A result containing pointers to the key, value, and hash.
 */
FLECS_DBG_API
flecs_hashmap_result_t flecs_hashmap_ensure_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

/** Type-safe hashmap ensure. */
#define flecs_hashmap_ensure(map, key, V)\
    flecs_hashmap_ensure_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

/** Set a key-value pair in the hashmap.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key.
 * @param value_size The size of the value type.
 * @param value The value to set.
 */
FLECS_DBG_API
void flecs_hashmap_set_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

/** Type-safe hashmap set. */
#define flecs_hashmap_set(map, key, value)\
    flecs_hashmap_set_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

/** Remove a key from the hashmap.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to remove.
 * @param value_size The size of the value type.
 */
FLECS_DBG_API
void flecs_hashmap_remove_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

/** Type-safe hashmap remove. */
#define flecs_hashmap_remove(map, key, V)\
    flecs_hashmap_remove_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

/** Remove a key from the hashmap using a precomputed hash.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to remove.
 * @param value_size The size of the value type.
 * @param hash The precomputed hash of the key.
 */
FLECS_DBG_API
void flecs_hashmap_remove_w_hash_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

/** Type-safe hashmap remove with precomputed hash. */
#define flecs_hashmap_remove_w_hash(map, key, V, hash)\
    flecs_hashmap_remove_w_hash_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

/** Get a bucket from the hashmap by hash value.
 *
 * @param map The hashmap.
 * @param hash The hash value.
 * @return The bucket, or NULL if not found.
 */
FLECS_DBG_API
ecs_hm_bucket_t* flecs_hashmap_get_bucket(
    const ecs_hashmap_t *map,
    uint64_t hash);

/** Remove an entry from a hashmap bucket by index.
 *
 * @param map The hashmap.
 * @param bucket The bucket.
 * @param hash The hash value.
 * @param index The index within the bucket to remove.
 */
FLECS_DBG_API
void flecs_hm_bucket_remove(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    uint64_t hash,
    int32_t index);

/** Copy a hashmap.
 *
 * @param dst The destination hashmap.
 * @param src The source hashmap.
 */
FLECS_DBG_API
void flecs_hashmap_copy(
    ecs_hashmap_t *dst,
    const ecs_hashmap_t *src);

/** Create an iterator for a hashmap.
 *
 * @param map The hashmap to iterate.
 * @return The iterator.
 */
FLECS_DBG_API
flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map);

/** Get the next element from a hashmap iterator.
 *
 * @param it The hashmap iterator.
 * @param key_size The size of the key type.
 * @param key_out Output parameter for the key.
 * @param value_size The size of the value type.
 * @return Pointer to the value, or NULL if no more elements.
 */
FLECS_DBG_API
void* flecs_hashmap_next_(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

/** Type-safe hashmap next (value only). */
#define flecs_hashmap_next(map, V)\
    (V*)flecs_hashmap_next_(map, 0, NULL, ECS_SIZEOF(V))

/** Type-safe hashmap next with key output. */
#define flecs_hashmap_next_w_key(map, K, key, V)\
    (V*)flecs_hashmap_next_(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_API_INTERNALS_H
#define FLECS_API_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/** Record for entity index. */
struct ecs_record_t {
    ecs_table_t *table;                        /**< Identifies a type (and table) in the world. */
    uint32_t row;                              /**< Table row of the entity. */
    int32_t dense;                             /**< Index in dense array of entity index. */
};

/** Header for table cache elements. */
typedef struct ecs_table_cache_hdr_t {
    struct ecs_component_record_t *cr;         /**< Component record for component. */
    ecs_table_t *table;                        /**< Table associated with element. */
    struct ecs_table_cache_hdr_t *prev, *next; /**< Previous and next elements for ID in table cache. */
} ecs_table_cache_hdr_t;

/** Record that stores the location of a component in a table.
 * Table records are registered with component records, which allows for quickly
 * finding all tables for a specific component. */
struct ecs_table_record_t {
    ecs_table_cache_hdr_t hdr;                 /**< Table cache header. */
    int16_t index;                             /**< First type index where ID occurs in table. */
    int16_t count;                             /**< Number of times ID occurs in table. */
    int16_t column;                            /**< First column index where ID occurs. */
};

/** Type that contains information about which components got added or removed on
 * a table edge. */
typedef struct ecs_table_diff_t {
    ecs_type_t added;                /* Components added between tables. */
    ecs_type_t removed;              /* Components removed between tables. */
    ecs_flags32_t added_flags;
    ecs_flags32_t removed_flags;
} ecs_table_diff_t;

/* Tracks which and how many non-fragmenting children are stored in a table for a parent. */
typedef struct ecs_parent_record_t {
    uint32_t entity;                /* If the table only contains a single entity for the parent, this will contain the entity ID (without generation). */
    int32_t count;                  /* The number of children for a parent in the table. */
} ecs_parent_record_t;

/** Find the record for an entity.
 * An entity record contains the table and row for the entity.
 * 
 * To use ecs_record_t::row as the record in the table, use:
 *   ECS_RECORD_TO_ROW(r->row)
 * 
 * This removes potential entity bitflags from the row field.
 * 
 * @param world The world.
 * @param entity The entity.
 * @return The record, NULL if the entity does not exist.
 */
FLECS_API
ecs_record_t* ecs_record_find(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the entity corresponding to a record.
 * This operation only works for entities that are not empty.
 *
 * @param record The record for which to obtain the entity ID.
 * @return The entity ID for the record.
 */
FLECS_API
ecs_entity_t ecs_record_get_entity(
    const ecs_record_t *record);

/** Begin exclusive write access to an entity.
 * This operation provides safe exclusive access to the components of an entity
 * without the overhead of deferring operations.
 *
 * When this operation is called simultaneously for the same entity more than
 * once, it will throw an assert. Note that for this to happen, asserts must be
 * enabled. It is up to the application to ensure that access is exclusive, for
 * example, by using a read-write mutex.
 *
 * Exclusive access is enforced at the table level, so only one entity can be
 * exclusively accessed per table. The exclusive access check is thread-safe.
 *
 * This operation must be followed up with ecs_write_end().
 *
 * @param world The world.
 * @param entity The entity.
 * @return A record to the entity.
 */
FLECS_API
ecs_record_t* ecs_write_begin(
    ecs_world_t *world,
    ecs_entity_t entity);

/** End exclusive write access to an entity.
 * This operation ends exclusive access, and must be called after
 * ecs_write_begin().
 *
 * @param record Record to the entity.
 */
FLECS_API
void ecs_write_end(
    ecs_record_t *record);

/** Begin read access to an entity.
 * This operation provides safe read access to the components of an entity.
 * Multiple simultaneous reads are allowed per entity.
 *
 * This operation ensures that code attempting to mutate the entity's table will
 * throw an assert. Note that for this to happen, asserts must be enabled. It is
 * up to the application to ensure that this does not happen, for example, by
 * using a read-write mutex.
 *
 * This operation does *not* provide the same guarantees as a read-write mutex,
 * as it is possible to call ecs_read_begin() after calling ecs_write_begin(). It is
 * up to the application to ensure that this does not happen.
 *
 * This operation must be followed up with ecs_read_end().
 *
 * @param world The world.
 * @param entity The entity.
 * @return A record to the entity.
 */
FLECS_API
const ecs_record_t* ecs_read_begin(
    ecs_world_t *world,
    ecs_entity_t entity);

/** End read access to an entity.
 * This operation ends read access, and must be called after ecs_read_begin().
 *
 * @param record Record to the entity.
 */
FLECS_API
void ecs_read_end(
    const ecs_record_t *record);

/** Get a component from an entity record.
 * This operation returns a pointer to a component for the entity
 * associated with the provided record. For safe access to the component, obtain
 * the record with ecs_read_begin() or ecs_write_begin().
 *
 * Obtaining a component from a record is faster than obtaining it from the
 * entity handle, as it reduces the number of lookups required.
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) ID.
 * @return Pointer to component, or NULL if entity does not have the component.
 *
 * @see ecs_record_ensure_id()
 */
FLECS_API
const void* ecs_record_get_id(
    const ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

/** Same as ecs_record_get_id(), but returns a mutable pointer.
 * For safe access to the component, obtain the record with ecs_write_begin().
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) ID.
 * @return Pointer to component, or NULL if entity does not have the component.
 */
FLECS_API
void* ecs_record_ensure_id(
    ecs_world_t *world,
    ecs_record_t *record,
    ecs_id_t id);

/** Test if the entity for a record has a (component) ID.
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) ID.
 * @return Whether the entity has the component.
 */
FLECS_API
bool ecs_record_has_id(
    ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

/** Get a component pointer from a column and record.
 * This returns a pointer to the component using a table column index. The
 * table's column index can be found with ecs_table_get_column_index().
 * 
 * Usage:
 * @code
 * ecs_record_t *r = ecs_record_find(world, entity);
 * int32_t column = ecs_table_get_column_index(world, table, ecs_id(Position));
 * Position *ptr = ecs_record_get_by_column(r, column, sizeof(Position));
 * @endcode
 * 
 * @param record The record.
 * @param column The column index in the entity's table.
 * @param size The component size.
 * @return The component pointer.
 */
FLECS_API
void* ecs_record_get_by_column(
    const ecs_record_t *record,
    int32_t column,
    size_t size);

/** Get the component record for a component ID.
 *
 * @param world The world.
 * @param id The component ID.
 * @return The component record, or NULL if it doesn't exist.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_component_record_t* flecs_components_get(
    const ecs_world_t *world,
    ecs_id_t id);

/** Ensure a component record for a component ID.
 *
 * @param world The world.
 * @param id The component ID.
 * @return The new or existing component record.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_component_record_t* flecs_components_ensure(
    ecs_world_t *world,
    ecs_id_t id);

/** Get the component ID from a component record.
 *
 * @param cr The component record.
 * @return The component ID.
 */
FLECS_API
ecs_id_t flecs_component_get_id(
    const ecs_component_record_t *cr);

/** Get the component flags for a component.
 *
 * @param world The world.
 * @param id The component ID.
 * @return The flags for the component ID.
 */
FLECS_API
ecs_flags32_t flecs_component_get_flags(
    const ecs_world_t *world,
    ecs_id_t id);

/** Get the type info for a component record.
 * 
 * @param cr The component record.
 * @return The type info struct, or NULL if the component is a tag.
 */
FLECS_API
const ecs_type_info_t* flecs_component_get_type_info(
    const ecs_component_record_t *cr);

/** Find the table record for a component record.
 * This operation returns the table record for the table and component record if it
 * exists. If the record exists, it means the table has the component.
 * 
 * @param cr The component record.
 * @param table The table.
 * @return The table record if the table has the component, or NULL if not.
 */
FLECS_API
FLECS_ALWAYS_INLINE const ecs_table_record_t* flecs_component_get_table(
    const ecs_component_record_t *cr,
    const ecs_table_t *table);

/** Get the parent record for a component and table.
 * A parent record stores how many children for a parent are stored in the 
 * specified table. If the table only stores a single child, the parent record
 * will also store the entity ID of that child.
 * 
 * This information is used by queries to determine whether an O(n) search 
 * through the table is required to find all children for the parent. If the 
 * table only contains a single child, the query can use
 * ecs_parent_record_t::entity directly, otherwise it has to do a scan.
 * 
 * The component record specified to this function must be a ChildOf pair. Only
 * tables with children that use the non-fragmenting hierarchy storage will have
 * parent records.
 * 
 * @param cr The ChildOf component record.
 * @param table The table to check the number of children for.
 * @return The parent record if it exists, NULL if it does not.
*/
FLECS_API
FLECS_ALWAYS_INLINE ecs_parent_record_t* flecs_component_get_parent_record(
    const ecs_component_record_t *cr,
    const ecs_table_t *table);

/** Return the hierarchy depth for a component record.
 * The specified component record must be a ChildOf pair. This function does not
 * compute the depth, it just returns the precomputed depth that is updated 
 * automatically when hierarchy changes happen.
 * 
 * @param cr The ChildOf component record.
 * @return The depth of the parent's children in the hierarchy.
 */
FLECS_API
FLECS_ALWAYS_INLINE int32_t flecs_component_get_childof_depth(
    const ecs_component_record_t *cr);

/** Create a component record iterator.
 * A component record iterator iterates all tables for the specified component
 * record.
 * 
 * The iterator should be used like this:
 * 
 * @code
 * ecs_table_cache_iter_t it;
 * if (flecs_component_iter(cr, &it)) {
 *   const ecs_table_record_t *tr;
 *   while ((tr = flecs_component_next(&it))) {
 *     ecs_table_t *table = tr->hdr.table;
 *     // ...
 *   }
 * }
 * @endcode
 * 
 * @param cr The component record.
 * @param iter_out Out parameter for the iterator.
 * @return True if there are results, false if there are no results.
 */
FLECS_API
bool flecs_component_iter(
    const ecs_component_record_t *cr,
    ecs_table_cache_iter_t *iter_out);

/** Get the next table record for the iterator.
 * Returns the next table record, or NULL if there are no more results.
 * 
 * @param iter The iterator.
 * @return The next table record, or NULL if there are no more results.
 */
FLECS_API
const ecs_table_record_t* flecs_component_next(
    ecs_table_cache_iter_t *iter);

/** Struct returned by flecs_table_records(). */
typedef struct ecs_table_records_t {
    const ecs_table_record_t *array;
    int32_t count;
} ecs_table_records_t;

/** Get the table records.
 * This operation returns an array with all records for the specified table.
 * 
 * @param table The table.
 * @return The table records for the table.
 */
FLECS_API
ecs_table_records_t flecs_table_records(
    ecs_table_t* table);

/** Get the component record from a table record.
 * 
 * @param tr The table record.
 * @return The component record.
 */
FLECS_API
ecs_component_record_t* flecs_table_record_get_component(
    const ecs_table_record_t *tr);

/** Get the table ID.
 * This operation returns a unique numerical identifier for a table.
 *
 * @param table The table.
 * @return The unique identifier for the table.
 */
FLECS_API
uint64_t flecs_table_id(
    ecs_table_t* table);

/** Find a table by adding an ID to the current table.
 * Same as ecs_table_add_id(), but with an additional diff parameter that contains
 * information about the traversed edge.
 *
 * @param world The world.
 * @param table The table.
 * @param id_ptr Pointer to the component ID to add.
 * @param diff Information about the traversed edge (out parameter).
 * @return The table that was traversed to.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_table_t *flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

#ifdef __cplusplus
}
#endif

#endif

/** Utility to hold a value of a dynamic type. */
typedef struct ecs_value_t {
    ecs_entity_t type;      /**< Type of value. */
    void *ptr;              /**< Pointer to value. */
} ecs_value_t;

/** Used with ecs_entity_init().
 *
 * @ingroup entities
 */
typedef struct ecs_entity_desc_t {
    int32_t _canary;      /**< Used for validity testing. Must be 0. */

    ecs_entity_t id;      /**< Set to modify existing entity (optional). */

    ecs_entity_t parent;  /**< Parent entity. */

    const char *name;     /**< Name of the entity. If no entity is provided, an
                           * entity with this name will be looked up first. When
                           * an entity is provided, the name will be verified
                           * with the existing entity. */

    const char *sep;      /**< Optional custom separator for hierarchical names.
                           * Leave to NULL for the default ('.') separator. Set to
                           * an empty string to prevent tokenization of the name. */

    const char *root_sep; /**< Optional, used for identifiers relative to the root. */

    const char *symbol;   /**< Optional entity symbol. A symbol is an unscoped
                           * identifier that can be used to look up an entity. The
                           * primary use case for this is to associate the entity
                           * with a language identifier, such as a type or
                           * function name, where these identifiers differ from
                           * the name they are registered with in Flecs. For
                           * example, C type "EcsPosition" might be registered
                           * as "flecs.components.transform.Position", with the
                           * symbol set to "EcsPosition". */

    bool use_low_id;      /**< When set to true, a low id (typically reserved for
                           * components) will be used to create the entity, if
                           * no ID is specified. */

    /** 0-terminated array of IDs to add to the entity. */
    const ecs_id_t *add;

    /** 0-terminated array of values to set on the entity. */
    const ecs_value_t *set;

    /** String expression with components to add. */
    const char *add_expr;
} ecs_entity_desc_t;

/** Used with ecs_bulk_init().
 *
 * @ingroup entities
 */
typedef struct ecs_bulk_desc_t {
    int32_t _canary;        /**< Used for validity testing. Must be 0. */

    ecs_entity_t *entities; /**< Entities to bulk insert. Entity IDs provided by
                             * the application must be empty (cannot
                             * have components). If no entity IDs are provided, the
                             * operation will create 'count' new entities. */

    int32_t count;     /**< Number of entities to create/populate. */

    ecs_id_t ids[FLECS_ID_DESC_MAX]; /**< IDs to create the entities with. */

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

/** Used with ecs_component_init().
 *
 * @ingroup components
 */
typedef struct ecs_component_desc_t {
    int32_t _canary;        /**< Used for validity testing. Must be 0. */

    /** Existing entity to associate with a component (optional). */
    ecs_entity_t entity;

    /** Parameters for type (size, hooks, ...). */
    ecs_type_info_t type;
} ecs_component_desc_t;

/** Iterator.
 * Used for iterating queries. The ecs_iter_t type contains all the information
 * that is provided by a query, and contains all the state required for the
 * iterator code.
 * 
 * Functions that create iterators accept as first argument the world, and as
 * second argument the object they iterate. For example:
 * 
 * @code
 * ecs_iter_t it = ecs_query_iter(world, q);
 * @endcode
 * 
 * When this code is called from a system, it is important to use the world
 * provided by its iterator object to ensure thread safety. For example:
 * 
 * @code
 * void Collide(ecs_iter_t *it) {
 *   ecs_iter_t qit = ecs_query_iter(it->world, Colliders);
 * }
 * @endcode
 * 
 * An iterator contains resources that need to be released. By default, this
 * is handled by the last call to next() that returns false. When iteration is
 * ended before iteration has completed, an application has to manually call
 * ecs_iter_fini() to release the iterator resources:
 * 
 * @code
 * ecs_iter_t it = ecs_query_iter(world, q);
 * while (ecs_query_next(&it)) {
 *   if (cond) {
 *     ecs_iter_fini(&it);
 *     break;
 *   }
 * }
 * @endcode
 *
 * @ingroup queries
 */
struct ecs_iter_t {
    /* World */
    ecs_world_t *world;           /**< The world. Can point to a stage when in deferred or readonly mode. */
    ecs_world_t *real_world;      /**< Actual world. Never points to a stage. */

    /* Matched data */
    int32_t offset;               /**< Offset relative to the current table. */
    int32_t count;                /**< Number of entities to iterate. */
    const ecs_entity_t *entities; /**< Entity identifiers. */
    void **ptrs;                  /**< Component pointers. If not set or if it is NULL for a field, use it->trs. */
    const ecs_table_record_t **trs; /**< Info on where to find the field in the table. */
    const int16_t *columns;
    const ecs_size_t *sizes;      /**< Component sizes. */
    ecs_table_t *table;           /**< Current table. */
    ecs_table_t *other_table;     /**< Previous or next table when adding or removing. */
    ecs_id_t *ids;                /**< (Component) IDs. */
    ecs_entity_t *sources;        /**< Entity on which the ID was matched (0 if same as entities). */
    ecs_flags64_t constrained_vars; /**< Bitset that marks constrained variables. */
    ecs_termset_t set_fields;     /**< Fields that are set. */
    ecs_termset_t ref_fields;     /**< Bitset with fields that aren't component arrays. */
    ecs_termset_t row_fields;     /**< Fields that must be obtained with field_at. */
    ecs_termset_t up_fields;      /**< Bitset with fields matched through up traversal. */

    /* Input information */
    ecs_entity_t system;          /**< The system (if applicable). */
    ecs_entity_t event;           /**< The event (if applicable). */
    ecs_id_t event_id;            /**< The (component) ID for the event. */
    int32_t event_cur;            /**< Unique event ID. Used to dedup observer calls. */

    /* Query information */
    int8_t field_count;           /**< Number of fields in the iterator. */
    int8_t term_index;            /**< Index of the term that emitted an event.
                                   * This field will be set to the 'index' field
                                   * of an observer term. */
    const ecs_query_t *query;     /**< Query being evaluated. */

    /* Context */
    void *param;                  /**< Param passed to ecs_run(). */
    void *ctx;                    /**< System context. */
    void *binding_ctx;            /**< System binding context. */
    void *callback_ctx;           /**< Callback language binding context. */
    void *run_ctx;                /**< Run language binding context. */

    /* Time */
    ecs_ftime_t delta_time;       /**< Time elapsed since last frame. */
    ecs_ftime_t delta_system_time;/**< Time elapsed since last system invocation. */

    /* Iterator counters */
    int32_t frame_offset;         /**< Offset relative to the start of iteration. */

    /* Misc */
    ecs_flags32_t flags;          /**< Iterator flags. */
    ecs_entity_t interrupted_by;  /**< When set, system execution is interrupted. */
    ecs_iter_private_t priv_;     /**< Private data. */

    /* Chained iterators */
    ecs_iter_next_action_t next;  /**< Function to progress iterator. */
    ecs_iter_action_t callback;   /**< Callback of system or observer. */
    ecs_iter_fini_action_t fini;  /**< Function to clean up iterator resources. */
    ecs_iter_t *chain_it;         /**< Optional, allows for creating iterator chains. */
};

/** Query must match prefabs.
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * \ingroup queries
 */
#define EcsQueryMatchPrefab           (1u << 1u)

/** Query must match disabled entities.
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * \ingroup queries
 */
#define EcsQueryMatchDisabled         (1u << 2u)

/** Query must match empty tables.
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * \ingroup queries
 */
#define EcsQueryMatchEmptyTables      (1u << 3u)

/** Query may have unresolved entity identifiers.
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * \ingroup queries
 */
#define EcsQueryAllowUnresolvedByName (1u << 6u)

/** Query only returns whole tables (ignores toggle or member fields).
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * \ingroup queries
 */
#define EcsQueryTableOnly             (1u << 7u)

/** Enable change detection for a query.
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * 
 * Adding this flag makes it possible to use ecs_query_changed() and 
 * ecs_iter_changed() with the query. Change detection requires the query to be
 * cached. If cache_kind is left to the default value, this flag will cause it
 * to default to EcsQueryCacheAuto.
 * 
 * \ingroup queries
 */
#define EcsQueryDetectChanges         (1u << 8u)

/** Enable ordering for query groups.
 * When this flag is set, groups will be iterated in ascending order, with lower
 * group ids first and higher group ids afterwards.
 * 
 * This flag is enabled automatically when a query contains cascade terms.
 * 
 * \ingroup queries
 */
#define EcsQueryGroupByOrdered        (1u << 9u)

/** Enable descending ordering for query groups.
 * When this flag is set in combination with EcsQueryGroupByOrdered, groups will 
 * be iterated in descending order, with higher group ids first and lower group 
 * ids afterwards.
 * 
 * This flag is enabled automatically when a query contains cascade|desc terms.
 * 
 * \ingroup queries
 */
#define EcsQueryGroupByDesc           (1u << 10u)

/** Used with ecs_query_init().
 * 
 * \ingroup queries
 */
typedef struct ecs_query_desc_t {
    /** Used for validity testing. Must be 0. */
    int32_t _canary;

    /** Query terms. */
    ecs_term_t terms[FLECS_TERM_COUNT_MAX];

    /** Query DSL expression (optional). */
    const char *expr;

    /** Caching policy of the query. */
    ecs_query_cache_kind_t cache_kind;

    /** Flags for enabling query features. */
    ecs_flags32_t flags;

    /** Callback used for ordering query results. If order_by is 0, the
     * pointer provided to the callback will be NULL. If the callback is not
     * set, results will not be ordered. */
    ecs_order_by_action_t order_by_callback;

    /** Callback used for ordering query results. Same as order_by_callback,
     * but more efficient. */
    ecs_sort_table_action_t order_by_table_callback;

    /** Component to sort on, used together with order_by_callback or
     * order_by_table_callback. */
    ecs_entity_t order_by;

    /** Component ID to be used for grouping. Used together with the
     * group_by_callback. */
    ecs_id_t group_by;

    /** Callback used for grouping results. If the callback is not set, results
     * will not be grouped. When set, this callback will be used to calculate a
     * "rank" for each entity (table) based on its components. This rank is then
     * used to sort entities (tables), so that entities (tables) of the same
     * rank are "grouped" together when iterated. */
    ecs_group_by_action_t group_by_callback;

    /** Callback that is invoked when a new group is created. The return value of
     * the callback is stored as context for a group. */
    ecs_group_create_action_t on_group_create;

    /** Callback that is invoked when an existing group is deleted. The return
     * value of the on_group_create callback is passed as context parameter. */
    ecs_group_delete_action_t on_group_delete;

    /** Context to pass to group_by. */
    void *group_by_ctx;

    /** Function to free group_by_ctx. */
    ecs_ctx_free_t group_by_ctx_free;

    /** User context to pass to callback. */
    void *ctx;

    /** Context to be used for language bindings. */
    void *binding_ctx;

    /** Callback to free ctx. */
    ecs_ctx_free_t ctx_free;

    /** Callback to free binding_ctx. */
    ecs_ctx_free_t binding_ctx_free;

    /** Entity associated with query (optional). */
    ecs_entity_t entity;
} ecs_query_desc_t;

/** Used with ecs_observer_init().
 *
 * @ingroup observers
 */
typedef struct ecs_observer_desc_t {
    /** Used for validity testing. Must be 0. */
    int32_t _canary;

    /** Existing entity to associate with an observer (optional). */
    ecs_entity_t entity;

    /** Query for observer. */
    ecs_query_desc_t query;

    /** Events to observe (OnAdd, OnRemove, OnSet). */
    ecs_entity_t events[FLECS_EVENT_DESC_MAX];

    /** When an observer is created, generate events from existing data. For example,
     * #EcsOnAdd `Position` would match all existing instances of `Position`. */
    bool yield_existing;

    /** Global observers are tied to the lifespan of the world. Creating a 
     * global observer does not create an entity, and therefore 
     * ecs_observer_init() will not return an entity handle. */
    bool global_observer;

    /** Callback to invoke on an event, invoked when the observer matches. */
    ecs_iter_action_t callback;

    /** Callback invoked on an event. When left to NULL, the default runner
     * is used, which matches the event with the observer's query, and calls
     * 'callback' when it matches.
     * A reason to override the run function is to improve performance, if there
     * are more efficient ways to test whether an event matches the observer than
     * the general-purpose query matcher. */
    ecs_run_action_t run;

    /** User context to pass to callback. */
    void *ctx;

    /** Callback to free ctx. */
    ecs_ctx_free_t ctx_free;

    /** Context associated with callback (for language bindings). */
    void *callback_ctx;

    /** Callback to free callback ctx. */
    ecs_ctx_free_t callback_ctx_free;

    /** Context associated with run (for language bindings). */
    void *run_ctx;

    /** Callback to free run ctx. */
    ecs_ctx_free_t run_ctx_free;

    /** Used for internal purposes. Do not set. */
    int32_t *last_event_id;
    int8_t term_index_;    /**< Used for internal purposes. Do not set. */
    ecs_flags32_t flags_;  /**< Used for internal purposes. Do not set. */
} ecs_observer_desc_t;

/** Used with ecs_emit().
 *
 * @ingroup observers
 */
typedef struct ecs_event_desc_t {
    /** The event ID. Only observers for the specified event will be notified. */
    ecs_entity_t event;

    /** Component IDs. Only observers with a matching component ID will be
     * notified. Observers are guaranteed to get notified once, even if they
     * match more than one ID. */
    const ecs_type_t *ids;

    /** The table for which to notify. */
    ecs_table_t *table;

    /** Optional second table to notify. This can be used to communicate the
     * previous or next table, in case an entity is moved between tables. */
    ecs_table_t *other_table;

    /** Limit notified entities to ones starting from offset (row) in table. */
    int32_t offset;

    /** Limit number of notified entities to count. offset+count must be less
     * than the total number of entities in the table. If left to 0, it will be
     * automatically determined by doing `ecs_table_count(table) - offset`. */
    int32_t count;

    /** Single-entity alternative to setting table / offset / count. */
    ecs_entity_t entity;

    /** Optional context.
     * The type of the param must be the event, where the event is a component.
     * When an event is enqueued, the value of param is copied to a temporary
     * storage of the event type. */
    void *param;

    /** Same as param, but with the guarantee that the value won't be modified.
     * When an event with a const parameter is enqueued, the value of the param
     * is copied to a temporary storage of the event type. */
    const void *const_param;

    /** Observable (usually the world). */
    ecs_poly_t *observable;

    /** Event flags. */
    ecs_flags32_t flags;
} ecs_event_desc_t;

/**
 * @defgroup misc_types Miscellaneous types
 * Types used to create entities, observers, queries, and more.
 *
 * @{
 */

/** Type with information about the current Flecs build. */
typedef struct ecs_build_info_t {
    const char *compiler;           /**< Compiler used to compile Flecs. */
    const char **addons;            /**< Addons included in the build. */
    const char **flags;             /**< Compile-time settings. */
    const char *version;            /**< Stringified version. */
    int16_t version_major;          /**< Major Flecs version. */
    int16_t version_minor;          /**< Minor Flecs version. */
    int16_t version_patch;          /**< Patch Flecs version. */
    bool debug;                     /**< Is this a debug build? */
    bool sanitize;                  /**< Is this a sanitize build? */
    bool perf_trace;                /**< Is this a perf tracing build? */
} ecs_build_info_t;

/** Type that contains information about the world. */
typedef struct ecs_world_info_t {
    ecs_entity_t last_component_id;   /**< Last issued component entity ID. */

    ecs_ftime_t delta_time_raw;       /**< Raw delta time (no time scaling). */
    ecs_ftime_t delta_time;           /**< Time passed to or computed by ecs_progress(). */
    ecs_ftime_t time_scale;           /**< Time scale applied to delta_time. */
    ecs_ftime_t target_fps;           /**< Target FPS. */
    ecs_ftime_t frame_time_total;     /**< Total time spent processing a frame. */
    ecs_ftime_t system_time_total;    /**< Total time spent in systems. */
    ecs_ftime_t emit_time_total;      /**< Total time spent notifying observers. */
    ecs_ftime_t merge_time_total;     /**< Total time spent in merges. */
    ecs_ftime_t rematch_time_total;   /**< Time spent on query rematching. */
    double world_time_total;          /**< Time elapsed in simulation. */
    double world_time_total_raw;      /**< Time elapsed in simulation (no scaling). */

    int64_t frame_count_total;        /**< Total number of frames. */
    int64_t merge_count_total;        /**< Total number of merges. */
    int64_t eval_comp_monitors_total; /**< Total number of monitor evaluations. */
    int64_t rematch_count_total;      /**< Total number of rematches. */

    int64_t id_create_total;          /**< Total number of times a new ID was created. */
    int64_t id_delete_total;          /**< Total number of times an ID was deleted. */
    int64_t table_create_total;       /**< Total number of times a table was created. */
    int64_t table_delete_total;       /**< Total number of times a table was deleted. */
    int64_t pipeline_build_count_total; /**< Total number of pipeline builds. */
    int64_t systems_ran_total;        /**< Total number of systems run. */
    int64_t observers_ran_total;      /**< Total number of times an observer was invoked. */
    int64_t queries_ran_total;        /**< Total number of times a query was evaluated. */

    int32_t tag_id_count;             /**< Number of tag (no data) IDs in the world. */
    int32_t component_id_count;       /**< Number of component (data) IDs in the world. */
    int32_t pair_id_count;            /**< Number of pair IDs in the world. */

    int32_t table_count;              /**< Number of tables. */

    uint32_t creation_time;           /**< Time when world was created. */

    /* -- Command counts -- */
    struct {
        int64_t add_count;             /**< Add commands processed. */
        int64_t remove_count;          /**< Remove commands processed. */
        int64_t delete_count;          /**< Delete commands processed. */
        int64_t clear_count;           /**< Clear commands processed. */
        int64_t set_count;             /**< Set commands processed. */
        int64_t ensure_count;          /**< Ensure or emplace commands processed. */
        int64_t modified_count;        /**< Modified commands processed. */
        int64_t discard_count;         /**< Commands discarded, happens when the entity is no longer alive when running the command. */
        int64_t event_count;           /**< Enqueued custom events. */
        int64_t other_count;           /**< Other commands processed. */
        int64_t batched_entity_count;  /**< Entities for which commands were batched. */
        int64_t batched_command_count; /**< Commands batched. */
    } cmd;                             /**< Command statistics. */

    const char *name_prefix;          /**< Value set by ecs_set_name_prefix(). Used
                                       * to remove library prefixes of symbol
                                       * names (such as `Ecs`, `ecs_`) when
                                       * registering them as names. */
} ecs_world_info_t;

/** Type that contains information about a query group. */
typedef struct ecs_query_group_info_t {
    uint64_t id;          /**< Group ID. */
    int32_t match_count;  /**< How often tables have been matched or unmatched. */
    int32_t table_count;  /**< Number of tables in group. */
    void *ctx;            /**< Group context, returned by on_group_create. */
} ecs_query_group_info_t;

/** Type that stores an entity id range.
 * Returned by ecs_entity_range_new(), used with ecs_entity_range_set().
 */
typedef struct ecs_entity_range_t {
    uint32_t min;           /**< First id in range (inclusive). */
    uint32_t max;           /**< Last id in range (inclusive, 0 = unlimited). */
    uint32_t cur;           /**< Last issued id in range. */
    ecs_vec_t recycled;     /**< Recycled entity ids (vec<entity_t>). */
} ecs_entity_range_t;

/** @} */

/**
 * @defgroup builtin_components Built-in component types.
 * Types that represent built-in components.
 *
 * @{
 */

/** A (string) identifier. Used as a pair with #EcsName and #EcsSymbol tags. */
typedef struct EcsIdentifier {
    char *value;          /**< Identifier string. */
    ecs_size_t length;    /**< Length of identifier. */
    uint64_t hash;        /**< Hash of current value. */
    uint64_t index_hash;  /**< Hash of existing record in current index. */
    ecs_hashmap_t *index; /**< Current index. */
} EcsIdentifier;

/** Component information. */
typedef struct EcsComponent {
    ecs_size_t size;           /**< Component size. */
    ecs_size_t alignment;      /**< Component alignment. */
} EcsComponent;

/** Component for storing a poly object. */
typedef struct EcsPoly {
    ecs_poly_t *poly;          /**< Pointer to poly object. */
} EcsPoly;

/** When added to an entity, this informs serialization formats which component
 * to use when a value is assigned to an entity without specifying the 
 * component. This is intended as a hint; serialization formats are not required 
 * to use it. Adding this component does not change the behavior of core ECS 
 * operations. */
typedef struct EcsDefaultChildComponent {
    ecs_id_t component;  /**< Default component ID. */
} EcsDefaultChildComponent;

/** Non-fragmenting ChildOf relationship. */
typedef struct EcsParent {
    ecs_entity_t value;     /**< Parent entity. */
} EcsParent;

/** Component with data to instantiate a non-fragmenting tree. */
typedef struct {
    const char *child_name; /**< Name of the prefab child. */
    ecs_table_t *table;     /**< Table in which the child will be stored. */
    uint32_t child;         /**< Prefab child entity (without generation). */
    int32_t parent_index;   /**< Index into the children vector. */
} ecs_tree_spawner_child_t;

/** Tree spawner data for a single hierarchy depth. */
typedef struct {
    ecs_vec_t children;     /**< vector<ecs_tree_spawner_child_t>. */
} ecs_tree_spawner_t;

/** Tree instantiation cache component.
 * Tree instantiation cache, indexed by depth. Tables will have a
 * (ParentDepth, depth) pair indicating the hierarchy depth. This means that
 * for different depths, the tables the children are created in will also be
 * different. Caching tables for different depths therefore speeds up
 * instantiating trees even when the top-level entity is not at the root.
 */
typedef struct EcsTreeSpawner {
    ecs_tree_spawner_t data[FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE]; /**< Cache data indexed by depth. */
} EcsTreeSpawner;

/** @} */
/** @} */

/* Only include deprecated definitions if deprecated addon is required */
#ifdef FLECS_DEPRECATED

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
 * Public API constants.
 *
 * @{
 */

/**
 * @defgroup id_flags Component ID flags.
 * ID flags are bits that can be set on an ID (ecs_id_t).
 *
 * @{
 */

/** Indicate that the ID is a pair. */
FLECS_API extern const ecs_id_t ECS_PAIR;

/** Automatically override component when it is inherited. */
FLECS_API extern const ecs_id_t ECS_AUTO_OVERRIDE;

/** Add a bitset to storage, which allows a component to be enabled or disabled. */
FLECS_API extern const ecs_id_t ECS_TOGGLE;

/** Indicate that the target of a pair is an integer value. */
FLECS_API extern const ecs_id_t ECS_VALUE_PAIR;

/** @} */

/**
 * @defgroup builtin_tags Built-in component IDs.
 * @{
 */

/* Built-in component IDs */

/** Component component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsComponent);

/** Identifier component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsIdentifier);

/** Poly component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsPoly);

/** Parent component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsParent);

/** Component with data to instantiate a tree. */
FLECS_API extern const ecs_entity_t ecs_id(EcsTreeSpawner);

/** DefaultChildComponent component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsDefaultChildComponent);

/** Relationship storing the entity's depth in a non-fragmenting hierarchy. */
FLECS_API extern const ecs_entity_t EcsParentDepth;

/** Tag added to queries. */
FLECS_API extern const ecs_entity_t EcsQuery;

/** Tag added to observers. */
FLECS_API extern const ecs_entity_t EcsObserver;

/** Tag added to systems. */
FLECS_API extern const ecs_entity_t EcsSystem;

/** TickSource component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsTickSource);

/** Pipeline module component IDs. */
FLECS_API extern const ecs_entity_t ecs_id(EcsPipelineQuery);

/** Timer component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsTimer);

/** RateFilter component ID. */
FLECS_API extern const ecs_entity_t ecs_id(EcsRateFilter);

/** Root scope for built-in Flecs entities. */
FLECS_API extern const ecs_entity_t EcsFlecs;

/** Core module scope. */
FLECS_API extern const ecs_entity_t EcsFlecsCore;

/** Entity associated with world (used for "attaching" components to world). */
FLECS_API extern const ecs_entity_t EcsWorld;

/** Wildcard entity ("*"). Matches any ID, returns all matches. */
FLECS_API extern const ecs_entity_t EcsWildcard;

/** Any entity ("_"). Matches any ID, returns only the first. */
FLECS_API extern const ecs_entity_t EcsAny;

/** This entity. Default source for queries. */
FLECS_API extern const ecs_entity_t EcsThis;

/** Variable entity ("$"). Used in expressions to prefix variable names. */
FLECS_API extern const ecs_entity_t EcsVariable;

/** Mark a relationship as transitive.
 * Behavior:
 *
 * @code
 *   if R(X, Y) and R(Y, Z) then R(X, Z)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsTransitive;

/** Mark a relationship as reflexive.
 * Behavior:
 *
 * @code
 *   R(X, X) == true
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsReflexive;

/** Ensure that an entity or component cannot be used as a target in an `IsA` relationship.
 * Final can improve the performance of queries as they will not attempt to 
 * substitute a final component with its subsets.
 *
 * Behavior:
 *
 * @code
 *   if IsA(X, Y) and Final(Y) throw error
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsFinal;

/** Mark component as inheritable.
 * This is the opposite of Final. This trait can be used to enforce that queries
 * take into account component inheritance before inheritance (IsA) 
 * relationships are added with the component as the target.
 */
FLECS_API extern const ecs_entity_t EcsInheritable;

/** Relationship that specifies component inheritance behavior. */
FLECS_API extern const ecs_entity_t EcsOnInstantiate;

/** Override component on instantiate. 
 * This will copy the component from the base entity `(IsA target)` to the
 * instance. The base component will never be inherited from the prefab. */
FLECS_API extern const ecs_entity_t EcsOverride;

/** Inherit component on instantiate. 
 * This will inherit (share) the component from the base entity `(IsA target)`.
 * The component can be manually overridden by adding it to the instance. */
FLECS_API extern const ecs_entity_t EcsInherit;

/** Never inherit component on instantiate. 
 * This will not copy or share the component from the base entity `(IsA target)`.
 * When the component is added to an instance, its value will never be copied 
 * from the base entity. */
FLECS_API extern const ecs_entity_t EcsDontInherit;

/** Mark relationship as commutative.
 * Behavior:
 *
 * @code
 *   if R(X, Y) then R(Y, X)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsSymmetric;

/** Can be added to a relationship to indicate that the relationship can only occur
 * once on an entity. Adding a second instance will replace the first.
 *
 * Behavior:
 *
 * @code
 *   R(X, Y) + R(X, Z) = R(X, Z)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsExclusive;

/** Mark a relationship as acyclic. Acyclic relationships may not form cycles. */
FLECS_API extern const ecs_entity_t EcsAcyclic;

/** Mark a relationship as traversable. Traversable relationships may be
 * traversed with "up" queries. Traversable relationships are acyclic. */
FLECS_API extern const ecs_entity_t EcsTraversable;

/** Ensure that a component is always added together with another component.
 *
 * Behavior:
 *
 * @code
 *   If With(R, O) and R(X) then O(X)
 *   If With(R, O) and R(X, Y) then O(X, Y)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsWith;

/** Ensure that a relationship target is a child of the specified entity.
 *
 * Behavior:
 *
 * @code
 *   If OneOf(R, O) and R(X, Y), Y must be a child of O
 *   If OneOf(R) and R(X, Y), Y must be a child of R
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsOneOf;

/** Mark a component as toggleable with ecs_enable_id(). */
FLECS_API extern const ecs_entity_t EcsCanToggle;

/** Can be added to components to indicate it is a trait. Traits are components
 * and/or tags that are added to other components to modify their behavior.
 */
FLECS_API extern const ecs_entity_t EcsTrait;

/** Ensure that an entity is always used in a pair as a relationship.
 *
 * Behavior:
 *
 * @code
 *   e.add(R) panics
 *   e.add(X, R) panics, unless X has the "Trait" trait
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsRelationship;

/** Ensure that an entity is always used in a pair as a target.
 *
 * Behavior:
 *
 * @code
 *   e.add(T) panics
 *   e.add(T, X) panics
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsTarget;

/** Can be added to a relationship to indicate that it should never hold data,
 * even when it or the relationship target is a component. */
FLECS_API extern const ecs_entity_t EcsPairIsTag;

/** Tag to indicate name identifier. */
FLECS_API extern const ecs_entity_t EcsName;

/** Tag to indicate symbol identifier. */
FLECS_API extern const ecs_entity_t EcsSymbol;

/** Tag to indicate alias identifier. */
FLECS_API extern const ecs_entity_t EcsAlias;

/** Used to express parent-child relationships. */
FLECS_API extern const ecs_entity_t EcsChildOf;

/** Used to express inheritance relationships. */
FLECS_API extern const ecs_entity_t EcsIsA;

/** Used to express dependency relationships. */
FLECS_API extern const ecs_entity_t EcsDependsOn;

/** Used to express a slot (used with prefab inheritance). */
FLECS_API extern const ecs_entity_t EcsSlotOf;

/** Tag that, when added to a parent, ensures stable order of ecs_children() results. */
FLECS_API extern const ecs_entity_t EcsOrderedChildren;

/** Tag added to module entities. */
FLECS_API extern const ecs_entity_t EcsModule;

/** Tag added to prefab entities. Any entity with this tag is automatically
 * ignored by queries, unless #EcsPrefab is explicitly queried for. */
FLECS_API extern const ecs_entity_t EcsPrefab;

/** When this tag is added to an entity, it is skipped by queries, unless
 * #EcsDisabled is explicitly queried for. */
FLECS_API extern const ecs_entity_t EcsDisabled;

/** Trait added to entities that should never be returned by queries. Reserved
 * for internal entities that have special meaning to the query engine, such as
 * #EcsThis, #EcsWildcard, #EcsAny. */
FLECS_API extern const ecs_entity_t EcsNotQueryable;

/** Event that triggers when an ID is added to an entity. */
FLECS_API extern const ecs_entity_t EcsOnAdd;

/** Event that triggers when an ID is removed from an entity. */
FLECS_API extern const ecs_entity_t EcsOnRemove;

/** Event that triggers when a component is set for an entity. */
FLECS_API extern const ecs_entity_t EcsOnSet;

/** Event that triggers an observer when an entity starts or stops matching a query. */
FLECS_API extern const ecs_entity_t EcsMonitor;

/** Event that triggers when a table is created. */
FLECS_API extern const ecs_entity_t EcsOnTableCreate;

/** Event that triggers when a table is deleted. */
FLECS_API extern const ecs_entity_t EcsOnTableDelete;

/** Relationship used for specifying cleanup behavior. */
FLECS_API extern const ecs_entity_t EcsOnDelete;

/** Relationship used to define what should happen when a target entity (second
 * element of a pair) is deleted. */
FLECS_API extern const ecs_entity_t EcsOnDeleteTarget;

/** Remove cleanup policy. Must be used as a target in a pair with #EcsOnDelete or
 * #EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsRemove;

/** Delete cleanup policy. Must be used as a target in a pair with #EcsOnDelete or
 * #EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsDelete;

/** Panic cleanup policy. Must be used as a target in a pair with #EcsOnDelete or
 * #EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsPanic;

/** Mark component as singleton. Singleton components may only be added to 
 * themselves. */
FLECS_API extern const ecs_entity_t EcsSingleton;

/** Mark component as sparse. */
FLECS_API extern const ecs_entity_t EcsSparse;

/** Mark component as non-fragmenting. */
FLECS_API extern const ecs_entity_t EcsDontFragment;

/** Marker used to indicate `$var == ...` matching in queries. */
FLECS_API extern const ecs_entity_t EcsPredEq;

/** Marker used to indicate `$var == "name"` matching in queries. */
FLECS_API extern const ecs_entity_t EcsPredMatch;

/** Marker used to indicate `$var ~= "pattern"` matching in queries. */
FLECS_API extern const ecs_entity_t EcsPredLookup;

/** Marker used to indicate the start of a scope (`{`) in queries. */
FLECS_API extern const ecs_entity_t EcsScopeOpen;

/** Marker used to indicate the end of a scope (`}`) in queries. */
FLECS_API extern const ecs_entity_t EcsScopeClose;

/** Tag used to indicate a query is empty.
 * This tag is removed automatically when a query becomes non-empty, and is not
 * automatically re-added when it becomes empty.
 */
FLECS_API extern const ecs_entity_t EcsEmpty;

FLECS_API extern const ecs_entity_t ecs_id(EcsPipeline); /**< Pipeline component ID. */
FLECS_API extern const ecs_entity_t EcsOnStart;     /**< OnStart pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPreFrame;    /**< PreFrame pipeline phase. */
FLECS_API extern const ecs_entity_t EcsOnLoad;      /**< OnLoad pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPostLoad;    /**< PostLoad pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPreUpdate;   /**< PreUpdate pipeline phase. */
FLECS_API extern const ecs_entity_t EcsOnUpdate;    /**< OnUpdate pipeline phase. */
FLECS_API extern const ecs_entity_t EcsOnValidate;  /**< OnValidate pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPostUpdate;  /**< PostUpdate pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPreStore;    /**< PreStore pipeline phase. */
FLECS_API extern const ecs_entity_t EcsOnStore;     /**< OnStore pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPostFrame;   /**< PostFrame pipeline phase. */
FLECS_API extern const ecs_entity_t EcsPhase;       /**< Phase pipeline phase. */

FLECS_API extern const ecs_entity_t EcsConstant;    /**< Tag added to enum or bitmask constants. */

/** Value used to quickly check if a component is built-in. This is used to
 * filter out tables with built-in components (for example, for ecs_delete()). */
#define EcsLastInternalComponentId (ecs_id(EcsTreeSpawner))

/** The first user-defined component starts from this ID. IDs up to this number
 * are reserved for built-in components. */
#define EcsFirstUserComponentId (8)

/** The first user-defined entity starts from this ID. IDs up to this number
 * are reserved for built-in entities. */
#define EcsFirstUserEntityId (FLECS_HI_COMPONENT_ID + 128)

/* When visualized, the reserved ID ranges look like this:
 * - [1..8]: Built-in components
 * - [9..FLECS_HI_COMPONENT_ID]: Low IDs reserved for application components
 * - [FLECS_HI_COMPONENT_ID + 1..EcsFirstUserEntityId]: Built-in entities
 */

/** @} */
/** @} */

/**
 * @defgroup world_api World
 * Functions for working with `ecs_world_t`.
 *
 * @{
 */

/**
 * @defgroup world_creation_deletion Creation & Deletion
 * @{
 */

/** Create a new world.
 * This operation automatically imports modules from addons Flecs has been built
 * with, except when the module specifies otherwise.
 *
 * @return A new world.
 */
FLECS_API
ecs_world_t* ecs_init(void);

/** Create a new world with just the core module.
 * Same as ecs_init(), but doesn't import modules from addons. This operation is
 * faster than ecs_init() and results in less memory utilization.
 *
 * @return A new tiny world.
 */
FLECS_API
ecs_world_t* ecs_mini(void);

/** Create a new world with arguments.
 * Same as ecs_init(), but allows passing in command-line arguments. Command-line
 * arguments are used to:
 * - automatically derive the name of the application from argv[0]
 *
 * @param argc The number of arguments.
 * @param argv The argument array.
 * @return A new world.
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

/** Return whether the world is being deleted.
 * This operation can be used in callbacks like type hooks or observers to
 * detect if they are invoked while the world is being deleted.
 *
 * @param world The world.
 * @return True if being deleted, false if not.
 */
FLECS_API
bool ecs_is_fini(
    const ecs_world_t *world);

/** Register an action to be executed when the world is destroyed.
 * Fini actions are typically used when a module needs to clean up before the
 * world shuts down.
 *
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function.
 */
FLECS_API
void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);

/** Type returned by ecs_get_entities(). */
typedef struct ecs_entities_t {
    const ecs_entity_t *ids; /**< Array with all entity IDs in the world. */
    int32_t count;           /**< Total number of entity IDs. */
    int32_t alive_count;     /**< Number of alive entity IDs. */
} ecs_entities_t;

/** Return entity identifiers in the world.
 * This operation returns an array with all entity IDs that exist in the world.
 * Note that the returned array will change and may get invalidated as a result
 * of entity creation and deletion.
 * 
 * To iterate all alive entity IDs, do:
 * @code
 * ecs_entities_t entities = ecs_get_entities(world);
 * for (int i = 0; i < entities.alive_count; i ++) {
 *   ecs_entity_t id = entities.ids[i];
 * }
 * @endcode
 * 
 * To iterate not-alive IDs, do:
 * @code
 * for (int i = entities.alive_count + 1; i < entities.count; i ++) {
 *   ecs_entity_t id = entities.ids[i];
 * }
 * @endcode
 * 
 * The returned array does not need to be freed. Mutating the returned array
 * will result in undefined behavior (and likely crashes).
 * 
 * @param world The world.
 * @return Struct with entity ID array.
 */
FLECS_API
ecs_entities_t ecs_get_entities(
    const ecs_world_t *world);

/** Get flags set on the world.
 * This operation returns the internal flags (see api_flags.h) that are
 * set on the world.
 *
 * @param world The world.
 * @return Flags set on the world.
 */
FLECS_API
ecs_flags32_t ecs_world_get_flags(
    const ecs_world_t *world);

/** @} */

/**
 * @defgroup world_frame Frame functions
 * @{
 */

/** Begin frame.
 * When an application does not use ecs_progress() to control the main loop, it
 * can still use Flecs features such as FPS limiting and time measurements. This
 * operation needs to be invoked whenever a new frame is about to get processed.
 *
 * Calls to ecs_frame_begin() must always be followed by ecs_frame_end().
 *
 * The function accepts a delta_time parameter, which will get passed to
 * systems. This value is also used to compute the amount of time the function
 * needs to sleep to ensure it does not exceed the target_fps, when it is set.
 * When 0 is provided for delta_time, the time will be measured.
 *
 * This function should only be run from the main thread.
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
 * ecs_frame_begin().
 *
 * @param world The world.
 */
FLECS_API
void ecs_frame_end(
    ecs_world_t *world);

/** Register an action to be executed once after the frame.
 * Post frame actions are typically used for calling operations that cannot be
 * invoked during iteration, such as changing the number of threads.
 *
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function.
 */
FLECS_API
void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);

/** Signal exit.
 * This operation signals that the application should quit. It will cause
 * ecs_progress() to return false.
 *
 * @param world The world to quit.
 */
FLECS_API
void ecs_quit(
    ecs_world_t *world);

/** Return whether a quit has been requested.
 *
 * @param world The world.
 * @return Whether a quit has been requested.
 * @see ecs_quit()
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

/** Set target frames per second (FPS) for an application.
 * Setting the target FPS ensures that ecs_progress() is not invoked faster than
 * the specified FPS. When enabled, ecs_progress() tracks the time passed since
 * the last invocation, and sleeps the remaining time of the frame (if any).
 *
 * This feature ensures systems are run at a consistent interval, as well as
 * conserving CPU time by not running systems more often than required.
 *
 * Note that ecs_progress() only sleeps if there is time left in the frame. Both
 * time spent in Flecs and time spent outside of Flecs are taken into
 * account.
 *
 * @param world The world.
 * @param fps The target FPS.
 */
FLECS_API
void ecs_set_target_fps(
    ecs_world_t *world,
    ecs_ftime_t fps);

/** Set the default query flags.
 * Set a default value for the ecs_query_desc_t::flags field. Default flags
 * are applied in addition to the flags provided in the descriptor. For a
 * list of available flags, see include/flecs/private/api_flags.h. Typical flags
 * to use are:
 *
 *  - `EcsQueryMatchEmptyTables`
 *  - `EcsQueryMatchDisabled`
 *  - `EcsQueryMatchPrefab`
 * 
 * @param world The world.
 * @param flags The query flags.
 */
FLECS_API
void ecs_set_default_query_flags(
    ecs_world_t *world,
    ecs_flags32_t flags);

/** @} */

/**
 * @defgroup commands Commands
 * @{
 */

/** Begin readonly mode.
 * This operation puts the world in readonly mode, which disallows mutations on
 * the world. Readonly mode exists so that internal mechanisms can implement
 * optimizations that assume certain aspects of the world do not change, while also
 * providing a mechanism for applications to prevent accidental mutations in, 
 * for example, multithreaded applications.
 * 
 * Readonly mode is a stronger version of deferred mode. In deferred mode,
 * ECS operations such as add, remove, set, delete, etc. are added to a command
 * queue to be executed later. In readonly mode, operations that could break
 * scheduler logic (such as creating systems, queries) are also disallowed.
 * 
 * Readonly mode itself has a single-threaded and a multithreaded mode. In
 * single-threaded mode, certain mutations on the world are still allowed, for
 * example:
 * - Entity liveliness operations (such as ecs_new(), ecs_make_alive()), so that systems are
 *   able to create new entities.
 * - Implicit component registration, so that it works from systems.
 * - Mutations to supporting data structures for the evaluation of uncached
 *   queries, so that these can be created on the fly.
 * 
 * These mutations are safe in single-threaded applications, but for
 * multithreaded applications the world needs to be entirely immutable. For this
 * purpose, multithreaded readonly mode exists, which disallows all mutations on
 * the world. This means that in multithreaded applications, entity liveliness
 * operations, implicit component registration, and on-the-fly query creation
 * are not guaranteed to work.
 * 
 * While in readonly mode, applications can still enqueue ECS operations on a
 * stage. Stages are managed automatically when using the pipeline addon and 
 * ecs_progress(), but they can also be configured manually as shown here:
 * 
 * @code
 * // Number of stages typically corresponds with number of threads
 * ecs_set_stage_count(world, 2);
 * ecs_world_t *stage = ecs_get_stage(world, 1);
 *
 * ecs_readonly_begin(world, false);
 * ecs_add(world, e, Tag); // readonly assert
 * ecs_add(stage, e, Tag); // OK
 * @endcode
 * 
 * When an attempt is made to perform an operation on a world in readonly mode,
 * the code will throw an assert saying that the world is in readonly mode.
 * 
 * A call to ecs_readonly_begin() must be followed up with ecs_readonly_end().
 * When ecs_readonly_end() is called, all enqueued commands from configured 
 * stages are merged back into the world. Calls to ecs_readonly_begin() and
 * ecs_readonly_end() should always happen from a context where the code has
 * exclusive access to the world. The functions themselves are not thread-safe.
 * 
 * In a typical application, a (non-exhaustive) call stack that uses 
 * ecs_readonly_begin() and ecs_readonly_end() will look like this:
 * 
 * @code
 * ecs_progress()
 *   ecs_readonly_begin()
 *     ecs_defer_begin()
 * 
 *       // user code
 * 
 *   ecs_readonly_end()
 *     ecs_defer_end()
 * @endcode
 *
 * @param world The world.
 * @param multi_threaded Whether to enable multithreaded readonly mode.
 * @return Whether world is in readonly mode.
 */
FLECS_API
bool ecs_readonly_begin(
    ecs_world_t *world,
    bool multi_threaded);

/** End readonly mode.
 * This operation ends readonly mode, and must be called after
 * ecs_readonly_begin(). Operations that were deferred while the world was in
 * readonly mode will be flushed.
 *
 * @param world The world.
 */
FLECS_API
void ecs_readonly_end(
    ecs_world_t *world);

/** Merge a stage.
 * This will merge all commands enqueued for a stage.
 *
 * @param stage The stage.
 */
FLECS_API
void ecs_merge(
    ecs_world_t *stage);

/** Defer operations until the end of the frame.
 * When this operation is invoked while iterating, operations between the
 * ecs_defer_begin() and ecs_defer_end() operations are executed at the end
 * of the frame.
 *
 * This operation is thread-safe.
 *
 * @param world The world.
 * @return true if world changed from non-deferred mode to deferred mode.
 *
 * @see ecs_defer_end()
 * @see ecs_is_deferred()
 * @see ecs_defer_resume()
 * @see ecs_defer_suspend()
 * @see ecs_is_defer_suspended()
 */
FLECS_API
bool ecs_defer_begin(
    ecs_world_t *world);

/** End a block of operations to defer.
 * See ecs_defer_begin().
 *
 * This operation is thread-safe.
 *
 * @param world The world.
 * @return true if world changed from deferred mode to non-deferred mode.
 *
 * @see ecs_defer_begin()
 * @see ecs_is_deferred()
 * @see ecs_defer_resume()
 * @see ecs_defer_suspend()
 */
FLECS_API
bool ecs_defer_end(
    ecs_world_t *world);

/** Suspend deferring but do not flush queue.
 * This operation can be used to do an undeferred operation while not flushing
 * the operations in the queue.
 *
 * An application should invoke ecs_defer_resume() before ecs_defer_end() is called.
 * The operation may only be called when deferring is enabled.
 *
 * @param world The world.
 *
 * @see ecs_defer_begin()
 * @see ecs_defer_end()
 * @see ecs_is_deferred()
 * @see ecs_defer_resume()
 */
FLECS_API
void ecs_defer_suspend(
    ecs_world_t *world);

/** Resume deferring.
 * See ecs_defer_suspend().
 *
 * @param world The world.
 *
 * @see ecs_defer_begin()
 * @see ecs_defer_end()
 * @see ecs_is_deferred()
 * @see ecs_defer_suspend()
 */
FLECS_API
void ecs_defer_resume(
    ecs_world_t *world);

/** Test if deferring is enabled for the current stage.
 *
 * @param world The world.
 * @return True if deferred, false if not.
 *
 * @see ecs_defer_begin()
 * @see ecs_defer_end()
 * @see ecs_defer_resume()
 * @see ecs_defer_suspend()
 * @see ecs_is_defer_suspended()
 */
FLECS_API
bool ecs_is_deferred(
    const ecs_world_t *world);

/** Test if deferring is suspended for the current stage.
 *
 * @param world The world.
 * @return True if suspended, false if not.
 *
 * @see ecs_defer_begin()
 * @see ecs_defer_end()
 * @see ecs_is_deferred()
 * @see ecs_defer_resume()
 * @see ecs_defer_suspend()
 */
FLECS_API
bool ecs_is_defer_suspended(
    const ecs_world_t *world);

/** Configure the world to have N stages.
 * This initializes N stages, which allows applications to defer operations to
 * multiple isolated defer queues. This is typically used for applications with
 * multiple threads, where each thread gets its own queue, and commands are
 * merged when threads are synchronized.
 *
 * Note that the ecs_set_threads() function already creates the appropriate
 * number of stages. The ecs_set_stage_count() operation is useful for applications
 * that want to manage their own stages and/or threads.
 *
 * @param world The world.
 * @param stages The number of stages.
 */
FLECS_API
void ecs_set_stage_count(
    ecs_world_t *world,
    int32_t stages);

/** Get the number of configured stages.
 * Return the number of stages set by ecs_set_stage_count().
 *
 * @param world The world.
 * @return The number of stages used for threading.
 */
FLECS_API
int32_t ecs_get_stage_count(
    const ecs_world_t *world);

/** Get stage-specific world pointer.
 * Flecs threads can safely invoke the API as long as they have a private
 * context to write to, also referred to as the stage. This function returns a
 * pointer to a stage, disguised as a world pointer.
 *
 * Note that this function does not create a new world. It simply wraps the
 * existing world in a thread-specific context, which the API knows how to
 * unwrap. The reason the stage is returned as an ecs_world_t is so that it
 * can be passed transparently to the existing API functions, instead of having to
 * create a dedicated API for threading.
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

/** Create an unmanaged stage.
 * Create a stage whose lifecycle is not managed by the world. Must be freed
 * with ecs_stage_free().
 *
 * @param world The world.
 * @return The stage.
 */
FLECS_API
ecs_world_t* ecs_stage_new(
    ecs_world_t *world);

/** Free an unmanaged stage.
 *
 * @param stage The stage to free.
 */
FLECS_API
void ecs_stage_free(
    ecs_world_t *stage);

/** Get the stage ID.
 * The stage ID can be used by an application to learn about which stage it is
 * using, which typically corresponds with the worker thread ID.
 *
 * @param world The world.
 * @return The stage ID.
 */
FLECS_API
int32_t ecs_stage_get_id(
    const ecs_world_t *world);

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
 * @param ctx A pointer to a user-defined structure.
 * @param ctx_free A function that is invoked with ctx when the world is freed.
 */
FLECS_API
void ecs_set_ctx(
    ecs_world_t *world,
    void *ctx,
    ecs_ctx_free_t ctx_free);

/** Set a world binding context.
 * Same as ecs_set_ctx(), but for binding context. A binding context is intended
 * specifically for language bindings to store binding-specific data.
 *
 * @param world The world.
 * @param ctx A pointer to a user-defined structure.
 * @param ctx_free A function that is invoked with ctx when the world is freed.
 */
FLECS_API
void ecs_set_binding_ctx(
    ecs_world_t *world,
    void *ctx,
    ecs_ctx_free_t ctx_free);

/** Get the world context.
 * This operation retrieves a previously set world context.
 *
 * @param world The world.
 * @return The context set with ecs_set_ctx(). If no context was set, the
 *         function returns NULL.
 */
FLECS_API
void* ecs_get_ctx(
    const ecs_world_t *world);

/** Get the world binding context.
 * This operation retrieves a previously set world binding context.
 *
 * @param world The world.
 * @return The context set with ecs_set_binding_ctx(). If no context was set, the
 *         function returns NULL.
 */
FLECS_API
void* ecs_get_binding_ctx(
    const ecs_world_t *world);

/** Get build info.
 * Return information about the current Flecs build.
 * 
 * @return A struct with information about the current Flecs build.
 */
FLECS_API
const ecs_build_info_t* ecs_get_build_info(void);

/** Get the world info.
 *
 * @param world The world.
 * @return A pointer to the world info. Valid for as long as the world exists.
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

/** Free unused memory.
 * This operation frees allocated memory that is no longer in use by the world.
 * Examples of allocations that get cleaned up are:
 * - Unused pages in the entity index
 * - Component columns
 * - Empty tables
 * 
 * Flecs uses allocators internally for speeding up allocations. Allocators are
 * not evaluated by this function, which means that the memory reported by the
 * OS may not go down. For this reason, this function is most effective when
 * combined with FLECS_USE_OS_ALLOC, which disables internal allocators.
 * 
 * @param world The world.
 */
FLECS_API
void ecs_shrink(
    ecs_world_t *world);

/** Create a new entity range.
 * This function creates a range that constrains new entity identifiers returned 
 * by the specified [min, max] interval. Each range maintains its own list of 
 * recycled entity ids, which ensures that recycled ids always respect the 
 * configured range. If `max` is set to 0, the range is unbounded.
 *
 * Entity ranges cannot be deleted once created. Use ecs_entity_range_set() to 
 * activate a range.
 *
 * @param world The world.
 * @param min The first entity id in the range (inclusive).
 * @param max The last entity id in the range (inclusive, 0 = unlimited).
 * @return A pointer to the new range. Does not need to be freed.
 */
FLECS_API
const ecs_entity_range_t* ecs_entity_range_new(
    ecs_world_t *world,
    uint32_t min,
    uint32_t max);

/** Set the active entity range.
 * This function activates a range created with ecs_entity_range_new().
 * When a range is activated, new entity identifiers will fall within the 
 * specified [min, max] interval, including recycled identifiers.
 *
 * When the active range is out of available ids, operations that create new
 * entity ids will assert.
 * 
 * The operation only accepts ranges that have been created by 
 * ecs_entity_range_new().
 *
 * @param world The world.
 * @param range The range to activate.
 */
FLECS_API
void ecs_entity_range_set(
    ecs_world_t *world,
    const ecs_entity_range_t *range);

/** Get the currently active entity id range.
 * Returns the range set by ecs_entity_range_set(), or NULL if no range is
 * active.
 *
 * @param world The world.
 * @return The active range, or NULL.
 */
FLECS_API
const ecs_entity_range_t* ecs_entity_range_get(
    const ecs_world_t *world);

/** Get the largest issued entity ID (not counting generation).
 *
 * @param world The world.
 * @return The largest issued entity ID.
 */
FLECS_API
ecs_entity_t ecs_get_max_id(
    const ecs_world_t *world);

/** Force aperiodic actions.
 * The world may delay certain operations until they are necessary for the
 * application to function correctly. This may cause observable side effects
 * such as delayed triggering of events, which can be inconvenient when, for
 * example, running a test suite.
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

/** Used with ecs_delete_empty_tables(). */
typedef struct ecs_delete_empty_tables_desc_t {
    /** Free table data when generation > clear_generation. */
    uint16_t clear_generation;

    /** Delete table when generation > delete_generation. */
    uint16_t delete_generation;

    /** Amount of time operation is allowed to spend. */
    double time_budget_seconds;

    /** Table index to start scanning at. The function loops around until it
     * reaches this offset again, or until the time budget is exceeded. */
    int32_t offset;
} ecs_delete_empty_tables_desc_t;

/** Clean up empty tables.
 * This operation cleans up empty tables that meet certain conditions. Having
 * large amounts of empty tables does not negatively impact performance of the
 * ECS, but can take up considerable amounts of memory, especially in
 * applications with many components, and many components per entity.
 *
 * The generation specifies the minimum number of times this operation has
 * to be called before an empty table is cleaned up. If a table becomes
 * non-empty, the generation is reset.
 *
 * The operation allows for both a "clear" generation and a "delete"
 * generation. When the clear generation is reached, the table's
 * resources are freed (like component arrays) but the table itself is not
 * deleted. When the delete generation is reached, the empty table is deleted.
 *
 * By specifying a non-zero ID, the cleanup logic can be limited to tables with
 * a specific (component) ID. The operation will only increase the generation
 * count of matching tables.
 *
 * The min_id_count specifies a lower bound for the number of components a table
 * should have. Often the more components a table has, the more specific it is
 * and therefore less likely to be reused.
 *
 * The time budget specifies how long the operation should take at most.
 *
 * The offset parameter specifies the table index at which to start scanning.
 * The function loops around until it reaches this offset again, or until the
 * time budget is exceeded.
 *
 * @param world The world.
 * @param desc Configuration parameters.
 * @return The index + 1 of the table where the function stopped, or 0 if the
 *         function scanned all tables. The return value can be used as the
 *         offset for the next call.
 */
FLECS_API
int32_t ecs_delete_empty_tables(
    ecs_world_t *world,
    const ecs_delete_empty_tables_desc_t *desc);

/** Get the world from a poly.
 *
 * @param poly A pointer to a poly object.
 * @return The world.
 */
FLECS_API
const ecs_world_t* ecs_get_world(
    const ecs_poly_t *poly);

/** Get the entity from a poly.
 *
 * @param poly A pointer to a poly object.
 * @return The entity associated with the poly object.
 */
FLECS_API
ecs_entity_t ecs_get_entity(
    const ecs_poly_t *poly);

/** Test if a pointer is of the specified type.
 * Usage:
 *
 * @code
 * flecs_poly_is(ptr, ecs_world_t)
 * @endcode
 *
 * This operation only works for poly types.
 *
 * @param object The object to test.
 * @param type The ID of the type.
 * @return True if the pointer is of the specified type.
 */
FLECS_API
bool flecs_poly_is_(
    const ecs_poly_t *object,
    int32_t type);

/** Test if a pointer is of the specified type.
 * @see flecs_poly_is_()
 */
#define flecs_poly_is(object, type)\
    flecs_poly_is_(object, type##_magic)

/** Make a pair ID.
 * This function is equivalent to using the ecs_pair() macro, and is added for
 * convenience to make it easier for non-C/C++ bindings to work with pairs.
 *
 * @param first The first element of the pair.
 * @param second The target of the pair.
 * @return A pair ID.
 */
FLECS_API
ecs_id_t ecs_make_pair(
    ecs_entity_t first,
    ecs_entity_t second);

/** Begin exclusive thread access.
 * This operation ensures that only the thread from which this operation is 
 * called can access the world. Attempts to access the world from other threads
 * will panic.
 * 
 * ecs_exclusive_access_begin() must be called in pairs with 
 * ecs_exclusive_access_end(). Calling ecs_exclusive_access_begin() from another
 * thread without first calling ecs_exclusive_access_end() will panic.
 * 
 * A thread name can be provided to the function to improve debug messages. The
 * function does not copy the thread name, which means the memory for the 
 * name must remain alive for as long as the thread has exclusive access.
 * 
 * This operation should only be called once per thread. Calling it multiple 
 * times for the same thread will cause a panic.
 * 
 * Note that this feature only works in builds where asserts are enabled. The
 * feature requires the OS API thread_self_ callback to be set.
 * 
 * @param world The world.
 * @param thread_name The name of the thread obtaining exclusive access.
 */
FLECS_API
void ecs_exclusive_access_begin(
    ecs_world_t *world,
    const char *thread_name);

/** End exclusive thread access.
 * This operation should be called after ecs_exclusive_access_begin(). After
 * calling this operation, other threads are no longer prevented from mutating
 * the world.
 * 
 * When "lock_world" is set to true, no thread will be able to mutate the world
 * until ecs_exclusive_access_begin() is called again. While the world is locked,
 * only read-only operations are allowed. For example, ecs_get_id() is allowed,
 * but ecs_get_mut_id() is not allowed.
 * 
 * A locked world can be unlocked by calling ecs_exclusive_access_end() again with
 * lock_world set to false. Note that this only works for locked worlds. If
 * ecs_exclusive_access_end() is called on a world that has exclusive thread
 * access from a different thread, a panic will happen.
 * 
 * This operation must be called from the same thread that called
 * ecs_exclusive_access_begin(). Calling it from a different thread will cause
 * a panic.
 * 
 * @param world The world.
 * @param lock_world When true, any mutations on the world will be blocked.
 */
FLECS_API
void ecs_exclusive_access_end(
    ecs_world_t *world,
    bool lock_world);

/** @} */

/** @} */

/**
 * @defgroup entities Entities
 * Functions for working with `ecs_entity_t`.
 *
 * @{
 */

/**
 * @defgroup creating_entities Creating & Deleting
 * Functions for creating and deleting entities.
 *
 * @{
 */

/** Create new entity ID.
 * This operation returns an unused entity ID. This operation is guaranteed to
 * return an empty entity as it does not use values set by ecs_set_scope() or
 * ecs_set_with().
 *
 * @param world The world.
 * @return The new entity ID.
 */
FLECS_API
ecs_entity_t ecs_new(
    ecs_world_t *world);

/** Create new low ID.
 * This operation returns a new low ID. Entity IDs start after the
 * FLECS_HI_COMPONENT_ID constant. This reserves a range of low IDs for things
 * like components, and allows parts of the code to optimize operations.
 *
 * Note that FLECS_HI_COMPONENT_ID does not represent the maximum number of
 * components that can be created, only the maximum number of components that
 * can take advantage of these optimizations.
 *
 * This operation is guaranteed to return an empty entity as it does not use
 * values set by ecs_set_scope() or ecs_set_with().
 *
 * This operation does not recycle IDs.
 *
 * @param world The world.
 * @return The new component ID.
 */
FLECS_API
ecs_entity_t ecs_new_low_id(
    ecs_world_t *world);

/** Create new entity with (component) ID.
 * This operation creates a new entity with an optional (component) ID.
 *
 * @param world The world.
 * @param component The component to create the new entity with.
 * @return The new entity.
 */
FLECS_API
ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t component);

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
 * If an ID to a non-existing entity is provided, that entity ID becomes alive.
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

/** Bulk create or populate new entities.
 * This operation bulk inserts a list of new or predefined entities into a
 * single table.
 *
 * The operation does not take ownership of component arrays provided by the
 * application. Components that are non-trivially copyable will be moved into
 * the storage.
 *
 * The operation will emit OnAdd events for each added ID, and OnSet events for
 * each component that has been set.
 *
 * If no entity IDs are provided by the application, the returned array of IDs
 * points to an internal data structure, which changes when new entities are
 * created or deleted.
 *
 * If as a result of the operation, observers are invoked that delete
 * entities and no entity IDs were provided by the application, the returned
 * array of identifiers may be incorrect. To avoid this problem, an application
 * can first call ecs_bulk_init() to create empty entities, copy the array to one
 * that is owned by the application, and then use this array to populate the
 * entities.
 *
 * @param world The world.
 * @param desc Bulk creation parameters.
 * @return An array with the list of entity IDs created or populated.
 */
FLECS_API
const ecs_entity_t* ecs_bulk_init(
    ecs_world_t *world,
    const ecs_bulk_desc_t *desc);

/** Create N new entities.
 * This operation is the same as ecs_new_w_id(), but creates N entities
 * instead of one.
 *
 * @param world The world.
 * @param component The component to create the entities with.
 * @param count The number of entities to create.
 * @return An array with the entity IDs of the newly created entities.
 */
FLECS_API
const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t component,
    int32_t count);

/** Clone an entity.
 * This operation clones the components of one entity into another entity. If
 * no destination entity is provided, a new entity will be created. Component
 * values are not copied unless copy_value is true.
 *
 * If the source entity has a name, it will not be copied to the destination
 * entity. This is to prevent having two entities with the same name under the
 * same parent, which is not allowed.
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
 * This operation will delete an entity and all of its components. The entity ID
 * will be made available for recycling. If the entity passed to ecs_delete() is
 * not alive, the operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_API
void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Delete all entities with the specified component.
 * This will delete all entities (tables) that have the specified ID. The 
 * component may be a wildcard and/or a pair.
 *
 * @param world The world.
 * @param component The component.
 */
FLECS_API
void ecs_delete_with(
    ecs_world_t *world,
    ecs_id_t component);

/** Set child order for parent with OrderedChildren.
 * If the parent has the OrderedChildren trait, the order of the children 
 * will be updated to the order in the specified children array. The operation
 * will fail if the parent does not have the OrderedChildren trait.
 * 
 * This operation always takes place immediately, and is not deferred. When the 
 * operation is called from a multithreaded system, it will fail.
 * 
 * The reason for not deferring this operation is that by the time the deferred
 * command would be executed, the children of the parent could have been changed
 * which would cause the operation to fail.
 * 
 * @param world The world.
 * @param parent The parent.
 * @param children An array with children.
 * @param child_count The number of children in the provided array.
 */
FLECS_API
void ecs_set_child_order(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_entity_t *children,
    int32_t child_count);

/** Get ordered children.
 * If a parent has the OrderedChildren trait, this operation can be used to
 * obtain the array with child entities. If this operation is used on a parent
 * that does not have the OrderedChildren trait, it will fail.
 * 
 * @param world The world.
 * @param parent The parent.
 * @return The array with child entities.
 */
FLECS_API
ecs_entities_t ecs_get_ordered_children(
    const ecs_world_t *world,
    ecs_entity_t parent);

/** @} */

/**
 * @defgroup adding_removing Adding & Removing
 * Functions for adding and removing components.
 *
 * @{
 */

/** Add a (component) ID to an entity.
 * This operation adds a single (component) ID to an entity. If the entity
 * already has the ID, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component ID to add.
 */
FLECS_API
void ecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Remove a component from an entity.
 * This operation removes a single component from an entity. If the entity
 * does not have the component, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to remove.
 */
FLECS_API
void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Add an auto override for a component.
 * An auto override is a component that is automatically added to an entity when
 * it is instantiated from a prefab. Auto overrides are added to the entity that
 * is inherited from (usually a prefab). For example:
 * 
 * @code
 * ecs_entity_t prefab = ecs_insert(world,
 *   ecs_value(Position, {10, 20}),
 *   ecs_value(Mass, {100}));
 * 
 * ecs_auto_override(world, prefab, Position);
 * 
 * ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, prefab);
 * assert(ecs_owns(world, inst, Position)); // true
 * assert(ecs_owns(world, inst, Mass)); // false
 * @endcode
 * 
 * An auto override is equivalent to a manual override:
 * 
 * @code
 * ecs_entity_t prefab = ecs_insert(world,
 *   ecs_value(Position, {10, 20}),
 *   ecs_value(Mass, {100}));
 * 
 * ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, prefab);
 * assert(ecs_owns(world, inst, Position)); // false
 * ecs_add(world, inst, Position); // manual override
 * assert(ecs_owns(world, inst, Position)); // true
 * assert(ecs_owns(world, inst, Mass)); // false
 * @endcode
 * 
 * This operation is equivalent to manually adding the ID with the AUTO_OVERRIDE
 * bit applied:
 *
 * @code
 * ecs_add_id(world, entity, ECS_AUTO_OVERRIDE | id);
 * @endcode
 * 
 * When a component is overridden and inherited from a prefab, the value from 
 * the prefab component is copied to the instance. When the component is not
 * inherited from a prefab, it is added to the instance as if using ecs_add_id().
 * 
 * Overriding is the default behavior on prefab instantiation. Auto overriding
 * is only useful for components with the `(OnInstantiate, Inherit)` trait.
 * When a component has the `(OnInstantiate, DontInherit)` trait and is overridden,
 * the component is added, but the value from the prefab will not be copied.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to auto override.
 */
FLECS_API
void ecs_auto_override_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Clear all components.
 * This operation will remove all components from an entity.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_API
void ecs_clear(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Remove all instances of the specified component.
 * This will remove the specified ID from all entities (tables). The ID may be
 * a wildcard and/or a pair.
 *
 * @param world The world.
 * @param component The component.
 */
FLECS_API
void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t component);

/** Create new entities with a specified component.
 * This operation configures a component that is automatically added to entities
 * created with ecs_entity_init(). This does not apply to entities created with
 * ecs_new().
 * 
 * Only one component can be specified at a time. If this operation is called 
 * while a component is already configured, the new component will override the
 * old component.
 *
 * @param world The world.
 * @param component The component.
 * @return The previously set component.
 * @see ecs_entity_init()
 * @see ecs_get_with()
 */
FLECS_API
ecs_entity_t ecs_set_with(
    ecs_world_t *world,
    ecs_id_t component);

/** Get the component set with ecs_set_with().
 * This operation returns the component that was previously provided to
 * ecs_set_with().
 *
 * @param world The world.
 * @return The last component provided to ecs_set_with().
 * @see ecs_set_with()
 */
FLECS_API
ecs_id_t ecs_get_with(
    const ecs_world_t *world);

/** @} */

/**
 * @defgroup enabling_disabling Enabling & Disabling
 * Functions for enabling/disabling entities and components.
 *
 * @{
 */

/** Enable or disable an entity.
 * This operation enables or disables an entity by adding or removing the
 * #EcsDisabled tag. A disabled entity will not be matched with any systems,
 * unless the system explicitly specifies the #EcsDisabled tag.
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

/** Enable or disable a component.
 * Enabling or disabling a component does not add or remove a component from an
 * entity, but prevents it from being matched with queries. This operation can
 * be useful when a component must be temporarily disabled without destroying
 * its value. It is also a more performant operation for when an application
 * needs to add/remove components at high frequency, as enabling/disabling is
 * cheaper than a regular add or remove.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to enable/disable.
 * @param enable True to enable the component, false to disable.
 */
FLECS_API
void ecs_enable_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    bool enable);

/** Test if a component is enabled.
 * Test whether a component is currently enabled or disabled. This operation
 * will return true when the entity has the component and if it has not been
 * disabled by ecs_enable_id().
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component.
 * @return True if the component is enabled, otherwise false.
 */
FLECS_API
bool ecs_is_enabled_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** @} */

/**
 * @defgroup getting Getting and Setting
 * Functions for getting and setting components.
 *
 * @{
 */

/** Get an immutable pointer to a component.
 * This operation obtains a const pointer to the requested component. The
 * operation accepts the component entity ID.
 * 
 * This operation can return inherited components reachable through an `IsA`
 * relationship.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to get.
 * @return The component pointer, NULL if the entity does not have the component.
 *
 * @see ecs_get_mut_id()
 */
FLECS_API
FLECS_ALWAYS_INLINE const void* ecs_get_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Get a mutable pointer to a component.
 * This operation obtains a mutable pointer to the requested component. The
 * operation accepts the component entity ID.
 * 
 * Unlike ecs_get_id(), this operation does not return inherited components. 
 * This is to prevent errors where an application accidentally resolves an
 * inherited component shared with many entities and modifies it, while thinking
 * it is modifying an owned component.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to get.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_API
FLECS_ALWAYS_INLINE void* ecs_get_mut_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Ensure an entity has a component and return a pointer.
 * This operation returns a mutable pointer to a component. If the entity did
 * not yet have the component, it will be added.
 *
 * If ensure() is called when the world is in deferred or read-only mode, the
 * function will:
 * - return a pointer to temporary storage if the component does not yet exist, or
 * - return a pointer to the existing component if it exists
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to get or add.
 * @param size The size of the component.
 * @return The component pointer.
 *
 * @see ecs_emplace_id()
 */
FLECS_API
void* ecs_ensure_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size);

/** Create a component ref.
 * A ref is a handle to an entity and component pair, which caches a small amount of
 * data to reduce the overhead of repeatedly accessing the component. Use
 * ecs_ref_get() to get the component data.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to create a ref for.
 * @return The reference.
 */
FLECS_ALWAYS_INLINE FLECS_API
ecs_ref_t ecs_ref_init_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Get a component from a ref.
 * Get a component pointer from a ref. The ref must be created with ecs_ref_init().
 * The specified component must match the component with which the ref was 
 * created.
 *
 * @param world The world.
 * @param ref The ref.
 * @param component The component to get.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_ALWAYS_INLINE FLECS_API
void* ecs_ref_get_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t component);

/** Update a ref.
 * Ensure the contents of a ref are up to date. Same as ecs_ref_get_id(), but does not
 * return a pointer to the component.
 *
 * @param world The world.
 * @param ref The ref.
 * @param component The component the ref was created with.
 */
FLECS_ALWAYS_INLINE FLECS_API
void ecs_ref_update(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t component);

/** Emplace a component.
 * Emplace is similar to ecs_ensure_id() except that the component constructor
 * is not invoked for the returned pointer, allowing the component to be 
 * constructed directly in the storage.
 * 
 * When the `is_new` parameter is not provided, the operation will assert when the
 * component already exists. When the `is_new` parameter is provided, it will
 * indicate whether the returned storage has been constructed.
 * 
 * When `is_new` indicates that the storage has not yet been constructed, it must
 * be constructed by the code invoking this operation. Not constructing the
 * component will result in undefined behavior.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to get or add.
 * @param size The component size.
 * @param is_new Whether this is an existing or new component.
 * @return The (uninitialized) component pointer.
 */
FLECS_API
void* ecs_emplace_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size,
    bool *is_new);

/** Signal that a component has been modified.
 * This operation is usually used after modifying a component value obtained by
 * ecs_ensure_id(). The operation will mark the component as dirty, and invoke
 * OnSet observers and hooks.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component that was modified.
 */
FLECS_API
void ecs_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Set the value of a component.
 * This operation allows an application to set the value of a component. The
 * operation is equivalent to calling ecs_ensure_id() followed by
 * ecs_modified_id(). The operation will not modify the value of the passed-in
 * component. If the component has a copy hook registered, it will be used to
 * copy in the component.
 *
 * If the provided entity is 0, a new entity will be created.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to set.
 * @param size The size of the pointed-to value.
 * @param ptr The pointer to the value.
 */
FLECS_API
void ecs_set_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size,
    const void *ptr);

/** @} */

/**
 * @defgroup liveliness Entity Liveliness
 * Functions for testing and modifying entity liveliness.
 *
 * @{
 */

/** Test whether an entity is valid.
 * This operation tests whether the entity ID:
 * - is not 0
 * - has a valid bit pattern
 * - is alive (see ecs_is_alive())
 * 
 * If this operation returns true, it is safe to use the entity with
 * other operations.
 * 
 * This operation should only be used if an application cannot be sure that an
 * entity is initialized with a valid value. In all other cases where an entity
 * was initialized with a valid value, but the application wants to check if the
 * entity is (still) alive, use ecs_is_alive().
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity is valid, false if the entity is not valid.
 * @see ecs_is_alive()
 */
FLECS_API
bool ecs_is_valid(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Test whether an entity is alive.
 * Entities are alive after they are created, and become not alive when they are
 * deleted. Operations that return alive IDs are (amongst others) ecs_new(),
 * ecs_new_low_id() and ecs_entity_init(). IDs can be made alive with the 
 * ecs_make_alive() function.
 *
 * After an ID is deleted it can be recycled. Recycled IDs are different from
 * the original ID in that they have a different generation count. This makes it
 * possible for the API to distinguish between the two. An example:
 *
 * @code
 * ecs_entity_t e1 = ecs_new(world);
 * ecs_is_alive(world, e1);             // true
 * ecs_delete(world, e1);
 * ecs_is_alive(world, e1);             // false
 *
 * ecs_entity_t e2 = ecs_new(world);    // recycles e1
 * ecs_is_alive(world, e2);             // true
 * ecs_is_alive(world, e1);             // false
 * @endcode
 * 
 * Unlike ecs_is_valid(), this operation will panic if the passed-in entity
 * ID is 0 or has an invalid bit pattern.
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity is alive, false if the entity is not alive.
 * @see ecs_is_valid()
 */
FLECS_API
bool ecs_is_alive(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Remove the generation from an entity ID.
 *
 * @param e The entity ID.
 * @return The entity ID without the generation count.
 */
FLECS_API
ecs_id_t ecs_strip_generation(
    ecs_entity_t e);

/** Get an alive identifier.
 * In some cases an application may need to work with identifiers from which
 * the generation has been stripped. A typical scenario in which this happens is
 * when iterating relationships in an entity type.
 *
 * For example, when obtaining the parent ID from a `ChildOf` relationship, the parent
 * (second element of the pair) will have been stored in a 32-bit value, which
 * cannot store the entity generation. This function can retrieve the identifier
 * with the current generation for that ID.
 *
 * If the provided identifier is not alive, the function will return 0.
 *
 * @param world The world.
 * @param e The entity for which to obtain the current alive entity ID.
 * @return The alive entity ID if there is one, or 0 if the ID is not alive.
 */
FLECS_API
ecs_entity_t ecs_get_alive(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Ensure an ID is alive.
 * This operation ensures that the provided ID is alive. This is useful in
 * scenarios where an application has an existing ID that has not been created
 * with ecs_new_w() (such as a global constant or an ID from a remote application).
 *
 * When this operation is successful, it guarantees that the provided ID exists,
 * is valid, and is alive.
 *
 * Before this operation, the ID must either not be alive or have a generation
 * that is equal to the passed-in entity.
 *
 * If the provided ID has a non-zero generation count and the ID does not exist
 * in the world, the ID will be created with the specified generation.
 *
 * If the provided ID is alive and has a generation count that does not match
 * the provided ID, the operation will fail.
 *
 * @param world The world.
 * @param entity The entity ID to make alive.
 *
 * @see ecs_make_alive_id()
 */
FLECS_API
void ecs_make_alive(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Same as ecs_make_alive(), but for components.
 * An ID can be an entity or a pair, and can contain ID flags. This operation
 * ensures that the entity (or entities, for a pair) are alive.
 *
 * When this operation is successful, it guarantees that the provided ID can be
 * used in operations that accept an ID.
 *
 * Since entities in a pair do not encode their generation IDs, this operation
 * will not fail when an entity with non-zero generation count already exists in
 * the world.
 *
 * This is different from ecs_make_alive(), which will fail if attempted with an ID
 * that has generation 0 and an entity with a non-zero generation is currently
 * alive.
 *
 * @param world The world.
 * @param component The component to make alive.
 */
FLECS_API
void ecs_make_alive_id(
    ecs_world_t *world,
    ecs_id_t component);

/** Test whether an entity exists.
 * Similar to ecs_is_alive(), but ignores the entity generation count.
 *
 * @param world The world.
 * @param entity The entity.
 * @return True if the entity exists, false if the entity does not exist.
 */
FLECS_API
bool ecs_exists(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Override the generation of an entity.
 * The generation count of an entity is increased each time an entity is deleted
 * and is used to test whether an entity ID is alive.
 *
 * This operation overrides the current generation of an entity with the
 * specified generation, which can be useful if an entity is externally managed,
 * like for external pools, savefiles, or netcode.
 * 
 * This operation is similar to ecs_make_alive(), except that it will also
 * override the generation of an alive entity.
 *
 * @param world The world.
 * @param entity The entity for which to set the generation.
 */
FLECS_API
void ecs_set_version(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Get the generation of an entity.
 *
 * @param entity The entity for which to get the generation.
 * @return The generation of the entity.
 */
FLECS_API
uint32_t ecs_get_version(
    ecs_entity_t entity);

/** @} */

/**
 * @defgroup entity_info Entity Information.
 * Get information from an entity.
 *
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
 * @return The table of the entity, NULL if the entity has no components or tags.
 */
FLECS_API
ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Convert a type to a string.
 * The result of this operation must be freed with ecs_os_free().
 *
 * @param world The world.
 * @param type The type.
 * @return The stringified type.
 */
FLECS_API
char* ecs_type_str(
    const ecs_world_t *world,
    const ecs_type_t* type);

/** Convert a table to a string.
 * Same as `ecs_type_str(world, ecs_table_get_type(table))`. The result of this
 * operation must be freed with ecs_os_free().
 *
 * @param world The world.
 * @param table The table.
 * @return The stringified table type.
 *
 * @see ecs_table_get_type()
 * @see ecs_type_str()
 */
FLECS_API
char* ecs_table_str(
    const ecs_world_t *world,
    const ecs_table_t *table);

/** Convert an entity to a string.
 * Same as combining:
 * - ecs_get_path(world, entity)
 * - ecs_type_str(world, ecs_get_type(world, entity))
 *
 * The result of this operation must be freed with ecs_os_free().
 *
 * @param world The world.
 * @param entity The entity.
 * @return The entity path with stringified type.
 *
 * @see ecs_get_path()
 * @see ecs_type_str()
 */
FLECS_API
char* ecs_entity_str(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Test if an entity has a component.
 * This operation returns true if the entity has or inherits the component.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to test for.
 * @return True if the entity has the component, false if not.
 *
 * @see ecs_owns_id()
 */
FLECS_API
FLECS_ALWAYS_INLINE bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Test if an entity owns a component.
 * This operation returns true if the entity has the component. The operation
 * behaves the same as ecs_has_id(), except that it will return false for
 * components that are inherited through an `IsA` relationship.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to test for.
 * @return True if the entity has the component, false if not.
 */
FLECS_API
FLECS_ALWAYS_INLINE bool ecs_owns_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component);

/** Get the target of a relationship.
 * This will return a target (second element of a pair) of the entity for the
 * specified relationship. The index allows for iterating through the targets,
 * if a single entity has multiple targets for the same relationship.
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

/** Get the parent (target of the `ChildOf` relationship) for an entity.
 * This operation is the same as calling:
 *
 * @code
 * ecs_get_target(world, entity, EcsChildOf, 0);
 * @endcode
 *
 * @param world The world.
 * @param entity The entity.
 * @return The parent of the entity, 0 if the entity has no parent.
 *
 * @see ecs_get_target()
 */
FLECS_API
ecs_entity_t ecs_get_parent(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Create child with Parent component.
 * This creates or returns an existing child for the specified parent. If a new
 * child is created, the Parent component is used to create the parent 
 * relationship.
 * 
 * If a child entity already exists with the specified name, it will be 
 * returned.
 * 
 * @param world The world.
 * @param parent The parent for which to create the child.
 * @param name The name with which to create the entity (may be NULL).
 * @return A new or existing child entity.
 */
FLECS_API
ecs_entity_t ecs_new_w_parent(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name);

/** Get the target of a relationship for a given component.
 * This operation returns the first entity that has the provided component by 
 * following the relationship. If the entity itself has the component then it 
 * will be returned. If the component cannot be found on the entity or by 
 * following the relationship, the operation will return 0.
 *
 * This operation can be used to look up, for example, which prefab is providing
 * a component by specifying the `IsA` relationship:
 *
 * @code
 * // Is Position provided by the entity or one of its base entities?
 * ecs_get_target_for_id(world, entity, EcsIsA, ecs_id(Position))
 * @endcode
 *
 * @param world The world.
 * @param entity The entity.
 * @param rel The relationship to follow.
 * @param component The component to look up.
 * @return The entity for which the target has been found.
 */
FLECS_API
ecs_entity_t ecs_get_target_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t component);

/** Return the depth for an entity in the tree for the specified relationship.
 * Depth is determined by counting the number of targets encountered while
 * traversing up the relationship tree for `rel`. Only acyclic relationships are
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

/** Count entities that have the specified ID.
 * Return the number of entities that have the specified ID.
 *
 * @param world The world.
 * @param entity The ID to search for.
 * @return The number of entities that have the ID.
 */
FLECS_API
int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_id_t entity);

/** @} */

/**
 * @defgroup paths Entity Names
 * Functions for working with entity names and paths.
 *
 * @{
 */

/** Get the name of an entity.
 * This will return the name stored in `(EcsIdentifier, EcsName)`.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The name of the entity, NULL if the entity has no name.
 *
 * @see ecs_set_name()
 */
FLECS_API
const char* ecs_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the symbol of an entity.
 * This will return the symbol stored in `(EcsIdentifier, EcsSymbol)`.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The symbol of the entity, NULL if the entity has no symbol.
 *
 * @see ecs_set_symbol()
 */
FLECS_API
const char* ecs_get_symbol(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Set the name of an entity.
 * This will set or overwrite the name of an entity. If no entity is provided,
 * a new entity will be created.
 *
 * The name is stored in `(EcsIdentifier, EcsName)`.
 *
 * @param world The world.
 * @param entity The entity.
 * @param name The name.
 * @return The provided entity, or a new entity if 0 was provided.
 *
 * @see ecs_get_name()
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
 * The symbol is stored in `(EcsIdentifier, EcsSymbol)`.
 *
 * @param world The world.
 * @param entity The entity.
 * @param symbol The symbol.
 * @return The provided entity, or a new entity if 0 was provided.
 *
 * @see ecs_get_symbol()
 */
FLECS_API
ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *symbol);

/** Set an alias for an entity.
 * An entity can be looked up using its alias from the root scope without
 * providing the fully qualified name of its parent. An entity can only have
 * a single alias.
 *
 * The alias is stored in `(EcsIdentifier, EcsAlias)`.
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

/** Look up an entity by its path.
 * This operation is equivalent to calling:
 *
 * @code
 * ecs_lookup_path_w_sep(world, 0, path, ".", NULL, true);
 * @endcode
 *
 * @param world The world.
 * @param path The entity path.
 * @return The entity with the specified path, or 0 if no entity was found.
 *
 * @see ecs_lookup_child()
 * @see ecs_lookup_path_w_sep()
 * @see ecs_lookup_symbol()
 */
FLECS_API
ecs_entity_t ecs_lookup(
    const ecs_world_t *world,
    const char *path);

/** Look up a child entity by name.
 * Return an entity that matches the specified name. Only look for entities in
 * the provided parent. If no parent is provided, look in the current scope
 * (root if no scope is provided).
 *
 * @param world The world.
 * @param parent The parent for which to look up the child.
 * @param name The entity name.
 * @return The entity with the specified name, or 0 if no entity was found.
 *
 * @see ecs_lookup()
 * @see ecs_lookup_path_w_sep()
 * @see ecs_lookup_symbol()
 */
FLECS_API
ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name);

/** Look up an entity from a path.
 * Look up an entity from a provided path, relative to the provided parent. The
 * operation will use the provided separator to tokenize the path expression. If
 * the provided path contains the prefix, the search will start from the root.
 *
 * If the entity is not found in the provided parent, the operation will
 * continue to search in the parent of the parent, until the root is reached. If
 * the entity is still not found, the lookup will search in the `flecs.core`
 * scope. If the entity is not found there either, the function returns 0.
 *
 * @param world The world.
 * @param parent The entity from which to resolve the path.
 * @param path The path to resolve.
 * @param sep The path separator.
 * @param prefix The path prefix.
 * @param recursive Recursively traverse up the tree until the entity is found.
 * @return The entity if found, else 0.
 *
 * @see ecs_lookup()
 * @see ecs_lookup_child()
 * @see ecs_lookup_symbol()
 */
FLECS_API
ecs_entity_t ecs_lookup_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix,
    bool recursive);

/** Look up an entity by its symbol name.
 * This looks up an entity by the symbol stored in `(EcsIdentifier, EcsSymbol)`. The
 * operation does not take into account hierarchies.
 *
 * This operation can be useful to resolve, for example, a type by its C
 * identifier, which does not include the Flecs namespacing.
 *
 * @param world The world.
 * @param symbol The symbol.
 * @param lookup_as_path If not found as a symbol, look up as path.
 * @param recursive If looking up as path, recursively traverse up the tree.
 * @return The entity if found, else 0.
 *
 * @see ecs_lookup()
 * @see ecs_lookup_child()
 * @see ecs_lookup_path_w_sep()
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
 * separator. If no parent is provided, the path will be relative to the root. If
 * a prefix is provided, the path will be prefixed by the prefix.
 *
 * If the parent is equal to the provided child, the operation will return an
 * empty string. If a non-zero component is provided, the path will be created by
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
 *
 * @see ecs_get_path_w_sep_buf()
 */
FLECS_API
char* ecs_get_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix);

/** Write a path identifier to a buffer.
 * Same as ecs_get_path_w_sep(), but writes the result to an `ecs_strbuf_t`.
 *
 * @param world The world.
 * @param parent The entity from which to create the path.
 * @param child The entity to which to create the path.
 * @param sep The separator to use between path elements.
 * @param prefix The initial character to use for root elements.
 * @param buf The buffer to write to.
 * @param escape Whether to escape separator characters in names.
 *
 * @see ecs_get_path_w_sep()
 */
FLECS_API
void ecs_get_path_w_sep_buf(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf,
    bool escape);

/** Find or create an entity from a path.
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

/** Add a specified path to an entity.
 * This operation is similar to ecs_new_from_path(), but will instead add the path
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
 * As a result, new entities will be created in this scope, and lookups will be
 * relative to the provided scope.
 *
 * It is considered good practice to restore the scope to the old value.
 *
 * @param world The world.
 * @param scope The entity to use as scope.
 * @return The previous scope.
 *
 * @see ecs_get_scope()
 */
FLECS_API
ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope);

/** Get the current scope.
 * Get the scope set by ecs_set_scope(). If no scope is set, this operation will
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
 * prefix. The name prefix is currently only used by `ECS_COMPONENT`.
 *
 * @param world The world.
 * @param prefix The name prefix to use.
 * @return The previous prefix.
 */
FLECS_API
const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix);

/** Set the search path for lookup operations.
 * This operation accepts an array of entity IDs that will be used as search
 * scopes by lookup operations. The operation returns the current search path.
 * It is good practice to restore the old search path.
 *
 * The search path will be evaluated starting from the last element.
 *
 * The default search path includes `flecs.core`. When a custom search path is
 * provided, it overwrites the existing search path. Operations that rely on
 * looking up names from `flecs.core` without providing the namespace may fail if
 * the custom search path does not include `flecs.core` (`EcsFlecsCore`).
 *
 * The search path array is not copied into managed memory. The application must
 * ensure that the provided array is valid for as long as it is used as the
 * search path.
 *
 * The provided array must be terminated with a 0 element. This enables an
 * application to push or pop elements to an existing array without invoking the
 * ecs_set_lookup_path() operation again.
 *
 * @param world The world.
 * @param lookup_path 0-terminated array with entity IDs for the lookup path.
 * @return The current lookup path array.
 *
 * @see ecs_get_lookup_path()
 */
FLECS_API
ecs_entity_t* ecs_set_lookup_path(
    ecs_world_t *world,
    const ecs_entity_t *lookup_path);

/** Get the current lookup path.
 * Return the value set by ecs_set_lookup_path().
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
 * Functions for registering and working with components.
 *
 * @{
 */

/** Find or create a component.
 * This operation creates a new component, or finds an existing one. The find or
 * create behavior is the same as ecs_entity_init().
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

/** Get the type info for a component.
 * This function returns the type information for a component. The component can
 * be a regular component or a pair. For the rules on how type information is 
 * determined based on a component ID, see ecs_get_typeid().
 *
 * @param world The world.
 * @param component The component.
 * @return The type information of the component ID.
 */
FLECS_API
const ecs_type_info_t* ecs_get_type_info(
    const ecs_world_t *world,
    ecs_id_t component);

/** Register hooks for a component.
 * Hooks allow for the execution of user code when components are constructed,
 * copied, moved, destructed, added, removed, or set. Hooks can be assigned
 * as long as a component has not yet been used (added to an entity).
 *
 * The hooks that are currently set can be accessed with ecs_get_type_info().
 *
 * @param world The world.
 * @param component The component for which to register the actions.
 * @param hooks The type that contains the component actions.
 */
FLECS_API
void ecs_set_hooks_id(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_type_hooks_t *hooks);

/** Get hooks for a component.
 *
 * @param world The world.
 * @param component The component for which to retrieve the hooks.
 * @return The hooks for the component, or NULL if not registered.
 */
FLECS_API
const ecs_type_hooks_t* ecs_get_hooks_id(
    const ecs_world_t *world,
    ecs_entity_t component);

/** @} */

/**
 * @defgroup ids IDs
 * Functions for working with `ecs_id_t`.
 *
 * @{
 */

/** Return whether a specified component is a tag.
 * This operation returns whether the specified component is a tag (a component
 * without data or size).
 *
 * An ID is a tag when:
 * - it is an entity without the `EcsComponent` component
 * - it has an `EcsComponent` with size member set to 0
 * - it is a pair where both elements are a tag
 * - it is a pair where the first element has the #EcsPairIsTag tag
 *
 * @param world The world.
 * @param component The component.
 * @return Whether the provided ID is a tag.
 */
FLECS_API
bool ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t component);

/** Return whether a specified component is in use.
 * This operation returns whether a component is in use in the world. A 
 * component is in use if it has been added to one or more tables.
 *
 * @param world The world.
 * @param component The component.
 * @return Whether the component is in use.
 */
FLECS_API
bool ecs_id_in_use(
    const ecs_world_t *world,
    ecs_id_t component);

/** Get the type for a component.
 * This operation returns the type for a component ID, if the ID is associated
 * with a type. For a regular component with a non-zero size (an entity with the
 * EcsComponent component), the operation will return the component ID itself.
 *
 * For an entity that does not have the EcsComponent component, or with an
 * EcsComponent value with size 0, the operation will return 0.
 *
 * For a pair ID, the operation will return the type associated with the pair, by
 * applying the following queries in order:
 * - The first pair element is returned if it is a component.
 * - 0 is returned if the relationship entity has the Tag property.
 * - The second pair element is returned if it is a component.
 * - 0 is returned.
 *
 * @param world The world.
 * @param component The component.
 * @return The type of the component.
 */
FLECS_API
ecs_entity_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_id_t component);

/** Utility to match a component with a pattern.
 * This operation returns true if the provided pattern matches the provided
 * component. The pattern may contain a wildcard (or wildcards, when a pair).
 *
 * @param component The component.
 * @param pattern The pattern to compare with.
 * @return Whether the ID matches the pattern.
 */
FLECS_API
bool ecs_id_match(
    ecs_id_t component,
    ecs_id_t pattern);

/** Utility to check if a component is a pair.
 *
 * @param component The component.
 * @return True if the component is a pair.
 */
FLECS_API
bool ecs_id_is_pair(
    ecs_id_t component);

/** Utility to check if a component is a wildcard.
 *
 * @param component The component.
 * @return True if the component is a wildcard or a pair containing a wildcard.
 */
FLECS_API
bool ecs_id_is_wildcard(
    ecs_id_t component);

/** Utility to check if a component is an any wildcard.
 *
 * @param component The component.
 * @return True if the component is an any wildcard or a pair containing an any wildcard.
 */
bool ecs_id_is_any(
    ecs_id_t component);

/** Utility to check if an ID is valid.
 * A valid ID is an ID that can be added to an entity. Invalid IDs are:
 * - IDs that contain wildcards
 * - IDs that contain invalid entities
 * - IDs that are 0 or contain 0 entities
 *
 * Note that the same rules apply to removing from an entity, with the exception
 * of wildcards.
 *
 * @param world The world.
 * @param component The component.
 * @return True if the ID is valid.
 */
FLECS_API
bool ecs_id_is_valid(
    const ecs_world_t *world,
    ecs_id_t component);

/** Get flags associated with an ID.
 * This operation returns the internal flags (see api_flags.h) that are
 * associated with the provided ID.
 *
 * @param world The world.
 * @param component The component.
 * @return The flags associated with the ID, or 0 if the ID is not in use.
 */
FLECS_API
ecs_flags32_t ecs_id_get_flags(
    const ecs_world_t *world,
    ecs_id_t component);

/** Convert a component flag to a string.
 * This operation converts a component flag to a string. Possible outputs are:
 * 
 * - PAIR
 * - TOGGLE
 * - AUTO_OVERRIDE
 *
 * @param component_flags The component flag.
 * @return The ID flag string, or NULL if no valid ID is provided.
 */
FLECS_API
const char* ecs_id_flag_str(
    uint64_t component_flags);

/** Convert a component ID to a string.
 * This operation converts the provided component ID to a string. It can output
 * strings of the following formats:
 * 
 * - "ComponentName"
 * - "FLAG|ComponentName"
 * - "(Relationship, Target)"
 * - "FLAG|(Relationship, Target)"
 * 
 * The PAIR flag is never added to the string.
 *
 * @param world The world.
 * @param component The component to convert to a string.
 * @return The component converted to a string.
 */
FLECS_API
char* ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t component);

/** Write a component string to a buffer.
 * Same as ecs_id_str(), but writes the result to ecs_strbuf_t.
 *
 * @param world The world.
 * @param component The component to convert to a string.
 * @param buf The buffer to write to.
 */
FLECS_API
void ecs_id_str_buf(
    const ecs_world_t *world,
    ecs_id_t component,
    ecs_strbuf_t *buf);

/** Convert a string to a component.
 * This operation is the reverse of ecs_id_str(). The FLECS_SCRIPT addon
 * is required for this operation to work.
 *
 * @param world The world.
 * @param expr The string to convert to an ID.
 * @return The ID, or 0 if the string could not be converted.
 */
FLECS_API
ecs_id_t ecs_id_from_str(
    const ecs_world_t *world,
    const char *expr);

/** @} */

/**
 * @defgroup queries Queries
 * @brief Functions for working with `ecs_term_t` and `ecs_query_t`.
 * @{
 */

/** Test whether a term ref is set.
 * A term ref is a reference to an entity, component, or variable for one of the
 * three parts of a term (src, first, second).
 *
 * @param ref The term ref.
 * @return True when set, false when not set.
 */
FLECS_API 
bool ecs_term_ref_is_set(
    const ecs_term_ref_t *ref);

/** Test whether a term is set.
 * This operation can be used to test whether a term has been initialized with
 * values or whether it is empty.
 *
 * An application generally does not need to invoke this operation. It is useful
 * when initializing a 0-initialized array of terms (like in ecs_query_desc_t), as
 * this operation can be used to find the last initialized element.
 *
 * @param term The term.
 * @return True when set, false when not set.
 */
FLECS_API
bool ecs_term_is_initialized(
    const ecs_term_t *term);

/** Is a term matched on the $this variable.
 * This operation checks whether a term is matched on the $this variable, which
 * is the default source for queries.
 *
 * A term has a $this source when:
 * - ecs_term_t::src::id is EcsThis
 * - ecs_term_t::src::flags is EcsIsVariable
 *
 * If ecs_term_t::src is not populated, it will be automatically initialized to
 * the $this source for the created query.
 *
 * @param term The term.
 * @return True if the term matches $this, false if not.
 */
FLECS_API
bool ecs_term_match_this(
    const ecs_term_t *term);

/** Is a term matched on a 0 source.
 * This operation checks whether a term is matched on a 0 source. A 0 source is
 * a term that isn't matched against anything, and can be used just to pass
 * (component) IDs to a query iterator.
 *
 * A term has a 0 source when:
 * - ecs_term_t::src::id is 0
 * - ecs_term_t::src::flags has EcsIsEntity set
 *
 * @param term The term.
 * @return True if the term has a 0 source, false if not.
 */
FLECS_API
bool ecs_term_match_0(
    const ecs_term_t *term);

/** Convert a term to a string expression.
 * Convert a term to a string expression. The resulting expression is equivalent
 * to the same term, with the exception of And and Or operators.
 *
 * @param world The world.
 * @param term The term.
 * @return The term converted to a string.
 */
FLECS_API
char* ecs_term_str(
    const ecs_world_t *world,
    const ecs_term_t *term);

/** Convert a query to a string expression.
 * Convert a query to a string expression. The resulting expression can be
 * parsed to create the same query.
 * 
 * @param query The query.
 * @return The query converted to a string.
 */
FLECS_API 
char* ecs_query_str(
    const ecs_query_t *query); 

/** @} */

/**
 * @defgroup each_iter Each iterator
 * @brief Find all entities that have a single (component) ID.
 * @{
 */

/** Iterate all entities with a specified (component ID). 
 * This returns an iterator that yields all entities with a single specified
 * component. This is a much lighter-weight operation than creating and 
 * iterating a query.
 * 
 * Usage:
 * @code
 * ecs_iter_t it = ecs_each(world, Player);
 * while (ecs_each_next(&it)) {
 *   for (int i = 0; i < it.count; i ++) {
 *     // Iterate as usual.
 *   }
 * }
 * @endcode
 * 
 * If the specified ID is a component, it is possible to access the component
 * pointer with ecs_field() just like with regular queries:
 * 
 * @code
 * ecs_iter_t it = ecs_each(world, Position);
 * while (ecs_each_next(&it)) {
 *   Position *p = ecs_field(&it, Position, 0);
 *   for (int i = 0; i < it.count; i ++) {
 *     // Iterate as usual.
 *   }
 * }
 * @endcode
 * 
 * @param world The world.
 * @param component The component to iterate.
 * @return An iterator that iterates all entities with the (component) ID.
 */
FLECS_API
ecs_iter_t ecs_each_id(
    const ecs_world_t *world,
    ecs_id_t component);

/** Progress an iterator created with ecs_each_id().
 * 
 * @param it The iterator.
 * @return True if the iterator has more results, false if not.
 */
FLECS_API
bool ecs_each_next(
    ecs_iter_t *it);

/** Iterate children of a parent.
 * This operation is usually equivalent to doing:
 * @code
 * ecs_iter_t it = ecs_each_id(world, ecs_pair(EcsChildOf, parent));
 * @endcode
 * 
 * The only exception is when the parent has the EcsOrderedChildren trait, in
 * which case this operation will return a single result with the ordered 
 * child entity IDs.
 * 
 * This operation is equivalent to doing:
 * 
 * @code
 * ecs_children_w_rel(world, EcsChildOf, parent);
 * @endcode
 * 
 * @param world The world.
 * @param parent The parent.
 * @return An iterator that iterates all children of the parent.
 *
 * @see ecs_each_id()
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_iter_t ecs_children(
    const ecs_world_t *world,
    ecs_entity_t parent);

/** Same as ecs_children(), but with a custom relationship argument.
 * 
 * @param world The world.
 * @param relationship The relationship.
 * @param parent The parent.
 * @return An iterator that iterates all children of the parent.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_iter_t ecs_children_w_rel(
    const ecs_world_t *world,
    ecs_entity_t relationship,
    ecs_entity_t parent);

/** Progress an iterator created with ecs_children().
 * 
 * @param it The iterator.
 * @return True if the iterator has more results, false if not.
 */
FLECS_API
bool ecs_children_next(
    ecs_iter_t *it);

/** @} */

/**
 * @defgroup queries Queries
 * Functions for working with `ecs_query_t`.
 *
 * @{
 */

/** Create a query.
 * If the descriptor specifies an existing entity, the entity must not already
 * be associated with a query. To replace an existing query on an entity, use
 * ecs_query_update().
 *
 * @param world The world.
 * @param desc The descriptor (see ecs_query_desc_t).
 * @return The query.
 */
FLECS_API
ecs_query_t* ecs_query_init(
    ecs_world_t *world,
    const ecs_query_desc_t *desc);

/** Replace the query on an existing entity.
 * Removes the query currently attached to the entity and creates a new one
 * from the descriptor. Any handles to the previous query become invalid; use
 * the returned handle for subsequent iteration.
 *
 * @param world The world.
 * @param entity The entity that holds the query to replace.
 * @param desc The descriptor (see ecs_query_desc_t).
 * @return The new query, or NULL if the operation failed.
 */
FLECS_API
ecs_query_t* ecs_query_update(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_query_desc_t *desc);

/** Delete a query.
 *
 * @param query The query.
 */
FLECS_API
void ecs_query_fini(
    ecs_query_t *query);

/** Find a variable index.
 * This operation looks up the index of a variable in the query. This index can
 * be used in operations like ecs_iter_set_var() and ecs_iter_get_var().
 *
 * @param query The query.
 * @param name The variable name.
 * @return The variable index.
 */
FLECS_API
int32_t ecs_query_find_var(
    const ecs_query_t *query,
    const char *name);    

/** Get the variable name.
 * This operation returns the variable name for an index.
 *
 * @param query The query.
 * @param var_id The variable index.
 * @return The variable name.
 */
FLECS_API
const char* ecs_query_var_name(
    const ecs_query_t *query,
    int32_t var_id);

/** Test if a variable is an entity.
 * Internally, the query engine has entity variables and table variables. When
 * iterating through query variables (by using ecs_query_t::var_count) only
 * the values for entity variables are accessible. This operation enables an
 * application to check if a variable is an entity variable.
 *
 * @param query The query.
 * @param var_id The variable ID.
 * @return Whether the variable is an entity variable.
 */
FLECS_API
bool ecs_query_var_is_entity(
    const ecs_query_t *query,
    int32_t var_id);  

/** Create a query iterator.
 * Use an iterator to iterate through the entities that match a query. Queries
 * can return multiple results, and have to be iterated by repeatedly calling
 * ecs_query_next() until the operation returns false.
 * 
 * Depending on the query, a single result can contain an entire table, a range
 * of entities in a table, or a single entity. Iteration code has an inner and
 * an outer loop. The outer loop loops through the query results, and typically
 * corresponds with a table. The inner loop iterates entities in the result.
 * 
 * Example:
 * @code
 * ecs_iter_t it = ecs_query_iter(world, q);
 * 
 * while (ecs_query_next(&it)) {
 *   Position *p = ecs_field(&it, Position, 0);
 *   Velocity *v = ecs_field(&it, Velocity, 1);
 * 
 *   for (int i = 0; i < it.count; i ++) {
 *     p[i].x += v[i].x;
 *     p[i].y += v[i].y;
 *   }
 * }
 * @endcode
 * 
 * The world passed into the operation must be either the actual world or the
 * current stage, when iterating from a system. The stage is accessible through
 * the it.world member.
 * 
 * Example:
 * @code
 * void MySystem(ecs_iter_t *it) {
 *   ecs_query_t *q = it->ctx; // Query passed as system context
 * 
 *   // Create query iterator from system stage
 *   ecs_iter_t qit = ecs_query_iter(it->world, q);
 *   while (ecs_query_next(&qit)) {
 *     // Iterate as usual
 *   }
 * }
 * @endcode
 * 
 * If query iteration is stopped without the last call to ecs_query_next() 
 * returning false, iterator resources need to be cleaned up explicitly
 * with ecs_iter_fini().
 * 
 * Example:
 * @code
 * ecs_iter_t it = ecs_query_iter(world, q);
 * 
 * while (ecs_query_next(&it)) {
 *   if (!ecs_field_is_set(&it, 0)) {
 *     ecs_iter_fini(&it); // Free iterator resources
 *     break;
 *   }
 * 
 *   for (int i = 0; i < it.count; i ++) {
 *     // ...
 *   }
 * }
 * @endcode
 *
 * @param world The world.
 * @param query The query.
 * @return An iterator.
 *
 * @see ecs_query_next()
 */
FLECS_API
ecs_iter_t ecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *query);

/** Progress a query iterator.
 *
 * @param it The iterator.
 * @return True if the iterator has more results, false if not.
 *
 * @see ecs_query_iter()
 */
FLECS_API
bool ecs_query_next(
    ecs_iter_t *it);

/** Match an entity with a query.
 * This operation matches an entity with a query and returns the result of the
 * match in the "it" out parameter. An application should free the iterator
 * resources with ecs_iter_fini() if this function returns true.
 * 
 * Usage:
 * @code
 * ecs_iter_t it;
 * if (ecs_query_has(q, e, &it)) {
 *   ecs_iter_fini(&it);
 * }
 * @endcode
 * 
 * @param query The query.
 * @param entity The entity to match.
 * @param it The iterator with matched data.
 * @return True if entity matches the query, false if not.
 */
FLECS_API
bool ecs_query_has(
    const ecs_query_t *query,
    ecs_entity_t entity,
    ecs_iter_t *it);

/** Match a table with a query.
 * This operation matches a table with a query and returns the result of the
 * match in the "it" out parameter. An application should free the iterator
 * resources with ecs_iter_fini() if this function returns true.
 * 
 * Usage:
 * @code
 * ecs_iter_t it;
 * if (ecs_query_has_table(q, t, &it)) {
 *   ecs_iter_fini(&it);
 * }
 * @endcode
 * 
 * @param query The query.
 * @param table The table to match.
 * @param it The iterator with matched data.
 * @return True if table matches the query, false if not.
 */
FLECS_API
bool ecs_query_has_table(
    const ecs_query_t *query,
    ecs_table_t *table,
    ecs_iter_t *it);

/** Match a range with a query.
 * This operation matches a range with a query and returns the result of the
 * match in the "it" out parameter. An application should free the iterator
 * resources with ecs_iter_fini() if this function returns true.
 * 
 * The entire range must match the query for the operation to return true.
 * 
 * Usage:
 * @code
 * ecs_table_range_t range = {
 *   .table = table,
 *   .offset = 1,
 *   .count = 2
 * };
 * 
 * ecs_iter_t it;
 * if (ecs_query_has_range(q, &range, &it)) {
 *   ecs_iter_fini(&it);
 * }
 * @endcode
 * 
 * @param query The query.
 * @param range The range to match.
 * @param it The iterator with matched data.
 * @return True if range matches the query, false if not.
 */
FLECS_API
bool ecs_query_has_range(
    const ecs_query_t *query,
    ecs_table_range_t *range,
    ecs_iter_t *it);

/** Return how often a match event happened for a cached query.
 * This operation can be used to determine whether the query cache has been 
 * updated with new tables.
 * 
 * @param query The query.
 * @return The number of match events that happened.
 */
FLECS_API
int32_t ecs_query_match_count(
    const ecs_query_t *query);

/** Convert a query to a string.
 * This will convert the query program to a string, which can aid in debugging
 * the behavior of a query.
 *
 * The returned string must be freed with ecs_os_free().
 *
 * @param query The query.
 * @return The query plan.
 */
FLECS_API
char* ecs_query_plan(
    const ecs_query_t *query);

/** Convert a query to a string with a profile.
 * To use this, you must set the EcsIterProfile flag on an iterator before
 * starting iteration:
 *
 * @code
 *   it.flags |= EcsIterProfile;
 * @endcode
 * 
 * The returned string must be freed with ecs_os_free().
 *
 * @param query The query.
 * @param it The iterator with profile data.
 * @return The query plan with profile data.
 */
FLECS_API
char* ecs_query_plan_w_profile(
    const ecs_query_t *query,
    const ecs_iter_t *it);

/** Same as ecs_query_plan(), but includes the plan for populating the cache (if any). 
 * 
 * @param query The query.
 * @return The query plan.
 */
FLECS_API
char* ecs_query_plans(
    const ecs_query_t *query);

/** Populate variables from a key-value string.
 * Convenience function to set query variables from a key-value string separated
 * by commas. The string must have the following format:
 *
 * @code
 *   var_a: value, var_b: value
 * @endcode
 *
 * The key-value list may optionally be enclosed in parentheses.
 * 
 * This function uses the script addon.
 *
 * @param query The query.
 * @param it The iterator for which to set the variables.
 * @param expr The key-value expression.
 * @return A pointer to the next character after the last parsed one.
 */
FLECS_API
const char* ecs_query_args_parse(
    ecs_query_t *query,
    ecs_iter_t *it,
    const char *expr);

/** Return whether the query data changed since the last iteration.
 * The operation will return true after:
 * - new entities have been matched
 * - new tables have been matched or unmatched
 * - matched entities were deleted
 * - matched components were changed
 *
 * The operation will not return true after a write-only (EcsOut) or filter
 * (EcsInOutFilter) term has changed, when a term is not matched with the
 * current table ($this source) or for tag terms.
 *
 * The changed state of a table is reset after it is iterated. If an iterator was
 * not iterated until completion, tables may still be marked as changed.
 *
 * To check the changed state of the current iterator result, use
 * ecs_iter_changed().
 *
 * @param query The query.
 * @return True if entities changed, otherwise false.
 *
 * @see ecs_iter_changed()
 */
FLECS_API
bool ecs_query_changed(
    ecs_query_t *query);

/** Get the query object.
 * Return the query object. Can be used to access various information about
 * the query.
 *
 * @param world The world.
 * @param query The query.
 * @return The query object.
 */
FLECS_API
const ecs_query_t* ecs_query_get(
    const ecs_world_t *world,
    ecs_entity_t query);

/** Skip a table while iterating.
 * This operation lets the query iterator know that a table was skipped while
 * iterating. A skipped table will not reset its changed state, and the query
 * will not update the dirty flags of the table for its out fields.
 *
 * Only valid iterators must be provided (next() has to be called at least once
 * and must return true), and the iterator must be a query iterator.
 *
 * @param it The iterator result to skip.
 */
FLECS_API
void ecs_iter_skip(
    ecs_iter_t *it);

/** Set the group to iterate for a query iterator.
 * This operation limits the results returned by the query to only the selected
 * group ID. The query must have a group_by function, and the iterator must
 * be a query iterator.
 *
 * Groups are sets of tables that are stored together in the query cache based
 * on a group ID, which is calculated per table by the group_by function. To
 * iterate a group, an iterator only needs to know the first and last cache node
 * for that group, which can both be found in a fast O(1) operation.
 *
 * As a result, group iteration is one of the most efficient mechanisms to
 * filter out large numbers of entities, even if those entities are distributed
 * across many tables. This makes it a good fit for things like dividing up
 * a world into cells, and only iterating cells close to a player.
 *
 * The group to iterate must be set before the first call to ecs_query_next(). No
 * operations that can add or remove components should be invoked between calling
 * ecs_iter_set_group() and ecs_query_next().
 *
 * @param it The query iterator.
 * @param group_id The group to iterate.
 */
FLECS_API
void ecs_iter_set_group(
    ecs_iter_t *it,
    uint64_t group_id);

/** Return the map with query groups.
 * This map can be used to iterate the active group identifiers of a query. The
 * payload of the map is opaque. The map can be used as follows:
 * 
 * @code
 * const ecs_map_t *keys = ecs_query_get_groups(q);
 * ecs_map_iter_t kit = ecs_map_iter(keys);
 * while (ecs_map_next(&kit)) {
 *   uint64_t group_id = ecs_map_key(&kit);
 * 
 *   // Iterate query for group
 *   ecs_iter_t it = ecs_query_iter(world, q);
 *   ecs_iter_set_group(&it, group_id);
 *   while (ecs_query_next(&it)) {
 *     // Iterate as usual
 *   }
 * }
 * @endcode
 * 
 * This operation is not valid for queries that do not use group_by. The 
 * returned map pointer will remain valid for as long as the query exists.
 *
 * @param query The query.
 * @return The map with query groups.
 */
FLECS_API
const ecs_map_t* ecs_query_get_groups(
    const ecs_query_t *query);

/** Get the context of a query group.
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

/** Get information about a query group.
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

/** Struct returned by ecs_query_count(). */
typedef struct ecs_query_count_t {
    int32_t results;      /**< Number of results returned by the query. */
    int32_t entities;     /**< Number of entities returned by the query. */
    int32_t tables;       /**< Number of tables returned by the query. Only set for
                             * queries for which the table count can be reliably
                             * determined. */
} ecs_query_count_t;

/** Return the number of entities and results the query matches with.
 * Only entities matching the $this variable as source are counted.
 *
 * @param query The query.
 * @return The number of matched entities.
 */
FLECS_API
ecs_query_count_t ecs_query_count(
    const ecs_query_t *query);

/** Test whether a query returns one or more results.
 * 
 * @param query The query.
 * @return True if query matches anything, false if not.
 */
FLECS_API
bool ecs_query_is_true(
    const ecs_query_t *query);

/** Get the query used to populate the cache.
 * This operation returns the query that is used to populate the query cache.
 * For queries that can be entirely cached, the returned query will be
 * equivalent to the query passed to ecs_query_init().
 *
 * @param query The query.
 * @return The query used to populate the cache, NULL if query is not cached.
 */
FLECS_API
const ecs_query_t* ecs_query_get_cache_query(
    const ecs_query_t *query);

/** @} */

/**
 * @defgroup observers Observers
 * Functions for working with events and observers.
 *
 * @{
 */

/** Send an event.
 * This sends an event to matching observers and is the mechanism used by Flecs
 * itself to send `OnAdd`, `OnRemove`, etc. events.
 *
 * Applications can use this function to send custom events, where a custom
 * event can be any regular entity.
 *
 * Applications should not send built-in Flecs events, as this may violate
 * assumptions the code makes about the conditions under which those events are
 * sent.
 *
 * Observers are invoked synchronously. It is therefore safe to use stack-based
 * data as event context, which can be set in the "param" member.
 *
 * @param world The world.
 * @param desc The event parameters.
 *
 * @see ecs_enqueue()
 */
FLECS_API
void ecs_emit(
    ecs_world_t *world,
    ecs_event_desc_t *desc);

/** Enqueue an event.
 * Same as ecs_emit(), but enqueues an event in the command queue instead. The
 * event will be emitted when ecs_defer_end() is called.
 * 
 * If this operation is called when the provided world is not in deferred mode,
 * it behaves just like ecs_emit().
 * 
 * @param world The world.
 * @param desc The event parameters.
 */
FLECS_API
void ecs_enqueue(
    ecs_world_t *world,
    ecs_event_desc_t *desc);

/** Create an observer.
 * Observers can subscribe for one or more terms. An observer only triggers
 * when the source of the event meets all terms.
 *
 * If the descriptor specifies an existing entity, the entity must not already
 * be associated with an observer. To modify an existing observer, use
 * ecs_observer_update().
 *
 * See the documentation for ecs_observer_desc_t for more details.
 *
 * @param world The world.
 * @param desc The observer creation parameters.
 * @return The observer, or 0 if the operation failed.
 */
FLECS_API
ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc);

/** Update an existing observer.
 * Updates the configuration of an observer that was previously created with
 * ecs_observer_init(). Only fields in desc that are set to a non-default
 * value will be applied; fields left at their default value preserve the
 * existing configuration of the observer.
 *
 * The query and events fields of the descriptor are not used by this function;
 * the observer query and event subscriptions cannot be modified after
 * creation.
 *
 * @param world The world.
 * @param observer The observer to update.
 * @param desc The observer descriptor.
 * @return The observer entity, or 0 if the operation failed.
 */
FLECS_API
ecs_entity_t ecs_observer_update(
    ecs_world_t *world,
    ecs_entity_t observer,
    const ecs_observer_desc_t *desc);

/** Get the observer object.
 * Return the observer object. Can be used to access various information about
 * the observer, like the query and context.
 *
 * @param world The world.
 * @param observer The observer.
 * @return The observer object.
 */
FLECS_API
const ecs_observer_t* ecs_observer_get(
    const ecs_world_t *world,
    ecs_entity_t observer);

/** @} */

/**
 * @defgroup iterator Iterators
 * Functions for working with `ecs_iter_t`.
 *
 * @{
 */

/** Progress any iterator.
 * This operation is useful in combination with iterators for which it is not
 * known what created them. Example use cases are functions that should accept
 * any kind of iterator (such as serializers) or iterators created from poly
 * objects.
 *
 * This operation is slightly slower than using a type-specific iterator (e.g.,
 * ecs_query_next(), ecs_each_next()), as it has to call a function pointer, which
 * introduces a level of indirection.
 *
 * @param it The iterator.
 * @return True if iterator has more results, false if not.
 */
FLECS_API
bool ecs_iter_next(
    ecs_iter_t *it);

/** Clean up iterator resources.
 * This operation cleans up any resources associated with the iterator.
 *
 * This operation should only be used when an iterator is not iterated until
 * completion (next() has not yet returned false). When an iterator is iterated
 * until completion, resources are automatically freed.
 *
 * @param it The iterator.
 */
FLECS_API
void ecs_iter_fini(
    ecs_iter_t *it);

/** Count the number of matched entities in a query.
 * This operation returns the number of matched entities. If a query contains no
 * matched entities but still yields results (e.g., it has no terms with $this
 * sources), the operation will return 0.
 *
 * To determine the number of matched entities, the operation iterates the
 * iterator until it yields no more results.
 *
 * @param it The iterator.
 * @return The number of matched entities.
 */
FLECS_API
int32_t ecs_iter_count(
    ecs_iter_t *it);

/** Test if an iterator is true.
 * This operation will return true if the iterator returns at least one result.
 * This is especially useful in combination with fact-checking queries (see the
 * queries addon).
 *
 * The operation requires a valid iterator. After the operation is invoked, the
 * application should no longer invoke next() on the iterator and should treat it
 * as if the iterator is iterated until completion.
 *
 * @param it The iterator.
 * @return True if the iterator returns at least one result.
 */
FLECS_API
bool ecs_iter_is_true(
    ecs_iter_t *it);

/** Get the first matching entity from an iterator.
 * After this operation, the application should treat the iterator as if it has
 * been iterated until completion.
 *
 * @param it The iterator.
 * @return The first matching entity, or 0 if no entities were matched.
 */
FLECS_API
ecs_entity_t ecs_iter_first(
    ecs_iter_t *it);

/** Set the value for an iterator variable.
 * This constrains the iterator to return only results for which the variable
 * equals the specified value. The default value for all variables is
 * EcsWildcard, which means the variable can assume any value.
 *
 * Example:
 *
 * @code
 * // Query that matches (Eats, *)
 * ecs_query_t *q = ecs_query(world, {
 *   .terms = {
 *     { .first.id = Eats, .second.name = "$food" }
 *   }
 * });
 * 
 * int food_var = ecs_query_find_var(q, "food");
 * 
 * // Set Food to Apples, so we're only matching (Eats, Apples)
 * ecs_iter_t it = ecs_query_iter(world, q);
 * ecs_iter_set_var(&it, food_var, Apples);
 * 
 * while (ecs_query_next(&it)) {
 *   for (int i = 0; i < it.count; i ++) {
 *     // iterate as usual
 *   }
 * }
 * @endcode
 *
 * The variable must be initialized after creating the iterator and before the
 * first call to next().
 *
 * @param it The iterator.
 * @param var_id The variable index.
 * @param entity The entity variable value.
 *
 * @see ecs_iter_set_var_as_range()
 * @see ecs_iter_set_var_as_table()
 */
FLECS_API
void ecs_iter_set_var(
    ecs_iter_t *it,
    int32_t var_id,
    ecs_entity_t entity);

/** Same as ecs_iter_set_var(), but for a table.
 * This constrains the variable to all entities in a table.
 *
 * @param it The iterator.
 * @param var_id The variable index.
 * @param table The table variable value.
 *
 * @see ecs_iter_set_var()
 * @see ecs_iter_set_var_as_range()
 */
FLECS_API
void ecs_iter_set_var_as_table(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_t *table);

/** Same as ecs_iter_set_var(), but for a range of entities.
 * This constrains the variable to a range of entities in a table.
 *
 * @param it The iterator.
 * @param var_id The variable index.
 * @param range The range variable value.
 *
 * @see ecs_iter_set_var()
 * @see ecs_iter_set_var_as_table()
 */
FLECS_API
void ecs_iter_set_var_as_range(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_range_t *range);

/** Get the value of an iterator variable as an entity.
 * A variable can be interpreted as an entity if it is set to an entity, or if it
 * is set to a table range with count 1.
 *
 * This operation can only be invoked on valid iterators. The variable index
 * must be smaller than the total number of variables provided by the iterator
 * (as returned by ecs_iter_get_var_count()).
 *
 * @param it The iterator.
 * @param var_id The variable index.
 * @return The variable value.
 */
FLECS_API
ecs_entity_t ecs_iter_get_var(
    ecs_iter_t *it,
    int32_t var_id);

/** Get the variable name.
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 * @return The variable name.
 */
FLECS_API
const char* ecs_iter_get_var_name(
    const ecs_iter_t *it,
    int32_t var_id);

/** Get the number of variables. 
 * 
 * @param it The iterator.
 * @return The number of variables.
 */
FLECS_API
int32_t ecs_iter_get_var_count(
    const ecs_iter_t *it);

/** Get the variable array.
 * 
 * @param it The iterator.
 * @return The variable array (if any).
 */
FLECS_API
ecs_var_t* ecs_iter_get_vars(
    const ecs_iter_t *it);

/** Get the value of an iterator variable as a table.
 * A variable can be interpreted as a table if it is set as a table range with
 * both offset and count set to 0, or if offset is 0 and count matches the
 * number of elements in the table.
 *
 * This operation can only be invoked on valid iterators. The variable index
 * must be smaller than the total number of variables provided by the iterator
 * (as returned by ecs_iter_get_var_count()).
 *
 * @param it The iterator.
 * @param var_id The variable index.
 * @return The variable value.
 */
FLECS_API
ecs_table_t* ecs_iter_get_var_as_table(
    ecs_iter_t *it,
    int32_t var_id);

/** Get the value of an iterator variable as a table range.
 * A value can be interpreted as a table range if it is set as a table range, or if
 * it is set to an entity with a non-empty type (the entity must have at least
 * one component, tag, or relationship in its type).
 *
 * This operation can only be invoked on valid iterators. The variable index
 * must be smaller than the total number of variables provided by the iterator
 * (as returned by ecs_iter_get_var_count()).
 *
 * @param it The iterator.
 * @param var_id The variable index.
 * @return The variable value.
 */
FLECS_API
ecs_table_range_t ecs_iter_get_var_as_range(
    ecs_iter_t *it,
    int32_t var_id);

/** Return whether a variable is constrained.
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

/** Return the group ID for the currently iterated result.
 * This operation returns the group ID for queries that use group_by. If this
 * operation is called on an iterator that is not iterating a query that uses
 * group_by, it will fail.
 * 
 * For queries that use cascade, this operation will return the hierarchy depth
 * of the currently iterated result.
 * 
 * @param it The iterator.
 * @return The group ID of the currently iterated result.
 */
FLECS_API
uint64_t ecs_iter_get_group(
    const ecs_iter_t *it);

/** Return whether the current iterator result has changed.
 * This operation must be used in combination with a query that supports change
 * detection (e.g., is cached). The operation returns whether the currently
 * iterated result has changed since the last time it was iterated by the query.
 * 
 * Change detection works on a per-table basis. Changes to individual entities
 * cannot be detected this way.
 * 
 * @param it The iterator.
 * @return True if the result changed, false if it didn't.
 */
FLECS_API
bool ecs_iter_changed(
    ecs_iter_t *it);

/** Convert an iterator to a string.
 * Prints the contents of an iterator to a string. Useful for debugging and/or
 * testing the output of an iterator.
 *
 * The function only converts the currently iterated data to a string. To
 * convert all data, the application has to manually call the next function and
 * call ecs_iter_str() on each result.
 *
 * @param it The iterator.
 * @return A string representing the contents of the iterator.
 */
FLECS_API
char* ecs_iter_str(
    const ecs_iter_t *it);

/** Create a paged iterator.
 * Paged iterators limit the results to those starting from 'offset', and will
 * return at most 'limit' results.
 *
 * The iterator must be iterated with ecs_page_next().
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
 * Progress an iterator created by ecs_page_iter().
 *
 * @param it The iterator.
 * @return True if the iterator has more results, false if not.
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
 * The iterator must be iterated with ecs_worker_next().
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
 * Progress an iterator created by ecs_worker_iter().
 *
 * @param it The iterator.
 * @return True if the iterator has more results, false if not.
 */
FLECS_API
bool ecs_worker_next(
    ecs_iter_t *it);

/** Get data for a field.
 * This operation retrieves a pointer to an array of data that belongs to the
 * term in the query. The index refers to the location of the term in the query,
 * and starts counting from zero.
 *
 * For example, the query `"Position, Velocity"` will return the `Position` array
 * for index 0, and the `Velocity` array for index 1.
 *
 * When the specified field is not owned by the entity, this function returns a
 * pointer instead of an array. This happens when the source of a field is not
 * the entity being iterated, such as a shared component (from a prefab), a
 * component from a parent, or another entity. The ecs_field_is_self() operation
 * can be used to test dynamically if a field is owned.
 * 
 * When a field contains a sparse component, use the ecs_field_at() function. When
 * a field is guaranteed to be set and owned, the ecs_field_self() function can be
 * used. ecs_field_self() has slightly better performance, and provides stricter 
 * validity checking.
 *
 * The provided size must be either 0 or must match the size of the type
 * of the returned array. If the size does not match, the operation may assert.
 * The size can be dynamically obtained with ecs_field_size().
 * 
 * An example:
 * 
 * @code
 * while (ecs_query_next(&it)) {
 *   Position *p = ecs_field(&it, Position, 0);
 *   Velocity *v = ecs_field(&it, Velocity, 1);
 *   for (int32_t i = 0; i < it.count; i ++) {
 *     p[i].x += v[i].x;
 *     p[i].y += v[i].y;
 *   }
 * }
 * @endcode
 *
 * @param it The iterator.
 * @param size The size of the field type.
 * @param index The index of the field.
 * @return A pointer to the data of the field.
 */
FLECS_API
void* ecs_field_w_size(
    const ecs_iter_t *it,
    size_t size,
    int8_t index);

/** Get data for a field at a specified row.
 * This operation should be used instead of ecs_field_w_size() for sparse 
 * component fields. This operation should be called for each returned row in a
 * result. In the following example, the Velocity component is sparse:
 * 
 * @code
 * while (ecs_query_next(&it)) {
 *   Position *p = ecs_field(&it, Position, 0);
 *   for (int32_t i = 0; i < it.count; i ++) {
 *     Velocity *v = ecs_field_at(&it, Velocity, 1, i);
 *     p[i].x += v->x;
 *     p[i].y += v->y;
 *   }
 * }
 * @endcode
 * 
 * @param it The iterator.
 * @param size The size of the field type.
 * @param index The index of the field.
 * @param row The row to get data for.
 * @return A pointer to the data of the field.
 */
FLECS_API
void* ecs_field_at_w_size(
    const ecs_iter_t *it,
    size_t size,
    int8_t index,
    int32_t row);

/** Test whether the field is read-only.
 * This operation returns whether the field is read-only. Read-only fields are
 * annotated with [in], or are added as a const type in the C++ API.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is read-only.
 */
FLECS_API
bool ecs_field_is_readonly(
    const ecs_iter_t *it,
    int8_t index);

/** Test whether the field is write-only.
 * This operation returns whether this is a write-only field. Write-only terms are
 * annotated with [out].
 *
 * Serializers are not required to serialize the values of a write-only field.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is write-only.
 */
FLECS_API
bool ecs_field_is_writeonly(
    const ecs_iter_t *it,
    int8_t index);

/** Test whether a field is set.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is set.
 */
FLECS_API
bool ecs_field_is_set(
    const ecs_iter_t *it,
    int8_t index);

/** Return the ID matched for a field.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The ID matched for the field.
 */
FLECS_API
ecs_id_t ecs_field_id(
    const ecs_iter_t *it,
    int8_t index);

/** Return the index of a matched table column.
 * This function only returns column indices for fields that have been matched
 * on the $this variable. Fields matched on other tables will return -1.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The index of the matched column, -1 if not matched.
 */
FLECS_API
int32_t ecs_field_column(
    const ecs_iter_t *it,
    int8_t index);

/** Return the field source.
 * The field source is the entity on which the field was matched.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The source for the field.
 */
FLECS_API
ecs_entity_t ecs_field_src(
    const ecs_iter_t *it,
    int8_t index);

/** Return the field type size.
 * Returns the type size of the field. Returns 0 if the field has no data.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The type size for the field.
 */
FLECS_API
size_t ecs_field_size(
    const ecs_iter_t *it,
    int8_t index);

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
    int8_t index);

/** @} */

/**
 * @defgroup tables Tables
 * Functions for working with `ecs_table_t`.
 *
 * @{
 */

/** Get the type for a table.
 * The table type is a vector that contains all component, tag, and pair IDs.
 *
 * @param table The table.
 * @return The type of the table.
 */
FLECS_API
const ecs_type_t* ecs_table_get_type(
    const ecs_table_t *table);

/** Get the type index for a component.
 * This operation returns the index for a component in the table's type.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component.
 * @return The index of the component in the table type, or -1 if not found.
 *
 * @see ecs_table_has_id()
 */
FLECS_API
int32_t ecs_table_get_type_index(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component);

/** Get the column index for a component.
 * This operation returns the column index for a component in the table's type. 
 * If the component doesn't have data (it is a tag), the function will return -1.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component.
 * @return The column index of the component ID, or -1 if not found or not a component.
 */
FLECS_API
int32_t ecs_table_get_column_index(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component);

/** Return the number of columns in a table.
 * Similar to `ecs_table_get_type(table)->count`, except that the column count
 * only counts the number of components in a table.
 *
 * @param table The table.
 * @return The number of columns in the table.
 */
FLECS_API
int32_t ecs_table_column_count(
    const ecs_table_t *table);

/** Convert a type index to a column index.
 * Tables have an array of columns for each component in the table. This array
 * does not include elements for tags, which means that the index for a
 * component in the table type is not necessarily the same as the index in the
 * column array. This operation converts from an index in the table type to an
 * index in the column array.
 *
 * @param table The table.
 * @param index The index in the table type.
 * @return The index in the table column array.
 *
 * @see ecs_table_column_to_type_index()
 */
FLECS_API
int32_t ecs_table_type_to_column_index(
    const ecs_table_t *table,
    int32_t index);

/** Convert a column index to a type index.
 * Same as ecs_table_type_to_column_index(), but converts from an index in the
 * column array to an index in the table type.
 *
 * @param table The table.
 * @param index The column index.
 * @return The index in the table type.
 */
FLECS_API
int32_t ecs_table_column_to_type_index(
    const ecs_table_t *table,
    int32_t index);

/** Get a column from a table by column index.
 * This operation returns the component array for the provided index.
 *
 * @param table The table.
 * @param index The column index.
 * @param offset The index of the first row to return (0 for entire column).
 * @return The component array, or NULL if the index is not a component.
 */
FLECS_API
void* ecs_table_get_column(
    const ecs_table_t *table,
    int32_t index,
    int32_t offset);

/** Get a column from a table by component.
 * This operation returns the component array for the provided component.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component for the column.
 * @param offset The index of the first row to return (0 for entire column).
 * @return The component array, or NULL if the component is not found.
 */
FLECS_API
void* ecs_table_get_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component,
    int32_t offset);

/** Get the column size from a table.
 * This operation returns the component size for the provided index.
 *
 * @param table The table.
 * @param index The column index.
 * @return The component size, or 0 if the index is not a component.
 */
FLECS_API
size_t ecs_table_get_column_size(
    const ecs_table_t *table,
    int32_t index);

/** Return the number of entities in the table.
 * This operation returns the number of entities in the table.
 *
 * @param table The table.
 * @return The number of entities in the table.
 */
FLECS_API
int32_t ecs_table_count(
    const ecs_table_t *table);

/** Return the allocated size of the table.
 * This operation returns the number of elements allocated in the table 
 * per column.
 * 
 * @param table The table.
 * @return The number of allocated elements in the table.
 */
FLECS_API
int32_t ecs_table_size(
    const ecs_table_t *table);

/** Return the array with entity IDs for the table.
 * The size of the returned array is the result of ecs_table_count().
 * 
 * @param table The table.
 * @return The array with entity IDs for the table.
 */
FLECS_API
const ecs_entity_t* ecs_table_entities(
    const ecs_table_t *table);

/** Test if a table has a component.
 * Same as `ecs_table_get_type_index(world, table, component) != -1`.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component.
 * @return True if the table has the component ID, false if the table doesn't.
 *
 * @see ecs_table_get_type_index()
 */
FLECS_API
bool ecs_table_has_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component);

/** Get the relationship target for a table.
 * 
 * @param world The world.
 * @param table The table.
 * @param relationship The relationship for which to obtain the target.
 * @param index The index, in case the table has multiple instances of the relationship.
 * @return The requested relationship target.
 * 
 * @see ecs_get_target()
 */
FLECS_API
ecs_entity_t ecs_table_get_target(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_entity_t relationship,
    int32_t index);

/** Return the depth for a table in the tree for the specified relationship.
 * Depth is determined by counting the number of targets encountered while
 * traversing up the relationship tree. Only acyclic relationships are
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

/** Get the table that has all components of the current table plus the specified ID.
 * If the provided table already has the provided ID, the operation will return
 * the provided table.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component to add.
 * @return The resulting table.
 */
FLECS_API
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component);

/** Find a table from an ID array.
 * This operation finds or creates a table with the specified array of
 * (component) IDs. The IDs in the array must be sorted, and it may not contain
 * duplicate elements.
 *
 * @param world The world.
 * @param ids The ID array.
 * @param id_count The number of elements in the ID array.
 * @return The table with the specified (component) IDs.
 */
FLECS_API
ecs_table_t* ecs_table_find(
    ecs_world_t *world,
    const ecs_id_t *ids,
    int32_t id_count);

/** Get the table that has all components of the current table minus the specified component.
 * If the provided table doesn't have the provided component, the operation will 
 * return the provided table.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component to remove.
 * @return The resulting table.
 */
FLECS_API
ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component);

/** Lock a table.
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
 * Must be called after calling ecs_table_lock().
 *
 * @param world The world.
 * @param table The table to unlock.
 */
FLECS_API
void ecs_table_unlock(
    ecs_world_t *world,
    ecs_table_t *table);

/** Test a table for flags.
 * Test if a table has all of the provided flags. See
 * include/flecs/private/api_flags.h for a list of table flags that can be used
 * with this function.
 *
 * @param table The table.
 * @param flags The flags to test for.
 * @return Whether the specified flags are set for the table.
 */
FLECS_API
bool ecs_table_has_flags(
    ecs_table_t *table,
    ecs_flags32_t flags);

/** Check if a table has traversable entities.
 * Traversable entities are entities that are used as a target in a pair with a
 * relationship that has the Traversable trait.
 * 
 * @param table The table.
 * @return Whether the table has traversable entities.
 */
FLECS_API
bool ecs_table_has_traversable(
    const ecs_table_t *table);

/** Swap two elements inside the table.
 * This is useful for implementing custom
 * table sorting algorithms.
 *
 * @param world The world.
 * @param table The table to swap elements in.
 * @param row_1 The table element to swap with row_2.
 * @param row_2 The table element to swap with row_1.
 */
FLECS_API
void ecs_table_swap_rows(
    ecs_world_t* world,
    ecs_table_t* table,
    int32_t row_1,
    int32_t row_2);

/** Commit (move) an entity to a table.
 * This operation moves an entity from its current table to the specified
 * table. This may cause the following actions:
 * - Ctor for each component in the target table.
 * - Move for each overlapping component.
 * - Dtor for each component in the source table.
 * - `OnAdd` observers for non-overlapping components in the target table.
 * - `OnRemove` observers for non-overlapping components in the source table.
 *
 * This operation is faster than adding or removing components individually.
 *
 * The application must explicitly provide the difference in components between
 * tables as the added and removed parameters. This can usually be derived directly
 * from the result of ecs_table_add_id() and ecs_table_remove_id(). These arrays are
 * required to properly execute `OnAdd` and `OnRemove` observers.
 *
 * @param world The world.
 * @param entity The entity to commit.
 * @param record The entity's record (optional, providing it saves a lookup).
 * @param table The table to commit the entity to.
 * @param added The components added to the entity.
 * @param removed The components removed from the entity.
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

/** Search for a component in a table type.
 * This operation returns the index of the first occurrence of the component in the
 * table type. The component may be a pair or a wildcard.
 *
 * When component_out is provided, the function will assign it with the found 
 * component. The found component may be different from the provided component 
 * if it is a wildcard.
 *
 * This is a constant-time operation.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component to search for.
 * @param component_out If provided, it will be set to the found component (optional).
 * @return The index of the ID in the table type.
 *
 * @see ecs_search_offset()
 * @see ecs_search_relation()
 */
FLECS_API
int32_t ecs_search(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component,
    ecs_id_t *component_out);

/** Search for a component in a table type starting from an offset.
 * This operation is the same as ecs_search(), but starts searching from an offset
 * in the table type.
 *
 * This operation is typically called in a loop where the resulting index is
 * used in the next iteration as offset:
 *
 * @code
 * int32_t index = -1;
 * while ((index = ecs_search_offset(world, table, index + 1, id, NULL)) != -1) {
 *   // do stuff
 * }
 * @endcode
 *
 * Depending on how the operation is used, it is either linear or constant time.
 * When the ID has the form `(id)` or `(rel, *)` and the operation is invoked as
 * in the above example, it is guaranteed to be constant time.
 *
 * If the provided component has the form `(*, tgt)`, the operation takes linear
 * time. The reason for this is that IDs for a target are not packed together,
 * as they are sorted relationship-first.
 *
 * If the component at the offset does not match the provided ID, the operation
 * will do a linear search to find a matching ID.
 *
 * @param world The world.
 * @param table The table.
 * @param offset The offset from where to start searching.
 * @param component The component to search for.
 * @param component_out If provided, it will be set to the found component (optional).
 * @return The index of the ID in the table type.
 *
 * @see ecs_search()
 * @see ecs_search_relation()
 */
FLECS_API
int32_t ecs_search_offset(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t component,
    ecs_id_t *component_out);

/** Search for a component or relationship ID in a table type starting from an offset.
 * This operation is the same as ecs_search_offset(), but has the additional
 * capability of traversing relationships to find a component. For example, if
 * an application wants to find a component for either the provided table or a
 * prefab (using the `IsA` relationship) of that table, it could use the operation
 * like this:
 *
 * @code
 * int32_t index = ecs_search_relation(
 *   world,            // the world
 *   table,            // the table
 *   0,                // offset 0
 *   ecs_id(Position), // the component ID
 *   EcsIsA,           // the relationship to traverse
 *   EcsSelf|EcsUp,    // search self and up
 *   NULL,             // (optional) entity on which component was found
 *   NULL,             // (optional) found component ID
 *   NULL);            // internal type with information about matched ID
 * @endcode
 *
 * The operation searches depth-first. If a table type has 2 `IsA` relationships, the
 * operation will first search the `IsA` tree of the first relationship.
 *
 * When choosing between ecs_search(), ecs_search_offset(), and ecs_search_relation(),
 * the simpler the function, the better its performance.
 *
 * @param world The world.
 * @param table The table.
 * @param offset The offset from where to start searching.
 * @param component The component to search for.
 * @param rel The relationship to traverse (optional).
 * @param flags Whether to search EcsSelf and/or EcsUp.
 * @param tgt_out If provided, it will be set to the matched entity.
 * @param component_out If provided, it will be set to the found component (optional).
 * @param tr_out The internal datatype.
 * @return The index of the component in the table type.
 *
 * @see ecs_search()
 * @see ecs_search_offset()
 */
FLECS_API
int32_t ecs_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t component,
    ecs_entity_t rel,
    ecs_flags64_t flags, /* EcsSelf and/or EcsUp */
    ecs_entity_t *tgt_out,
    ecs_id_t *component_out,
    struct ecs_table_record_t **tr_out);

/** Search for a component ID by following a relationship, starting from an entity.
 * This operation is the same as ecs_search_relation(), but starts the search
 * from an entity rather than a table.
 *
 * @param world The world.
 * @param entity The entity from which to begin the search.
 * @param id The component ID to search for.
 * @param rel The relationship to follow.
 * @param self If true, also search components on the entity itself.
 * @param cr Optional component record for the component ID.
 * @param tgt_out Out parameter for the target entity.
 * @param id_out Out parameter for the found component ID.
 * @param tr_out Out parameter for the table record.
 * @return The index of the component ID in the entity's type, or -1 if not found.
 */
FLECS_API
int32_t ecs_search_relation_for_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_entity_t rel,
    bool self,
    ecs_component_record_t *cr,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out);

/** Remove all entities in a table. Does not deallocate table memory. 
 * Retaining table memory can be efficient when planning 
 * to refill the table with operations like ecs_bulk_init().
 *
 * @param world The world.
 * @param table The table to clear.
 */
FLECS_API
void ecs_table_clear_entities(
    ecs_world_t* world,
    ecs_table_t* table);

/** @} */

/**
 * @defgroup values Values
 * Construct, destruct, copy, and move dynamically created values.
 *
 * @{
 */

/** Construct a value in existing storage.
 *
 * @param world The world.
 * @param type The type of the value to create.
 * @param ptr A pointer to a value of type 'type'.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_init(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr);

/** Construct a value in existing storage.
 *
 * @param world The world.
 * @param ti The type info of the type to create.
 * @param ptr A pointer to a value of type 'type'.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_init_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr);

/** Construct a value in new storage.
 *
 * @param world The world.
 * @param type The type of the value to create.
 * @return A pointer to the value if successful, NULL if failed.
 */
FLECS_API
void* ecs_value_new(
    ecs_world_t *world,
    ecs_entity_t type);

/** Construct a value in new storage.
 *
 * @param world The world.
 * @param ti The type info of the type to create.
 * @return A pointer to the value if successful, NULL if failed.
 */
void* ecs_value_new_w_type_info(
    ecs_world_t *world,
    const ecs_type_info_t *ti);

/** Destruct a value.
 *
 * @param world The world.
 * @param ti The type info of the value to destruct.
 * @param ptr A pointer to a constructed value of type 'type'.
 * @return Zero if successful, nonzero if failed.
 */
int ecs_value_fini_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr);

/** Destruct a value.
 *
 * @param world The world.
 * @param type The type of the value to destruct.
 * @param ptr A pointer to a constructed value of type 'type'.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_fini(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* ptr);

/** Destruct a value and free storage.
 *
 * @param world The world.
 * @param type The type of the value to destruct.
 * @param ptr A pointer to the value.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_free(
    ecs_world_t *world,
    ecs_entity_t type,
    void* ptr);

/** Copy a value.
 *
 * @param world The world.
 * @param ti The type info of the value to copy.
 * @param dst A pointer to the storage to copy to.
 * @param src A pointer to the value to copy.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_copy_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    const void *src);

/** Copy a value.
 *
 * @param world The world.
 * @param type The type of the value to copy.
 * @param dst A pointer to the storage to copy to.
 * @param src A pointer to the value to copy.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_copy(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    const void *src);

/** Move a value.
 *
 * @param world The world.
 * @param ti The type info of the value to move.
 * @param dst A pointer to the storage to move to.
 * @param src A pointer to the value to move.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_move_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src);

/** Move a value.
 *
 * @param world The world.
 * @param type The type of the value to move.
 * @param dst A pointer to the storage to move to.
 * @param src A pointer to the value to move.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_move(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src);

/** Move-construct a value.
 *
 * @param world The world.
 * @param ti The type info of the value to move.
 * @param dst A pointer to the storage to move to.
 * @param src A pointer to the value to move.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_move_ctor_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src);

/** Move-construct a value.
 *
 * @param world The world.
 * @param type The type of the value to move.
 * @param dst A pointer to the storage to move to.
 * @param src A pointer to the value to move.
 * @return Zero if successful, nonzero if failed.
 */
FLECS_API
int ecs_value_move_ctor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src);

/** @} */

/** @} */

/**
 * @defgroup c_addons Addons
 * @ingroup c
 * C APIs for addons.
 *
 * @{
 * @}
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup flecs_c Macro API
 * @ingroup c
 * Convenience macros for C API.
 *
 * @{
 */

/**
 * @defgroup flecs_c_creation Creation macros
 * Convenience macros for creating entities, components, and observers.
 *
 * @{
 */

/** Forward declare an entity, tag, prefab, or any other entity identifier. */
#define ECS_DECLARE(id)\
    ecs_entity_t id, ecs_id(id)

/** Forward declare an entity. */
#define ECS_ENTITY_DECLARE ECS_DECLARE

/** Define a forward-declared entity.
 *
 * Example:
 *
 * @code
 * ECS_ENTITY_DEFINE(world, MyEntity, Position, Velocity);
 * @endcode
 */
#define ECS_ENTITY_DEFINE(world, id_, ...) \
    { \
        ecs_entity_desc_t desc = {0}; \
        desc.id = id_; \
        desc.name = #id_; \
        desc.add_expr = #__VA_ARGS__; \
        id_ = ecs_entity_init(world, &desc); \
        ecs_id(id_) = id_; \
        ecs_assert(id_ != 0, ECS_INVALID_PARAMETER, "failed to create entity %s", #id_); \
    } \
    (void)id_; \
    (void)ecs_id(id_)

/** Declare and define an entity.
 *
 * Example:
 *
 * @code
 * ECS_ENTITY(world, MyEntity, Position, Velocity);
 * @endcode
 */
#define ECS_ENTITY(world, id, ...) \
    ecs_entity_t ecs_id(id); \
    ecs_entity_t id = 0; \
    ECS_ENTITY_DEFINE(world, id, __VA_ARGS__)

/** Forward declare a tag. */
#define ECS_TAG_DECLARE ECS_DECLARE

/** Define a forward-declared tag.
 *
 * Example:
 *
 * @code
 * ECS_TAG_DEFINE(world, MyTag);
 * @endcode
 */
#define ECS_TAG_DEFINE(world, id) ECS_ENTITY_DEFINE(world, id, 0)

/** Declare and define a tag.
 *
 * Example:
 *
 * @code
 * ECS_TAG(world, MyTag);
 * @endcode
 */
#define ECS_TAG(world, id) ECS_ENTITY(world, id, 0)

/** Forward declare a prefab. */
#define ECS_PREFAB_DECLARE ECS_DECLARE

/** Define a forward-declared prefab.
 *
 * Example:
 *
 * @code
 * ECS_PREFAB_DEFINE(world, MyPrefab, Position, Velocity);
 * @endcode
 */
#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)

/** Declare and define a prefab.
 *
 * Example:
 *
 * @code
 * ECS_PREFAB(world, MyPrefab, Position, Velocity);
 * @endcode
 */
#define ECS_PREFAB(world, id, ...) ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/** Forward declare a component. */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward-declared component.
 *
 * Example:
 *
 * @code
 * ECS_COMPONENT_DEFINE(world, Position);
 * @endcode
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
    ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, "failed to create component %s", #id_)

/** Declare and define a component.
 *
 * Example:
 *
 * @code
 * ECS_COMPONENT(world, Position);
 * @endcode
 */
#define ECS_COMPONENT(world, id)\
    ecs_entity_t ecs_id(id) = 0;\
    ECS_COMPONENT_DEFINE(world, id);\
    (void)ecs_id(id)

/** Forward declare an observer. */
#define ECS_OBSERVER_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward-declared observer.
 *
 * Example:
 *
 * @code
 * ECS_OBSERVER_DEFINE(world, AddPosition, EcsOnAdd, Position);
 * @endcode
 */
#define ECS_OBSERVER_DEFINE(world, id_, kind, ...)\
    {\
        ecs_observer_desc_t desc = {0};\
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_); \
        edesc.name = #id_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.callback = id_;\
        desc.query.expr = #__VA_ARGS__;\
        desc.events[0] = kind;\
        ecs_id(id_) = ecs_observer_init(world, &desc);\
        ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, "failed to create observer %s", #id_);\
    }

/** Declare and define an observer.
 *
 * Example:
 *
 * @code
 * ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);
 * @endcode
 */
#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ecs_id(id) = 0; \
    ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id

/** Forward declare a query. */
#define ECS_QUERY_DECLARE(name)         ecs_query_t* name

/** Define a forward-declared query.
 *
 * Example:
 *
 * @code
 * ECS_QUERY_DEFINE(world, Move, Position, [in] Velocity);
 * @endcode
 */
#define ECS_QUERY_DEFINE(world, name_, ...)\
    {\
        ecs_query_desc_t desc = {0};\
        ecs_entity_desc_t edesc = {0}; \
        edesc.name = #name_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.expr = #__VA_ARGS__;\
        name_ = ecs_query_init(world, &desc);\
        ecs_assert(name_ != NULL, ECS_INVALID_PARAMETER, "failed to create query %s", #name_);\
    }

/** Declare and define a query.
 *
 * Example:
 *
 * @code
 * ECS_QUERY(world, Move, Position, [in] Velocity);
 * @endcode
 */
#define ECS_QUERY(world, name, ...)\
    ecs_query_t* name = NULL; \
    ECS_QUERY_DEFINE(world, name, __VA_ARGS__);\
    (void)name

/** Shorthand for creating an entity with ecs_entity_init().
 *
 * Example:
 *
 * @code
 * ecs_entity(world, {
 *   .name = "MyEntity"
 * });
 * @endcode
 */
#define ecs_entity(world, ...)\
    ecs_entity_init(world, &(ecs_entity_desc_t) __VA_ARGS__ )

/** Shorthand for creating a component with ecs_component_init().
 *
 * Example:
 *
 * @code
 * ecs_component(world, {
 *   .type.size = 4,
 *   .type.alignment = 4
 * });
 * @endcode
 */
#define ecs_component(world, ...)\
    ecs_component_init(world, &(ecs_component_desc_t) __VA_ARGS__ )

/** Shorthand for creating a query with ecs_query_init().
 *
 * Example:
 *
 * @code
 *   ecs_query(world, {
 *     .terms = {{ ecs_id(Position) }}
 *   });
 * @endcode
 */
#define ecs_query(world, ...)\
    ecs_query_init(world, &(ecs_query_desc_t) __VA_ARGS__ )

/** Shorthand for creating an observer with ecs_observer_init().
 *
 * Example:
 *
 * @code
 * ecs_observer(world, {
 *   .terms = {{ ecs_id(Position) }},
 *   .events = { EcsOnAdd },
 *   .callback = AddPosition
 * });
 * @endcode
 */
#define ecs_observer(world, ...)\
    ecs_observer_init(world, &(ecs_observer_desc_t) __VA_ARGS__ )

/** @} */

/**
 * @defgroup flecs_c_type_safe Type-Safe API
 * Macros that wrap around core functions to provide a "type-safe" API in C.
 *
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

/** Create a new entity with a component. */
#define ecs_new_w(world, T) ecs_new_w_id(world, ecs_id(T))

/** Create a new entity with a pair. */
#define ecs_new_w_pair(world, first, second)\
    ecs_new_w_id(world, ecs_pair(first, second))

/** Bulk create entities with a component. */
#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)

/** @} */

/**
 * @defgroup flecs_c_adding_removing Adding & Removing
 * @{
 */

/** Add a component to an entity. */
#define ecs_add(world, entity, T)\
    ecs_add_id(world, entity, ecs_id(T))

/** Add a pair to an entity. */
#define ecs_add_pair(world, subject, first, second)\
    ecs_add_id(world, subject, ecs_pair(first, second))

/** Remove a component from an entity. */
#define ecs_remove(world, entity, T)\
    ecs_remove_id(world, entity, ecs_id(T))

/** Remove a pair from an entity. */
#define ecs_remove_pair(world, subject, first, second)\
    ecs_remove_id(world, subject, ecs_pair(first, second))

/** Auto-override a component on an entity. */
#define ecs_auto_override(world, entity, T)\
    ecs_auto_override_id(world, entity, ecs_id(T))

/** Auto-override a pair on an entity. */
#define ecs_auto_override_pair(world, subject, first, second)\
    ecs_auto_override_id(world, subject, ecs_pair(first, second))

/** @} */

/**
 * @defgroup flecs_c_getting_setting Getting & Setting
 * @{
 */

/** Insert a new entity with a list of component values. */
#define ecs_insert(world, ...)\
    ecs_entity(world, { .set = ecs_values(__VA_ARGS__)})

/** Set a component using a pointer. */
#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), ptr)

/** Set a component value. */
#define ecs_set(world, entity, component, ...)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)

/** Set the first element of a pair. */
#define ecs_set_pair(world, subject, First, second, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(ecs_id(First), second),\
        sizeof(First), &(First)__VA_ARGS__)

/** Set the second element of a pair. */
#define ecs_set_pair_second(world, subject, first, Second, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(first, ecs_id(Second)),\
        sizeof(Second), &(Second)__VA_ARGS__)

/** Set a component with auto-override. */
#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_AUTO_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

/** Emplace a component. */
#define ecs_emplace(world, entity, T, is_new)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T), sizeof(T), is_new)))

/** Emplace the first element of a pair. */
#define ecs_emplace_pair(world, entity, First, second, is_new)\
    (ECS_CAST(First*, ecs_emplace_id(world, entity, ecs_pair_t(First, second), sizeof(First), is_new)))

/** Get a component. */
#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

/** Get the first element of a pair. */
#define ecs_get_pair(world, subject, First, second)\
    (ECS_CAST(const First*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

/** Get the second element of a pair. */
#define ecs_get_pair_second(world, subject, first, Second)\
    (ECS_CAST(const Second*, ecs_get_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

/** Get a mutable pointer to a component. */
#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

/** Get a mutable pointer to the first element of a pair. */
#define ecs_get_mut_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

/** Get a mutable pointer to the second element of a pair. */
#define ecs_get_mut_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_get_mut_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

/** Get a mutable pointer to a component. */
#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

/** Ensure entity has a component, return mutable pointer. */
#define ecs_ensure(world, entity, T)\
    (ECS_CAST(T*, ecs_ensure_id(world, entity, ecs_id(T), sizeof(T))))

/** Ensure entity has the first element of a pair, return mutable pointer. */
#define ecs_ensure_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_ensure_id(world, subject,\
        ecs_pair(ecs_id(First), second), sizeof(First))))

/** Ensure entity has the second element of a pair, return mutable pointer. */
#define ecs_ensure_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_ensure_id(world, subject,\
        ecs_pair(first, ecs_id(Second)), sizeof(Second))))

/** Signal that a component has been modified. */
#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

/** Signal that a pair has been modified. */
#define ecs_modified_pair(world, subject, first, second)\
    ecs_modified_id(world, subject, ecs_pair(first, second))

/** Get a component from a record. */
#define ecs_record_get(world, record, T)\
    (ECS_CAST(const T*, ecs_record_get_id(world, record, ecs_id(T))))

/** Test if a record has a component. */
#define ecs_record_has(world, record, T)\
    (ecs_record_has_id(world, record, ecs_id(T)))

/** Get the first element of a pair from a record. */
#define ecs_record_get_pair(world, record, First, second)\
    (ECS_CAST(const First*, ecs_record_get_id(world, record, \
        ecs_pair(ecs_id(First), second))))

/** Get the second element of a pair from a record. */
#define ecs_record_get_pair_second(world, record, first, Second)\
    (ECS_CAST(const Second*, ecs_record_get_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

/** Ensure a component on a record, return mutable pointer. */
#define ecs_record_ensure(world, record, T)\
    (ECS_CAST(T*, ecs_record_ensure_id(world, record, ecs_id(T))))

/** Ensure the first element of a pair on a record, return mutable pointer. */
#define ecs_record_ensure_pair(world, record, First, second)\
    (ECS_CAST(First*, ecs_record_ensure_id(world, record, \
        ecs_pair(ecs_id(First), second))))

/** Ensure the second element of a pair on a record, return mutable pointer. */
#define ecs_record_ensure_pair_second(world, record, first, Second)\
    (ECS_CAST(Second*, ecs_record_ensure_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

/** Initialize a ref for a component. */
#define ecs_ref_init(world, entity, T)\
    ecs_ref_init_id(world, entity, ecs_id(T))

/** Get a component from a ref. */
#define ecs_ref_get(world, ref, T)\
    (ECS_CAST(T*, ecs_ref_get_id(world, ref, ecs_id(T))))

/** @} */

/**
 * @defgroup flecs_c_singletons Singletons
 * @{
 */

/** Add a singleton component. */
#define ecs_singleton_add(world, comp)\
    ecs_add(world, ecs_id(comp), comp)

/** Remove a singleton component. */
#define ecs_singleton_remove(world, comp)\
    ecs_remove(world, ecs_id(comp), comp)

/** Get a singleton component. */
#define ecs_singleton_get(world, comp)\
    ecs_get(world, ecs_id(comp), comp)

/** Get a mutable pointer to a singleton component. */
#define ecs_singleton_get_mut(world, comp)\
    ecs_get_mut(world, ecs_id(comp), comp)

/** Set a singleton component using a pointer. */
#define ecs_singleton_set_ptr(world, comp, ptr)\
    ecs_set_ptr(world, ecs_id(comp), comp, ptr)

/** Set a singleton component value. */
#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)

/** Ensure a singleton component, return mutable pointer. */
#define ecs_singleton_ensure(world, comp)\
    ecs_ensure(world, ecs_id(comp), comp)

/** Emplace a singleton component. */
#define ecs_singleton_emplace(world, comp, is_new)\
    ecs_emplace(world, ecs_id(comp), comp, is_new)

/** Signal that a singleton component has been modified. */
#define ecs_singleton_modified(world, comp)\
    ecs_modified(world, ecs_id(comp), comp)

/** @} */

/**
 * @defgroup flecs_c_has Has, Owns, Shares
 * @{
 */

/** Test if an entity has a component. */
#define ecs_has(world, entity, T)\
    ecs_has_id(world, entity, ecs_id(T))

/** Test if an entity has a pair. */
#define ecs_has_pair(world, entity, first, second)\
    ecs_has_id(world, entity, ecs_pair(first, second))

/** Test if an entity owns a pair. */
#define ecs_owns_pair(world, entity, first, second)\
    ecs_owns_id(world, entity, ecs_pair(first, second))

/** Test if an entity owns a component. */
#define ecs_owns(world, entity, T)\
    ecs_owns_id(world, entity, ecs_id(T))

/** Test if an entity shares a component. */
#define ecs_shares_id(world, entity, id)\
    (ecs_search_relation(world, ecs_get_table(world, entity), 0, ecs_id(id), \
        EcsIsA, 1, 0, 0, 0, 0) != -1)

/** Test if an entity shares a pair. */
#define ecs_shares_pair(world, entity, first, second)\
    (ecs_shares_id(world, entity, ecs_pair(first, second)))

/** Test if an entity shares a component. */
#define ecs_shares(world, entity, T)\
    (ecs_shares_id(world, entity, ecs_id(T)))

/** Get the target for a relationship. */
#define ecs_get_target_for(world, entity, rel, T)\
    ecs_get_target_for_id(world, entity, rel, ecs_id(T))

/** @} */

/**
 * @defgroup flecs_c_enable_disable Enabling & Disabling
 * @{
 */

/** Enable or disable a component. */
#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_id(world, entity, ecs_id(T), enable)

/** Test if a component is enabled. */
#define ecs_is_enabled(world, entity, T)\
    ecs_is_enabled_id(world, entity, ecs_id(T))

/** Enable or disable a pair. */
#define ecs_enable_pair(world, entity, First, second, enable)\
    ecs_enable_id(world, entity, ecs_pair(ecs_id(First), second), enable)

/** Test if a pair is enabled. */
#define ecs_is_enabled_pair(world, entity, First, second)\
    ecs_is_enabled_id(world, entity, ecs_pair(ecs_id(First), second))

/** @} */

/**
 * @defgroup flecs_c_entity_names Entity Names
 * @{
 */

/** Lookup an entity from a parent. */
#define ecs_lookup_from(world, parent, path)\
    ecs_lookup_path_w_sep(world, parent, path, ".", NULL, true)

/** Get path from a parent. */
#define ecs_get_path_from(world, parent, child)\
    ecs_get_path_w_sep(world, parent, child, ".", NULL)

/** Get path from root. */
#define ecs_get_path(world, child)\
    ecs_get_path_w_sep(world, 0, child, ".", NULL)

/** Get path from root, write to buffer. */
#define ecs_get_path_buf(world, child, buf)\
    ecs_get_path_w_sep_buf(world, 0, child, ".", NULL, buf, false)

/** Create a new entity from a path. */
#define ecs_new_from_path(world, parent, path)\
    ecs_new_from_path_w_sep(world, parent, path, ".", NULL)

/** Add a path to an entity. */
#define ecs_add_path(world, entity, parent, path)\
    ecs_add_path_w_sep(world, entity, parent, path, ".", NULL)

/** Add a full path to an entity. */
#define ecs_add_fullpath(world, entity, path)\
    ecs_add_path_w_sep(world, entity, 0, path, ".", NULL)

/** @} */

/** @} */

/**
 * @defgroup flecs_c_components Component API
 * @{
 */

/** Set component hooks. */
#define ecs_set_hooks(world, T, ...)\
    ecs_set_hooks_id(world, ecs_id(T), &(ecs_type_hooks_t)__VA_ARGS__)

/** Get component hooks. */
#define ecs_get_hooks(world, T)\
    ecs_get_hooks_id(world, ecs_id(T));

/** Declare a constructor.
 * Example:
 *
 * @code
 * ECS_CTOR(MyType, ptr, { ptr->value = NULL; });
 * @endcode
 */
#define ECS_CTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, ctor, var, __VA_ARGS__)

/** Declare a destructor.
 * Example:
 *
 * @code
 * ECS_DTOR(MyType, ptr, { free(ptr->value); });
 * @endcode
 */
#define ECS_DTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, dtor, var, __VA_ARGS__)

/** Declare a copy action.
 * Example:
 *
 * @code
 * ECS_COPY(MyType, dst, src, { dst->value = strdup(src->value); });
 * @endcode
 */
#define ECS_COPY(type, dst_var, src_var, ...)\
    ECS_COPY_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare a move action.
 * Example:
 *
 * @code
 * ECS_MOVE(MyType, dst, src, { dst->value = src->value; src->value = 0; });
 * @endcode
 */
#define ECS_MOVE(type, dst_var, src_var, ...)\
    ECS_MOVE_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare an on_add hook.
 * Example:
 *
 * @code
 * ECS_ON_ADD(MyType, ptr, { printf("added\n"); });
 * @endcode
 */
#define ECS_ON_ADD(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_add(type), ptr, __VA_ARGS__)
/** Declare an on_remove hook. */
#define ECS_ON_REMOVE(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_remove(type), ptr, __VA_ARGS__)
/** Declare an on_set hook. */
#define ECS_ON_SET(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_set(type), ptr, __VA_ARGS__)

/** Map from typename to constructor function name. */
#define ecs_ctor(type) type##_ctor
/** Map from typename to destructor function name. */
#define ecs_dtor(type) type##_dtor
/** Map from typename to copy function name. */
#define ecs_copy(type) type##_copy
/** Map from typename to move function name. */
#define ecs_move(type) type##_move
/** Map from typename to on_set function name. */
#define ecs_on_set(type) type##_on_set
/** Map from typename to on_add function name. */
#define ecs_on_add(type) type##_on_add
/** Map from typename to on_remove function name. */
#define ecs_on_remove(type) type##_on_remove

/** @} */

/**
 * @defgroup flecs_c_ids ID API
 * @{
 */

/** Count entities with a component. */
#define ecs_count(world, type)\
    ecs_count_id(world, ecs_id(type))

/** @} */

/**
 * @defgroup flecs_c_iterators Iterator API
 * @{
 */

/** Get field data for a component. */
#define ecs_field(it, T, index)\
    (ECS_CAST(T*, ecs_field_w_size(it, sizeof(T), index)))

/** Get field data for a self-owned component. */
#define ecs_field_self(it, T, index)\
    (ECS_CAST(T*, ecs_field_self_w_size(it, sizeof(T), index)))

/** Get field data at a specific row. */
#define ecs_field_at(it, T, index, row)\
    (ECS_CAST(T*, ecs_field_at_w_size(it, sizeof(T), index, row)))

/** @} */

/**
 * @defgroup flecs_c_tables Table API
 * @{
 */

/** Get a component from a table at an offset. */
#define ecs_table_get(world, table, T, offset)\
    (ECS_CAST(T*, ecs_table_get_id(world, table, ecs_id(T), offset)))

/** Get the first element of a pair from a table at an offset. */
#define ecs_table_get_pair(world, table, First, second, offset)\
    (ECS_CAST(First*, ecs_table_get_id(world, table, ecs_pair(ecs_id(First), second), offset)))

/** Get the second element of a pair from a table at an offset. */
#define ecs_table_get_pair_second(world, table, first, Second, offset)\
    (ECS_CAST(Second*, ecs_table_get_id(world, table, ecs_pair(first, ecs_id(Second)), offset)))

/** @} */

/**
 * @defgroup flecs_c_values Value API
 * @{
 */

/** Convenience macro for creating a compound literal ID array. */
#define ecs_ids(...) (ecs_id_t[]){ __VA_ARGS__, 0 }

/** Convenience macro for creating a compound literal values array. */
#define ecs_values(...) (ecs_value_t[]){ __VA_ARGS__, {0, 0}}

/** Convenience macro for creating a compound literal value. */
#define ecs_value_ptr(T, ptr) ((ecs_value_t){ecs_id(T), ptr})

/** Convenience macro for creating a compound literal pair value. */
#define ecs_pair_value(R, t, ...) ((ecs_value_t){ecs_pair_t(R, t), &(R)__VA_ARGS__})

/** Convenience macro for creating a compound literal pair value. */
#define ecs_pair_value_2nd(r, T, ...) ((ecs_value_t){ecs_pair(r, ecs_id(T)), &(T)__VA_ARGS__})

/** Convenience macro for creating a heap-allocated value. */
#define ecs_value_new_t(world, T) ecs_value_new(world, ecs_id(T))

/** Convenience macro for creating a compound literal value literal. */
#define ecs_value(T, ...) ((ecs_value_t){ecs_id(T), &(T)__VA_ARGS__})

/** @} */

/** @} */

/**
 * @defgroup flecs_c_table_sorting Table sorting
 * Convenience macros for sorting tables.
 *
 * @{
 */
/** Declare a table sort function. */
#define ecs_sort_table(id) ecs_id(id##_sort_table)

/** Declare a comparison function. */
#define ecs_compare(id) ecs_id(id##_compare_fn)

/** Declare an efficient table sorting operation that uses the provided compare function.
 * For best results, use LTO or make the function body visible in the same compilation unit.
 * Variadic arguments are prepended before generated functions; use them to declare static
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
 *
 * @code
 * int CompareMyType(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; }
 * ECS_SORT_TABLE_WITH_COMPARE(MyType, MyCustomCompare, CompareMyType)
 * @endcode
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

/** Declare an efficient table sorting operation that uses the default component comparison operator.
 * For best results, use LTO or make the comparison operator visible in the same compilation unit.
 * Variadic arguments are prepended before generated functions; use them to declare static
 *   or exported functions.
 * Example:
 *
 * @code
 * ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 * ECS_SORT_TABLE(MyType)
 * @endcode
 */
#define ECS_SORT_TABLE(id, ...) \
    ECS_SORT_TABLE_WITH_COMPARE(id, ecs_sort_table(id), ecs_compare(id), __VA_ARGS__)

/** Declare component comparison operations.
 * Parameters:
 *   ecs_entity_t e1, const void* ptr1,
 *   ecs_entity_t e2, const void* ptr2
 * Example:
 *
 * @code
 * ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 * @endcode
 */
#define ECS_COMPARE(id, ...) \
    int ecs_compare(id)(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { \
        __VA_ARGS__ \
    }

/** @} */

/**
 * @defgroup flecs_c_misc Misc
 * Misc convenience macros.
 *
 * @{
 */

/** Construct an IsA pair. */
#define ecs_isa(e)       ecs_pair(EcsIsA, e)
/** Construct a ChildOf pair. */
#define ecs_childof(e)   ecs_pair(EcsChildOf, e)
/** Construct a DependsOn pair. */
#define ecs_dependson(e) ecs_pair(EcsDependsOn, e)
/** Construct a With pair. */
#define ecs_with(e)      ecs_pair(EcsWith, e)

/** Iterate entities with a component. */
#define ecs_each(world, id) ecs_each_id(world, ecs_id(id))
/** Iterate entities with a pair. */
#define ecs_each_pair(world, r, t) ecs_each_id(world, ecs_pair(r, t))
/** Iterate entities with a pair (type-safe first element). */
#define ecs_each_pair_t(world, R, t) ecs_each_id(world, ecs_pair(ecs_id(R), t))

/** @} */

/** @} */

#endif // FLECS_C_

#ifdef __cplusplus
}
#endif

#ifndef FLECS_ADDONS_H
#define FLECS_ADDONS_H

/* Blacklist macros */
#ifdef FLECS_NO_CPP
#undef FLECS_CPP
#endif
#ifdef FLECS_NO_MODULE
#undef FLECS_MODULE
#endif
#ifdef FLECS_NO_SCRIPT
#undef FLECS_SCRIPT
#endif
#ifdef FLECS_NO_PARSER
#undef FLECS_PARSER
#endif
#ifdef FLECS_NO_QUERY_DSL
#undef FLECS_QUERY_DSL
#endif
#ifdef FLECS_NO_SCRIPT_MATH
#undef FLECS_SCRIPT_MATH
#endif
#ifdef FLECS_NO_STATS
#undef FLECS_STATS
#endif
#ifdef FLECS_NO_SYSTEM
#undef FLECS_SYSTEM
#endif
#ifdef FLECS_NO_ALERTS
#undef FLECS_ALERTS
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
#ifdef FLECS_NO_UNITS
#undef FLECS_UNITS
#endif
#ifdef FLECS_NO_JSON
#undef FLECS_JSON
#endif
#ifdef FLECS_NO_DOC
#undef FLECS_DOC
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

/* Always included; if disabled, functions are replaced with dummy macros. */

#ifndef FLECS_LOG_H
#define FLECS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

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

////////////////////////////////////////////////////////////////////////////////
//// Logging functions (do nothing when logging is disabled)
////////////////////////////////////////////////////////////////////////////////

/** Print at the provided log level.
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_print_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Print at the provided log level (va_list).
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_printv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

/** Log at the provided level.
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_log_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Log at the provided level (va_list).
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_logv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

/** Abort with error code.
 *
 * @param error_code The error code.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_abort_(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Log an assertion failure.
 *
 * @param error_code The error code.
 * @param condition_str The condition that was not met.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_assert_log_(
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Log a parser error.
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 */
FLECS_API
void ecs_parser_error_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    ...);

/** Log a parser error (va_list).
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_parser_errorv_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    va_list args);

/** Log a parser warning.
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 */
FLECS_API
void ecs_parser_warning_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    ...);

/** Log a parser warning (va_list).
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_parser_warningv_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    va_list args);

////////////////////////////////////////////////////////////////////////////////
//// Logging macros
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY /* C89 doesn't support variadic macros */

/** Base logging function. Accepts a custom level. */
#define ecs_print(level, ...)\
    ecs_print_(level, __FILE__, __LINE__, __VA_ARGS__)

/** Base logging function with va_list. */
#define ecs_printv(level, fmt, args)\
    ecs_printv_(level, __FILE__, __LINE__, fmt, args)

/** Log at the provided level. */
#define ecs_log(level, ...)\
    ecs_log_(level, __FILE__, __LINE__, __VA_ARGS__)

/** Log at the provided level with va_list. */
#define ecs_logv(level, fmt, args)\
    ecs_logv_(level, __FILE__, __LINE__, fmt, args)

/** Tracing. Used for logging of infrequent events. */
#define ecs_trace_(file, line, ...) ecs_log_(0, file, line, __VA_ARGS__)
/** Tracing macro. */
#define ecs_trace(...) ecs_trace_(__FILE__, __LINE__, __VA_ARGS__)

/** Warning. Used when an issue occurs, but operation is successful. */
#define ecs_warn_(file, line, ...) ecs_log_(-2, file, line, __VA_ARGS__)
/** Warning macro. */
#define ecs_warn(...) ecs_warn_(__FILE__, __LINE__, __VA_ARGS__)

/** Error. Used when an issue occurs, and operation failed. */
#define ecs_err_(file, line, ...) ecs_log_(-3, file, line, __VA_ARGS__)
/** Error macro. */
#define ecs_err(...) ecs_err_(__FILE__, __LINE__, __VA_ARGS__)

/** Fatal. Used when an issue occurs, and the application cannot continue. */
#define ecs_fatal_(file, line, ...) ecs_log_(-4, file, line, __VA_ARGS__)
/** Fatal macro. */
#define ecs_fatal(...) ecs_fatal_(__FILE__, __LINE__, __VA_ARGS__)

/** Optionally include warnings about using deprecated features. */
#ifndef FLECS_NO_DEPRECATED_WARNINGS
/** Emit deprecation warning. */
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
/** Debug trace at level 1. */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
/** Debug trace at level 2. */
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
/** Debug trace at level 3. */
#define ecs_dbg_3(...) ecs_log(3, __VA_ARGS__);

/** Push log indentation at level 1. */
#define ecs_log_push_1() ecs_log_push_(1);
/** Push log indentation at level 2. */
#define ecs_log_push_2() ecs_log_push_(2);
/** Push log indentation at level 3. */
#define ecs_log_push_3() ecs_log_push_(3);

/** Pop log indentation at level 1. */
#define ecs_log_pop_1() ecs_log_pop_(1);
/** Pop log indentation at level 2. */
#define ecs_log_pop_2() ecs_log_pop_(2);
/** Pop log indentation at level 3. */
#define ecs_log_pop_3() ecs_log_pop_(3);

/** Test if level 1 logging is enabled. */
#define ecs_should_log_1() ecs_should_log(1)
/** Test if level 2 logging is enabled. */
#define ecs_should_log_2() ecs_should_log(2)
/** Test if level 3 logging is enabled. */
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

/** Default debug tracing is at level 1. */
#define ecs_dbg ecs_dbg_1

/** Push log indentation at the default level. */
#define ecs_log_push() ecs_log_push_(0)
/** Pop log indentation at the default level. */
#define ecs_log_pop() ecs_log_pop_(0)

/** Abort.
 * Unconditionally aborts the process. */
#define ecs_abort(error_code, ...)\
    ecs_abort_(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    ecs_os_abort(); abort(); /* satisfy compiler/static analyzers */

/** Assert.
 * Aborts if condition is false, disabled in release mode. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        ecs_os_abort();\
    }\
    assert(condition) /* satisfy compiler/static analyzers */
#endif // FLECS_NDEBUG

/** Debug assert.
 * Assert that is only valid in debug mode (ignores FLECS_KEEP_ASSERT). */
#ifndef FLECS_NDEBUG
#define ecs_dbg_assert(condition, error_code, ...) ecs_assert(condition, error_code, __VA_ARGS__)
#else
#define ecs_dbg_assert(condition, error_code, ...)
#endif

/** Always assert.
 * Assert that is always active, even in release builds. Always aborts on
 * failure (does not support FLECS_SOFT_ASSERT). */
#define ecs_always_assert(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        ecs_os_abort();\
    }\
    assert(condition) /* satisfy compiler/static analyzers */

/** Sanitize assert.
 * Assert that is only valid in sanitized mode (ignores FLECS_KEEP_ASSERT). */
#ifdef FLECS_SANITIZE
#define ecs_san_assert(condition, error_code, ...) ecs_assert(condition, error_code, __VA_ARGS__)
#else
#define ecs_san_assert(condition, error_code, ...)
#endif

/** Silence dead code/unused label warnings when compiling without checks. */
#define ecs_dummy_check\
    if ((false)) {\
        goto error;\
    }

/** Check.
 * Go to error if condition is false. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_check(condition, error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        goto error;\
    }\
    ecs_dummy_check
#else // FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        ecs_os_abort();\
    }\
    assert(condition); /* satisfy compiler/static analyzers */ \
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Panic.
 * Go to error when FLECS_SOFT_ASSERT is defined, otherwise abort. */
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

/** Parser error. */
#define ecs_parser_error(name, expr, column, ...)\
    ecs_parser_error_(name, expr, column, __VA_ARGS__)

/** Parser error with va_list. */
#define ecs_parser_errorv(name, expr, column, fmt, args)\
    ecs_parser_errorv_(name, expr, column, fmt, args)

/** Parser warning. */
#define ecs_parser_warning(name, expr, column, ...)\
    ecs_parser_warning_(name, expr, column, __VA_ARGS__)

/** Parser warning with va_list. */
#define ecs_parser_warningv(name, expr, column, fmt, args)\
    ecs_parser_warningv_(name, expr, column, fmt, args)

#endif // FLECS_LEGACY

////////////////////////////////////////////////////////////////////////////////
//// Functions that are always available
////////////////////////////////////////////////////////////////////////////////

/** Enable or disable log.
 * This will enable the built-in log. For log to work, it will have to be
 * compiled in, which requires defining one of the following macros:
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
 * @return Current log level.
 */
FLECS_API
int ecs_log_get_level(void);

/** Enable/disable tracing with colors.
 * By default, colors are enabled.
 *
 * @param enabled Whether to enable tracing with colors.
 * @return Previous color setting.
 */
FLECS_API
bool ecs_log_enable_colors(
    bool enabled);

/** Enable/disable logging timestamp.
 * By default, timestamps are disabled. Note that enabling timestamps introduces
 * overhead as the logging code will need to obtain the current time.
 *
 * @param enabled Whether to enable tracing with timestamps.
 * @return Previous timestamp setting.
 */
FLECS_API
bool ecs_log_enable_timestamp(
    bool enabled);

/** Enable/disable logging time since last log.
 * By default, deltatime is disabled. Note that enabling timestamps introduces
 * overhead as the logging code will need to obtain the current time.
 *
 * When enabled, this logs the amount of time in seconds passed since the last
 * log, when this amount is non-zero. The format is a '+' character followed by
 * the number of seconds:
 *
 *     +1 trace: log message
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

/** Start capturing log output.
 *
 * @param capture_try If true, also capture messages from ecs_log_try blocks.
 */
FLECS_API
void ecs_log_start_capture(bool capture_try);

/** Stop capturing log output.
 *
 * @return The captured log output, or NULL if no output was captured.
 */
FLECS_API
char* ecs_log_stop_capture(void);

////////////////////////////////////////////////////////////////////////////////
//// Error codes
////////////////////////////////////////////////////////////////////////////////

/** Invalid operation error code. */
#define ECS_INVALID_OPERATION (1)
/** Invalid parameter error code. */
#define ECS_INVALID_PARAMETER (2)
/** Constraint violated error code. */
#define ECS_CONSTRAINT_VIOLATED (3)
/** Out of memory error code. */
#define ECS_OUT_OF_MEMORY (4)
/** Out of range error code. */
#define ECS_OUT_OF_RANGE (5)
/** Unsupported error code. */
#define ECS_UNSUPPORTED (6)
/** Internal error code. */
#define ECS_INTERNAL_ERROR (7)
/** Already defined error code. */
#define ECS_ALREADY_DEFINED (8)
/** Missing OS API error code. */
#define ECS_MISSING_OS_API (9)
/** Operation failed error code. */
#define ECS_OPERATION_FAILED (10)
/** Invalid conversion error code. */
#define ECS_INVALID_CONVERSION (11)
/** Cycle detected error code. */
#define ECS_CYCLE_DETECTED (13)
/** Leak detected error code. */
#define ECS_LEAK_DETECTED (14)
/** Double free error code. */
#define ECS_DOUBLE_FREE (15)

/** Inconsistent name error code. */
#define ECS_INCONSISTENT_NAME (20)
/** Name in use error code. */
#define ECS_NAME_IN_USE (21)
/** Invalid component size error code. */
#define ECS_INVALID_COMPONENT_SIZE (23)
/** Invalid component alignment error code. */
#define ECS_INVALID_COMPONENT_ALIGNMENT (24)
/** Component not registered error code. */
#define ECS_COMPONENT_NOT_REGISTERED (25)
/** Inconsistent component id error code. */
#define ECS_INCONSISTENT_COMPONENT_ID (26)
/** Inconsistent component action error code. */
#define ECS_INCONSISTENT_COMPONENT_ACTION (27)
/** Module undefined error code. */
#define ECS_MODULE_UNDEFINED (28)
/** Missing symbol error code. */
#define ECS_MISSING_SYMBOL (29)
/** Already in use error code. */
#define ECS_ALREADY_IN_USE (30)

/** Access violation error code. */
#define ECS_ACCESS_VIOLATION (40)
/** Column index out of range error code. */
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (41)
/** Column is not shared error code. */
#define ECS_COLUMN_IS_NOT_SHARED (42)
/** Column is shared error code. */
#define ECS_COLUMN_IS_SHARED (43)
/** Column type mismatch error code. */
#define ECS_COLUMN_TYPE_MISMATCH (45)

/** Invalid while readonly error code. */
#define ECS_INVALID_WHILE_READONLY (70)
/** Locked storage error code. */
#define ECS_LOCKED_STORAGE (71)
/** Invalid from worker error code. */
#define ECS_INVALID_FROM_WORKER (72)

////////////////////////////////////////////////////////////////////////////////
//// Used when logging with colors is enabled
////////////////////////////////////////////////////////////////////////////////

/** Black ANSI color escape code. */
#define ECS_BLACK   "\033[1;30m"
/** Red ANSI color escape code. */
#define ECS_RED     "\033[0;31m"
/** Green ANSI color escape code. */
#define ECS_GREEN   "\033[0;32m"
/** Yellow ANSI color escape code. */
#define ECS_YELLOW  "\033[0;33m"
/** Blue ANSI color escape code. */
#define ECS_BLUE    "\033[0;34m"
/** Magenta ANSI color escape code. */
#define ECS_MAGENTA "\033[0;35m"
/** Cyan ANSI color escape code. */
#define ECS_CYAN    "\033[0;36m"
/** White ANSI color escape code. */
#define ECS_WHITE   "\033[1;37m"
/** Grey ANSI color escape code. */
#define ECS_GREY    "\033[0;37m"
/** Normal ANSI color escape code. */
#define ECS_NORMAL  "\033[0;49m"
/** Bold ANSI escape code. */
#define ECS_BOLD    "\033[1;49m"

#ifdef __cplusplus
}
#endif

/** @} */

#endif // FLECS_LOG_H

/* Handle addon dependencies that need declarations to be visible in the header. */

#ifdef FLECS_OS_API_IMPL
#ifdef FLECS_NO_OS_API_IMPL
#error "FLECS_NO_OS_API_IMPL failed: OS_API_IMPL is required by other addons"
#endif

#ifdef FLECS_OS_API_IMPL

/**
 * @defgroup c_addons_os_api_impl OS API Implementation
 * @ingroup c_addons
 * Default implementation for OS API interface.
 *
 * @{
 */

#ifndef FLECS_OS_API_IMPL_H
#define FLECS_OS_API_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/** Set default OS API implementation.
 * This initializes the OS API with a default implementation for the current
 * platform.
 */
FLECS_API
void ecs_set_os_api_impl(void);

#ifdef __cplusplus
}
#endif

#endif // FLECS_OS_API_IMPL_H

/** @} */

#endif // FLECS_OS_API_IMPL

#endif

#endif

#endif

