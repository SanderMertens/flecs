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
 * @ingroup c
 * Core ECS functionality (entities, storage, queries).
 *
 * @{
 */

/**
 * @defgroup options API defines
 * Defines for customizing compile time features.
 *
 * @{
 */

/* Flecs version macros */
#define FLECS_VERSION_MAJOR 4  /**< Flecs major version. */
#define FLECS_VERSION_MINOR 1  /**< Flecs minor version. */
#define FLECS_VERSION_PATCH 1  /**< Flecs patch version. */

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
 * Customizable precision for floating point operations */
#ifndef ecs_float_t
#define ecs_float_t float
#endif

/** @def ecs_ftime_t
 * Customizable precision for scalar time values. Change to double precision for
 * processes that can run for a long time (e.g. longer than a day). */
#ifndef ecs_ftime_t
#define ecs_ftime_t ecs_float_t
#endif

/** @def FLECS_LEGACY
 * Define when building for C89
 */
// #define FLECS_LEGACY

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

/** @def FLECS_KEEP_ASSERT
 * By default asserts are disabled in release mode, when either FLECS_NDEBUG or
 * NDEBUG is defined. Defining FLECS_KEEP_ASSERT ensures that asserts are not
 * disabled. This define can be combined with FLECS_SOFT_ASSERT.
 */
// #define FLECS_KEEP_ASSERT

/** @def FLECS_DEFAULT_TO_UNCACHED_QUERIES 
 * When set, this will cause queries with the EcsQueryCacheDefault policy
 * to default to EcsQueryCacheNone. This can reduce the memory footprint of
 * applications at the cost of performance. Queries that use features which 
 * require caching such as group_by and order_by will still use caching.
 */
// #define FLECS_DEFAULT_TO_UNCACHED_QUERIES

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
 * Disabling this feature can significantly improve compile times and reduces
 * the RAM footprint of an application.
 */
// #define FLECS_CPP_NO_ENUM_REFLECTION

/** @def FLECS_NO_ALWAYS_INLINE
 * When set, this will prevent functions from being annotated with always_inline
 * which can improve performance at the cost of increased binary footprint.
 */
// #define FLECS_NO_ALWAYS_INLINE

/** @def FLECS_CUSTOM_BUILD
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
 *
 * @code
 * ecs_log_set_level(0);
 * @endcode
 *
 * which outputs the full list of addons Flecs was compiled with.
 */
// #define FLECS_CUSTOM_BUILD

#ifndef FLECS_CUSTOM_BUILD
#define FLECS_ALERTS         /**< Monitor conditions for errors */
#define FLECS_APP            /**< Application addon */
// #define FLECS_C           /**< C API convenience macros, always enabled */
#define FLECS_CPP            /**< C++ API */
#define FLECS_DOC            /**< Document entities & components */
// #define FLECS_JOURNAL     /**< Journaling addon */
#define FLECS_JSON           /**< Parsing JSON to/from component values */
#define FLECS_HTTP           /**< Tiny HTTP server for connecting to remote UI */
#define FLECS_LOG            /**< When enabled ECS provides more detailed logs */
#define FLECS_META           /**< Reflection support */
#define FLECS_METRICS        /**< Expose component data as statistics */
#define FLECS_MODULE         /**< Module support */
#define FLECS_OS_API_IMPL    /**< Default implementation for OS API */
// #define FLECS_PERF_TRACE  /**< Enable performance tracing */
#define FLECS_PIPELINE       /**< Pipeline support */
#define FLECS_REST           /**< REST API for querying application data */
#define FLECS_PARSER         /**< Utilities for script and query DSL parsers */
#define FLECS_QUERY_DSL      /**< Flecs query DSL parser */
#define FLECS_SCRIPT         /**< Flecs entity notation language */
// #define FLECS_SCRIPT_MATH /**< Math functions for flecs script (may require linking with libm) */
#define FLECS_SYSTEM         /**< System support */
#define FLECS_STATS          /**< Track runtime statistics */
#define FLECS_TIMER          /**< Timer support */
#define FLECS_UNITS          /**< Builtin standard units */
#endif // ifndef FLECS_CUSTOM_BUILD

/** @def FLECS_LOW_FOOTPRINT
 * Set a number of constants to values that decrease memory footprint, at the
 * cost of decreased performance. */
// #define FLECS_LOW_FOOTPRINT
#ifdef FLECS_LOW_FOOTPRINT
#define FLECS_HI_COMPONENT_ID (16)
#define FLECS_HI_ID_RECORD_ID (16)
#define FLECS_ENTITY_PAGE_BITS (6)
#define FLECS_USE_OS_ALLOC
#define FLECS_DEFAULT_TO_UNCACHED_QUERIES
#endif

/** @def FLECS_HI_COMPONENT_ID
 * This constant can be used to balance between performance and memory
 * utilization. The constant is used in two ways:
 * - Entity ids 0..FLECS_HI_COMPONENT_ID are reserved for component ids.
 * - Used as lookup array size in table edges.
 *
 * Increasing this value increases the size of the lookup array, which allows
 * fast table traversal, which improves performance of ECS add/remove
 * operations. Component ids that fall outside of this range use a regular map
 * lookup, which is slower but more memory efficient. 
 * 
 * This value must be set to a value that is a power of 2. Setting it to a value
 * that is not a power of two will degrade performance.
 */
#ifndef FLECS_HI_COMPONENT_ID
#define FLECS_HI_COMPONENT_ID (256)
#endif

/** @def FLECS_HI_ID_RECORD_ID
 * This constant can be used to balance between performance and memory
 * utilization. The constant is used to determine the size of the component record
 * lookup array. Id values that fall outside of this range use a regular map
 * lookup, which is slower but more memory efficient.
 */
#ifndef FLECS_HI_ID_RECORD_ID
#define FLECS_HI_ID_RECORD_ID (1024)
#endif

/** @def FLECS_SPARSE_PAGE_BITS
 * This constant is used to determine the number of bits of an id that is used
 * to determine the page index when used with a sparse set. The number of bits
 * determines the page size, which is (1 << bits).
 * Lower values decrease memory utilization, at the cost of more allocations. */
#ifndef FLECS_SPARSE_PAGE_BITS
#define FLECS_SPARSE_PAGE_BITS (6)
#endif

/** @def FLECS_ENTITY_PAGE_BITS
 * Same as FLECS_SPARSE_PAGE_BITS, but for the entity index. */
#ifndef FLECS_ENTITY_PAGE_BITS
#define FLECS_ENTITY_PAGE_BITS (10)
#endif

/** @def FLECS_USE_OS_ALLOC
 * When enabled, Flecs will use the OS allocator provided in the OS API directly
 * instead of the builtin block allocator. This can decrease memory utilization
 * as memory will be freed more often, at the cost of decreased performance. */
// #define FLECS_USE_OS_ALLOC

/** @def FLECS_ID_DESC_MAX
 * Maximum number of ids to add ecs_entity_desc_t / ecs_bulk_desc_t */
#ifndef FLECS_ID_DESC_MAX
#define FLECS_ID_DESC_MAX (32)
#endif

/** @def FLECS_EVENT_DESC_MAX
 * Maximum number of events in ecs_observer_desc_t */
#ifndef FLECS_EVENT_DESC_MAX
#define FLECS_EVENT_DESC_MAX (8)
#endif

/** @def FLECS_VARIABLE_COUNT_MAX
 * Maximum number of query variables per query */
#define FLECS_VARIABLE_COUNT_MAX (64)

/** @def FLECS_TERM_COUNT_MAX 
 * Maximum number of terms in queries. Should not exceed 64. */
#ifndef FLECS_TERM_COUNT_MAX
#define FLECS_TERM_COUNT_MAX 32
#endif

/** @def FLECS_TERM_ARG_COUNT_MAX 
 * Maximum number of arguments for a term. */
#ifndef FLECS_TERM_ARG_COUNT_MAX
#define FLECS_TERM_ARG_COUNT_MAX (16)
#endif

/** @def FLECS_QUERY_VARIABLE_COUNT_MAX
 * Maximum number of query variables per query. Should not exceed 128. */
#ifndef FLECS_QUERY_VARIABLE_COUNT_MAX
#define FLECS_QUERY_VARIABLE_COUNT_MAX (64)
#endif

/** @def FLECS_QUERY_SCOPE_NESTING_MAX
 * Maximum nesting depth of query scopes */
#ifndef FLECS_QUERY_SCOPE_NESTING_MAX
#define FLECS_QUERY_SCOPE_NESTING_MAX (8)
#endif

/** @def FLECS_DAG_DEPTH_MAX
 * Maximum of levels in a DAG (acyclic relationship graph). If a graph with a
 * depth larger than this is encountered, a CYCLE_DETECTED panic is thrown.
 */
#ifndef FLECS_DAG_DEPTH_MAX
#define FLECS_DAG_DEPTH_MAX (128)
#endif

/** @} */

#include "flecs/private/api_defines.h"

/**
 * @defgroup core_types Core API Types
 * Types for core API objects.
 *
 * @{
 */

/** Ids are the things that can be added to an entity.
 * An id can be an entity or pair, and can have optional id flags. */
typedef uint64_t ecs_id_t;

/** An entity identifier.
 * Entity ids consist out of a number unique to the entity in the lower 32 bits,
 * and a counter used to track entity liveliness in the upper 32 bits. When an
 * id is recycled, its generation count is increased. This causes recycled ids
 * to be very large (>4 billion), which is normal. */
typedef ecs_id_t ecs_entity_t;

/** A type is a list of (component) ids.
 * Types are used to communicate the "type" of an entity. In most type systems a
 * typeof operation returns a single type. In ECS however, an entity can have
 * multiple components, which is why an ECS type consists of a vector of ids.
 *
 * The component ids of a type are sorted, which ensures that it doesn't matter
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
    ecs_id_t *array;    /**< Array with ids. */
    int32_t count;      /**< Number of elements in array. */
} ecs_type_t;

/** A world is the container for all ECS data and supporting features.
 * Applications can have multiple worlds, though in most cases will only need
 * one. Worlds are isolated from each other, and can have separate sets of
 * systems, components, modules etc.
 *
 * If an application has multiple worlds with overlapping components, it is
 * common (though not strictly required) to use the same component ids across
 * worlds, which can be achieved by declaring a global component id variable.
 * To do this in the C API, see the entities/fwd_component_decl example. The
 * C++ API automatically synchronizes component ids between worlds.
 *
 * Component id conflicts between worlds can occur when a world has already used
 * an id for something else. There are a few ways to avoid this:
 *
 * - Ensure to register the same components in each world, in the same order.
 * - Create a dummy world in which all components are preregistered which
 *   initializes the global id variables.
 *
 * In some use cases, typically when writing tests, multiple worlds are created
 * and deleted with different components, registered in different order. To
 * ensure isolation between tests, the C++ API has a `flecs::reset` function
 * that forces the API to ignore the old component ids. */
typedef struct ecs_world_t ecs_world_t;

/** A stage enables modification while iterating and from multiple threads */
typedef struct ecs_stage_t ecs_stage_t;

/** A table stores entities and components for a specific type. */
typedef struct ecs_table_t ecs_table_t;

/** A term is a single element in a query. */
typedef struct ecs_term_t ecs_term_t;

/** A query returns entities matching a list of constraints. */
typedef struct ecs_query_t ecs_query_t;

/** An observer is a system that is invoked when an event matches its query.
 * Observers allow applications to respond to specific events, such as adding or
 * removing a component. Observers are created by both specifying a query and
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
 * Typical examples of lifecycle events are construction, destruction, copying
 * and moving of components. */
typedef struct ecs_type_hooks_t ecs_type_hooks_t;

/** Type information.
 * Contains information about a (component) type, such as its size and
 * alignment and type hooks. */
typedef struct ecs_type_info_t ecs_type_info_t;

/** Information about an entity, like its table and row. */
typedef struct ecs_record_t ecs_record_t;

/** Information about a (component) id, such as type info and tables with the id */
typedef struct ecs_component_record_t ecs_component_record_t;

/** A poly object.
 * A poly (short for polymorph) object is an object that has a variable list of
 * capabilities, determined by a mixin table. This is the current list of types
 * in the flecs API that can be used as an ecs_poly_t:
 *
 * - ecs_world_t
 * - ecs_stage_t
 * - ecs_query_t
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
    int32_t type;               /**< Magic number indicating which type of flecs object */
    int32_t refcount;           /**< Refcount, to enable RAII handles */
    ecs_mixins_t *mixins;       /**< Table with offsets to (optional) mixins */
} ecs_header_t;

typedef struct ecs_table_record_t ecs_table_record_t;

/** @} */

#include "flecs/datastructures/vec.h"              /* Vector datatype */
#include "flecs/datastructures/sparse.h"           /* Sparse set */
#include "flecs/datastructures/block_allocator.h"  /* Block allocator */
#include "flecs/datastructures/stack_allocator.h"  /* Stack allocator */
#include "flecs/datastructures/map.h"              /* Map */
#include "flecs/datastructures/allocator.h"        /* Allocator */
#include "flecs/datastructures/strbuf.h"           /* String builder */
#include "flecs/os_api.h"  /* Abstraction for operating system functions */

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

/** Compare hook to compare component instances */
typedef int (*ecs_cmp_t)(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info);

/** Equals operator hook */
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

/** Specify read/write access for term */
typedef enum ecs_inout_kind_t {
    EcsInOutDefault,  /**< InOut for regular terms, In for shared terms */
    EcsInOutNone,     /**< Term is neither read nor written */
    EcsInOutFilter,   /**< Same as InOutNone + prevents term from triggering observers */
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

/** Specify cache policy for query */
typedef enum ecs_query_cache_kind_t {
    EcsQueryCacheDefault,   /**< Behavior determined by query creation context */
    EcsQueryCacheAuto,      /**< Cache query terms that are cacheable */
    EcsQueryCacheAll,       /**< Require that all query terms can be cached */
    EcsQueryCacheNone,      /**< No caching */
} ecs_query_cache_kind_t;

/* Term id flags  */

/** Match on self.
 * Can be combined with other term flags on the ecs_term_t::flags_ field.
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

/** Sort results breadth first.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsCascade                    (1llu << 60)

/** Iterate groups in descending order.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsDesc                       (1llu << 59)

/** Term id is a variable.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsIsVariable                 (1llu << 58)

/** Term id is an entity.
 * Can be combined with other term flags on the ecs_term_ref_t::id field.
 * \ingroup queries
 */
#define EcsIsEntity                   (1llu << 57)

/** Term id is a name (don't attempt to lookup as entity).
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
    ecs_entity_t id;            /**< Entity id. If left to 0 and flags does not 
                                 * specify whether id is an entity or a variable
                                 * the id will be initialized to #EcsThis.
                                 * To explicitly set the id to 0, leave the id
                                 * member to 0 and set #EcsIsEntity in flags. */

    const char *name;           /**< Name. This can be either the variable name
                                 * (when the #EcsIsVariable flag is set) or an
                                 * entity name. When ecs_term_t::move is true,
                                 * the API assumes ownership over the string and
                                 * will free it when the term is destroyed. */
} ecs_term_ref_t;

/** Type that describes a term (single element in a query). */
struct ecs_term_t {
    ecs_id_t id;                /**< Component id to be matched by term. Can be
                                 * set directly, or will be populated from the
                                 * first/second members, which provide more
                                 * flexibility. */

    ecs_term_ref_t src;          /**< Source of term */
    ecs_term_ref_t first;        /**< Component or first element of pair */
    ecs_term_ref_t second;       /**< Second element of pair */

    ecs_entity_t trav;          /**< Relationship to traverse when looking for the
                                 * component. The relationship must have
                                 * the `Traversable` property. Default is `IsA`. */

    int16_t inout;              /**< Access to contents matched by term */
    int16_t oper;               /**< Operator of term */

    int8_t field_index;         /**< Index of field for term in iterator */
    ecs_flags16_t flags_;       /**< Flags that help eval, set by ecs_query_init() */
};

/** Queries are lists of constraints (terms) that match entities. 
 * Created with ecs_query_init().
 */
struct ecs_query_t {
    ecs_header_t hdr;           /**< Object header */

    ecs_term_t *terms;          /**< Query terms */
    int32_t *sizes;             /**< Component sizes. Indexed by field */
    ecs_id_t *ids;              /**< Component ids. Indexed by field */

    uint64_t bloom_filter;      /**< Bitmask used to quickly discard tables */
    ecs_flags32_t flags;        /**< Query flags */
    int8_t var_count;           /**< Number of query variables */
    int8_t term_count;          /**< Number of query terms */
    int8_t field_count;         /**< Number of fields returned by query */

    /* Bitmasks for quick field information lookups */
    ecs_termset_t fixed_fields; /**< Fields with a fixed source */
    ecs_termset_t var_fields;   /**< Fields with non-$this variable source */
    ecs_termset_t static_id_fields; /**< Fields with a static (component) id */
    ecs_termset_t data_fields;  /**< Fields that have data */
    ecs_termset_t write_fields; /**< Fields that write data */
    ecs_termset_t read_fields;  /**< Fields that read data */
    ecs_termset_t row_fields;   /**< Fields that must be acquired with field_at */
    ecs_termset_t shared_readonly_fields; /**< Fields that don't write shared data */
    ecs_termset_t set_fields;   /**< Fields that will be set */

    ecs_query_cache_kind_t cache_kind;  /**< Caching policy of query */
    
    char **vars;                /**< Array with variable names for iterator */

    void *ctx;                  /**< User context to pass to callback */
    void *binding_ctx;          /**< Context to be used for language bindings */

    ecs_entity_t entity;        /**< Entity associated with query (optional) */
    ecs_world_t *real_world;    /**< Actual world. */
    ecs_world_t *world;         /**< World or stage query was created with. */

    int32_t eval_count;         /**< Number of times query is evaluated */
};

/** An observer reacts to events matching a query.
 * Created with ecs_observer_init().
 */
struct ecs_observer_t {
    ecs_header_t hdr;           /**< Object header */
    
    ecs_query_t *query;         /**< Observer query */

    /** Observer events */
    ecs_entity_t events[FLECS_EVENT_DESC_MAX];
    int32_t event_count;        /**< Number of events */

    ecs_iter_action_t callback; /**< See ecs_observer_desc_t::callback */
    ecs_run_action_t run;       /**< See ecs_observer_desc_t::run */

    void *ctx;                  /**< Observer context */
    void *callback_ctx;         /**< Callback language binding context */
    void *run_ctx;              /**< Run language binding context */

    ecs_ctx_free_t ctx_free;    /**< Callback to free ctx */
    ecs_ctx_free_t callback_ctx_free; /**< Callback to free callback_ctx */
    ecs_ctx_free_t run_ctx_free; /**< Callback to free run_ctx */

    ecs_observable_t *observable; /**< Observable for observer */

    ecs_world_t *world;         /**< The world */
    ecs_entity_t entity;        /**< Entity associated with observer */
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

    /** Compare hook */
    ecs_cmp_t cmp;

    /** Equals hook */
    ecs_equals_t equals;

    /** Hook flags.
     * Indicates which hooks are set for the type, and which hooks are illegal.
     * When an ILLEGAL flag is set when calling ecs_set_hooks() a hook callback
     * will be set that panics when called. */
    ecs_flags32_t flags;
    

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

    /** Callback that is invoked with the existing and new value before the
     * value is assigned. Invoked after on_add and before on_set. Registering
     * an on_replace hook prevents using operations that return a mutable 
     * pointer to the component like get_mut, ensure and emplace. */
    ecs_iter_action_t on_replace;

    void *ctx;                         /**< User defined context */
    void *binding_ctx;                 /**< Language binding context */
    void *lifecycle_ctx;               /**< Component lifecycle context (see meta add-on)*/

    ecs_ctx_free_t ctx_free;           /**< Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free;   /**< Callback to free binding_ctx */
    ecs_ctx_free_t lifecycle_ctx_free; /**< Callback to free lifecycle_ctx */
};

/** Type that contains component information (passed to ctors/dtors/...)
 *
 * @ingroup components
 */
struct ecs_type_info_t {
    ecs_size_t size;         /**< Size of type */
    ecs_size_t alignment;    /**< Alignment of type */
    ecs_type_hooks_t hooks;  /**< Type hooks */
    ecs_entity_t component;  /**< Handle to component (do not set) */
    const char *name;        /**< Type name. */
};

#include "flecs/private/api_types.h"        /* Supporting API types */
#include "flecs/private/api_support.h"      /* Supporting API functions */
#include "flecs/datastructures/hashmap.h"   /* Hashmap */
#include "flecs/private/api_internals.h"    /* Supporting API functions */

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

    ecs_entity_t id;      /**< Set to modify existing entity (optional) */

    ecs_entity_t parent;  /**< Parent entity. */

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

    /** 0-terminated array of ids to add to the entity. */
    const ecs_id_t *add;

    /** 0-terminated array of values to set on the entity. */
    const ecs_value_t *set;

    /** String expression with components to add */
    const char *add_expr;
} ecs_entity_desc_t;

/** Used with ecs_bulk_init().
 *
 * @ingroup entities
 */
typedef struct ecs_bulk_desc_t {
    int32_t _canary;        /**< Used for validity testing. Must be 0. */

    ecs_entity_t *entities; /**< Entities to bulk insert. Entity ids provided by
                             * the application must be empty (cannot
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

/** Used with ecs_component_init().
 *
 * @ingroup components
 */
typedef struct ecs_component_desc_t {
    int32_t _canary;        /**< Used for validity testing. Must be 0. */

    /** Existing entity to associate with observer (optional) */
    ecs_entity_t entity;

    /** Parameters for type (size, hooks, ...) */
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
 * An iterator contains resources that need to be released. By default this 
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
    ecs_world_t *world;           /**< The world. Can point to stage when in deferred/readonly mode. */
    ecs_world_t *real_world;      /**< Actual world. Never points to a stage. */

    /* Matched data */
    int32_t offset;               /**< Offset relative to current table */
    int32_t count;                /**< Number of entities to iterate */
    const ecs_entity_t *entities; /**< Entity identifiers */
    void **ptrs;                  /**< Component pointers. If not set or if it's NULL for a field, use it.trs. */
    const ecs_table_record_t **trs; /**< Info on where to find field in table */
    const ecs_size_t *sizes;      /**< Component sizes */
    ecs_table_t *table;           /**< Current table */
    ecs_table_t *other_table;     /**< Prev or next table when adding/removing */
    ecs_id_t *ids;                /**< (Component) ids */
    ecs_entity_t *sources;        /**< Entity on which the id was matched (0 if same as entities) */
    ecs_flags64_t constrained_vars; /**< Bitset that marks constrained variables */
    ecs_termset_t set_fields;     /**< Fields that are set */
    ecs_termset_t ref_fields;     /**< Bitset with fields that aren't component arrays */
    ecs_termset_t row_fields;     /**< Fields that must be obtained with field_at */
    ecs_termset_t up_fields;      /**< Bitset with fields matched through up traversal */

    /* Input information */
    ecs_entity_t system;          /**< The system (if applicable) */
    ecs_entity_t event;           /**< The event (if applicable) */
    ecs_id_t event_id;            /**< The (component) id for the event */
    int32_t event_cur;            /**< Unique event id. Used to dedup observer calls */

    /* Query information */
    int8_t field_count;           /**< Number of fields in iterator */
    int8_t term_index;            /**< Index of term that emitted an event.
                                   * This field will be set to the 'index' field
                                   * of an observer term. */
    const ecs_query_t *query;     /**< Query being evaluated */

    /* Context */
    void *param;                  /**< Param passed to ecs_run */
    void *ctx;                    /**< System context */
    void *binding_ctx;            /**< System binding context */
    void *callback_ctx;           /**< Callback language binding context */
    void *run_ctx;                /**< Run language binding context */

    /* Time */
    ecs_ftime_t delta_time;       /**< Time elapsed since last frame */
    ecs_ftime_t delta_system_time;/**< Time elapsed since last system invocation */

    /* Iterator counters */
    int32_t frame_offset;         /**< Offset relative to start of iteration */

    /* Misc */
    ecs_flags32_t flags;          /**< Iterator flags */
    ecs_entity_t interrupted_by;  /**< When set, system execution is interrupted */
    ecs_iter_private_t priv_;     /**< Private data */

    /* Chained iterators */
    ecs_iter_next_action_t next;  /**< Function to progress iterator */
    ecs_iter_action_t callback;   /**< Callback of system or observer */
    ecs_iter_fini_action_t fini;  /**< Function to cleanup iterator resources */
    ecs_iter_t *chain_it;         /**< Optional, allows for creating iterator chains */
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

/** Query only returns whole tables (ignores toggle/member fields).
 * Can be combined with other query flags on the ecs_query_desc_t::flags field.
 * \ingroup queries
 */
#define EcsQueryTableOnly             (1u << 7u)

/** Enable change detection for query.
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


/** Used with ecs_query_init().
 * 
 * \ingroup queries
 */
typedef struct ecs_query_desc_t {
    /** Used for validity testing. Must be 0. */
    int32_t _canary;

    /** Query terms */
    ecs_term_t terms[FLECS_TERM_COUNT_MAX];

    /** Query DSL expression (optional) */
    const char *expr;

    /** Caching policy of query */
    ecs_query_cache_kind_t cache_kind;

    /** Flags for enabling query features */
    ecs_flags32_t flags;

    /** Callback used for ordering query results. If order_by_id is 0, the
     * pointer provided to the callback will be NULL. If the callback is not
     * set, results will not be ordered. */
    ecs_order_by_action_t order_by_callback;

    /** Callback used for ordering query results. Same as order_by_callback,
     * but more efficient. */
    ecs_sort_table_action_t order_by_table_callback;

    /** Component to sort on, used together with order_by_callback or
     * order_by_table_callback. */
    ecs_entity_t order_by;

    /** Component id to be used for grouping. Used together with the
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

    /** Context to pass to group_by */
    void *group_by_ctx;

    /** Function to free group_by_ctx */
    ecs_ctx_free_t group_by_ctx_free;

    /** User context to pass to callback */
    void *ctx;

    /** Context to be used for language bindings */
    void *binding_ctx;

    /** Callback to free ctx */
    ecs_ctx_free_t ctx_free;

    /** Callback to free binding_ctx */
    ecs_ctx_free_t binding_ctx_free;

    /** Entity associated with query (optional) */
    ecs_entity_t entity;
} ecs_query_desc_t;

/** Used with ecs_observer_init().
 *
 * @ingroup observers
 */
typedef struct ecs_observer_desc_t {
    /** Used for validity testing. Must be 0. */
    int32_t _canary;

    /** Existing entity to associate with observer (optional) */
    ecs_entity_t entity;

    /** Query for observer */
    ecs_query_desc_t query;

    /** Events to observe (OnAdd, OnRemove, OnSet) */
    ecs_entity_t events[FLECS_EVENT_DESC_MAX];

    /** When observer is created, generate events from existing data. For example,
     * #EcsOnAdd `Position` would match all existing instances of `Position`. */
    bool yield_existing;

    /** Callback to invoke on an event, invoked when the observer matches. */
    ecs_iter_action_t callback;

    /** Callback invoked on an event. When left to NULL the default runner
     * is used which matches the event with the observer's query, and calls
     * 'callback' when it matches.
     * A reason to override the run function is to improve performance, if there
     * are more efficient way to test whether an event matches the observer than
     * the general purpose query matcher. */
    ecs_run_action_t run;

    /** User context to pass to callback */
    void *ctx;

    /** Callback to free ctx */
    ecs_ctx_free_t ctx_free;

    /** Context associated with callback (for language bindings). */
    void *callback_ctx;

    /** Callback to free callback ctx. */
    ecs_ctx_free_t callback_ctx_free;

    /** Context associated with run (for language bindings). */
    void *run_ctx;

    /** Callback to free run ctx. */
    ecs_ctx_free_t run_ctx_free;

    /** Observable with which to register the observer */
    ecs_poly_t *observable;

    /** Optional shared last event id for multiple observers. Ensures only one
     * of the observers with the shared id gets triggered for an event */
    int32_t *last_event_id;

    /** Used for internal purposes */
    int8_t term_index_;
    ecs_flags32_t flags_;
} ecs_observer_desc_t;

/** Used with ecs_emit().
 *
 * @ingroup observers
 */
typedef struct ecs_event_desc_t {
    /** The event id. Only observers for the specified event will be notified */
    ecs_entity_t event;

    /** Component ids. Only observers with a matching component id will be
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
     * automatically determined by doing `ecs_table_count(table) - offset`. */
    int32_t count;

    /** Single-entity alternative to setting table / offset / count */
    ecs_entity_t entity;

    /** Optional context.
     * The type of the param must be the event, where the event is a component.
     * When an event is enqueued, the value of param is coped to a temporary
     * storage of the event type. */
    void *param;

    /** Same as param, but with the guarantee that the value won't be modified.
     * When an event with a const parameter is enqueued, the value of the param
     * is copied to a temporary storage of the event type. */
    const void *const_param;

    /** Observable (usually the world) */
    ecs_poly_t *observable;

    /** Event flags */
    ecs_flags32_t flags;
} ecs_event_desc_t;


/**
 * @defgroup misc_types Miscellaneous types
 * Types used to create entities, observers, queries and more.
 *
 * @{
 */

/** Type with information about the current Flecs build */
typedef struct ecs_build_info_t {
    const char *compiler;           /**< Compiler used to compile flecs */
    const char **addons;            /**< Addons included in build */
    const char *version;            /**< Stringified version */
    int16_t version_major;          /**< Major flecs version */
    int16_t version_minor;          /**< Minor flecs version */
    int16_t version_patch;          /**< Patch flecs version */
    bool debug;                     /**< Is this a debug build */
    bool sanitize;                  /**< Is this a sanitize build */
    bool perf_trace;                /**< Is this a perf tracing build */
} ecs_build_info_t;

/** Type that contains information about the world. */
typedef struct ecs_world_info_t {
    ecs_entity_t last_component_id;   /**< Last issued component entity id */
    ecs_entity_t min_id;              /**< First allowed entity id */
    ecs_entity_t max_id;              /**< Last allowed entity id */

    ecs_ftime_t delta_time_raw;       /**< Raw delta time (no time scaling) */
    ecs_ftime_t delta_time;           /**< Time passed to or computed by ecs_progress() */
    ecs_ftime_t time_scale;           /**< Time scale applied to delta_time */
    ecs_ftime_t target_fps;           /**< Target fps */
    ecs_ftime_t frame_time_total;     /**< Total time spent processing a frame */
    ecs_ftime_t system_time_total;    /**< Total time spent in systems */
    ecs_ftime_t emit_time_total;      /**< Total time spent notifying observers */
    ecs_ftime_t merge_time_total;     /**< Total time spent in merges */
    ecs_ftime_t rematch_time_total;   /**< Time spent on query rematching */
    double world_time_total;          /**< Time elapsed in simulation */
    double world_time_total_raw;      /**< Time elapsed in simulation (no scaling) */

    int64_t frame_count_total;        /**< Total number of frames */
    int64_t merge_count_total;        /**< Total number of merges */
    int64_t eval_comp_monitors_total; /**< Total number of monitor evaluations */
    int64_t rematch_count_total;      /**< Total number of rematches */

    int64_t id_create_total;          /**< Total number of times a new id was created */
    int64_t id_delete_total;          /**< Total number of times an id was deleted */
    int64_t table_create_total;       /**< Total number of times a table was created */
    int64_t table_delete_total;       /**< Total number of times a table was deleted */
    int64_t pipeline_build_count_total; /**< Total number of pipeline builds */
    int64_t systems_ran_frame;        /**< Total number of systems ran in last frame */
    int64_t observers_ran_frame;      /**< Total number of times observer was invoked */

    int32_t tag_id_count;             /**< Number of tag (no data) ids in the world */
    int32_t component_id_count;       /**< Number of component (data) ids in the world */
    int32_t pair_id_count;            /**< Number of pair ids in the world */

    int32_t table_count;              /**< Number of tables */

    /* -- Command counts -- */
    struct {
        int64_t add_count;             /**< Add commands processed */
        int64_t remove_count;          /**< Remove commands processed */
        int64_t delete_count;          /**< Selete commands processed */
        int64_t clear_count;           /**< Clear commands processed */
        int64_t set_count;             /**< Set commands processed */
        int64_t ensure_count;          /**< Ensure/emplace commands processed */
        int64_t modified_count;        /**< Modified commands processed */
        int64_t discard_count;         /**< Commands discarded, happens when entity is no longer alive when running the command */
        int64_t event_count;           /**< Enqueued custom events */
        int64_t other_count;           /**< Other commands processed */
        int64_t batched_entity_count;  /**< Entities for which commands were batched */
        int64_t batched_command_count; /**< Commands batched */
    } cmd;                             /**< Command statistics. */

    const char *name_prefix;          /**< Value set by ecs_set_name_prefix(). Used
                                       * to remove library prefixes of symbol
                                       * names (such as `Ecs`, `ecs_`) when
                                       * registering them as names. */
} ecs_world_info_t;

/** Type that contains information about a query group. */
typedef struct ecs_query_group_info_t {
    uint64_t id;
    int32_t match_count;  /**< How often tables have been matched/unmatched */
    int32_t table_count;  /**< Number of tables in group */
    void *ctx;            /**< Group context, returned by on_group_create */
} ecs_query_group_info_t;

/** @} */

/**
 * @defgroup builtin_components Builtin component types.
 * Types that represent builtin components.
 *
 * @{
 */

/** A (string) identifier. Used as pair with #EcsName and #EcsSymbol tags */
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

/** When added to an entity this informs serialization formats which component 
 * to use when a value is assigned to an entity without specifying the 
 * component. This is intended as a hint, serialization formats are not required 
 * to use it. Adding this component does not change the behavior of core ECS 
 * operations. */
typedef struct EcsDefaultChildComponent {
    ecs_id_t component;  /**< Default component id. */
} EcsDefaultChildComponent;

/** @} */
/** @} */

/* Only include deprecated definitions if deprecated addon is required */
#ifdef FLECS_DEPRECATED
#include "flecs/addons/deprecated.h"
#endif

/**
 * @defgroup api_constants API Constants
 * Public API constants.
 *
 * @{
 */

/**
 * @defgroup id_flags Component id flags.
 * Id flags are bits that can be set on an id (ecs_id_t).
 *
 * @{
 */

/** Indicates that the id is a pair. */
FLECS_API extern const ecs_id_t ECS_PAIR;

/** Automatically override component when it is inherited */
FLECS_API extern const ecs_id_t ECS_AUTO_OVERRIDE;

/** Adds bitset to storage which allows component to be enabled/disabled */
FLECS_API extern const ecs_id_t ECS_TOGGLE;

/** @} */

/**
 * @defgroup builtin_tags Builtin component ids.
 * @{
 */

/* Builtin component ids */

/** Component component id. */
FLECS_API extern const ecs_entity_t ecs_id(EcsComponent);

/** Identifier component id. */
FLECS_API extern const ecs_entity_t ecs_id(EcsIdentifier);

/** Poly component id. */
FLECS_API extern const ecs_entity_t ecs_id(EcsPoly);

/** DefaultChildComponent component id. */
FLECS_API extern const ecs_entity_t ecs_id(EcsDefaultChildComponent);

/** Tag added to queries. */
FLECS_API extern const ecs_entity_t EcsQuery;

/** Tag added to observers. */
FLECS_API extern const ecs_entity_t EcsObserver;

/** Tag added to systems. */
FLECS_API extern const ecs_entity_t EcsSystem;

/** TickSource component id. */
FLECS_API extern const ecs_entity_t ecs_id(EcsTickSource);

/** Pipeline module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsPipelineQuery);

/** Timer component id. */
FLECS_API extern const ecs_entity_t ecs_id(EcsTimer);

/** RateFilter component id. */
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
 *
 * @code
 *   if R(X, Y) and R(Y, Z) then R(X, Z)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsTransitive;

/** Marks a relationship as reflexive.
 * Behavior:
 *
 * @code
 *   R(X, X) == true
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsReflexive;

/** Ensures that entity/component cannot be used as target in `IsA` relationship.
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
 * relationships are added with the component as target.
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

/** Marks relationship as commutative.
 * Behavior:
 *
 * @code
 *   if R(X, Y) then R(Y, X)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsSymmetric;

/** Can be added to relationship to indicate that the relationship can only occur
 * once on an entity. Adding a 2nd instance will replace the 1st.
 *
 * Behavior:
 *
 * @code
 *   R(X, Y) + R(X, Z) = R(X, Z)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsExclusive;

/** Marks a relationship as acyclic. Acyclic relationships may not form cycles. */
FLECS_API extern const ecs_entity_t EcsAcyclic;

/** Marks a relationship as traversable. Traversable relationships may be
 * traversed with "up" queries. Traversable relationships are acyclic. */
FLECS_API extern const ecs_entity_t EcsTraversable;

/** Ensure that a component always is added together with another component.
 *
 * Behavior:
 *
 * @code
 *   If With(R, O) and R(X) then O(X)
 *   If With(R, O) and R(X, Y) then O(X, Y)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsWith;

/** Ensure that relationship target is child of specified entity.
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

/** Ensure that an entity is always used in pair as relationship.
 *
 * Behavior:
 *
 * @code
 *   e.add(R) panics
 *   e.add(X, R) panics, unless X has the "Trait" trait
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsRelationship;

/** Ensure that an entity is always used in pair as target.
 *
 * Behavior:
 *
 * @code
 *   e.add(T) panics
 *   e.add(T, X) panics
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsTarget;

/** Can be added to relationship to indicate that it should never hold data, 
 * even when it or the relationship target is a component. */
FLECS_API extern const ecs_entity_t EcsPairIsTag;

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

/** Tag that when added to a parent ensures stable order of ecs_children result. */
FLECS_API extern const ecs_entity_t EcsOrderedChildren;

/** Tag added to module entities */
FLECS_API extern const ecs_entity_t EcsModule;

/** Tag to indicate an entity/component/system is private to a module */
FLECS_API extern const ecs_entity_t EcsPrivate;

/** Tag added to prefab entities. Any entity with this tag is automatically
 * ignored by queries, unless #EcsPrefab is explicitly queried for. */
FLECS_API extern const ecs_entity_t EcsPrefab;

/** When this tag is added to an entity it is skipped by queries, unless
 * #EcsDisabled is explicitly queried for. */
FLECS_API extern const ecs_entity_t EcsDisabled;

/** Trait added to entities that should never be returned by queries. Reserved
 * for internal entities that have special meaning to the query engine, such as
 * #EcsThis, #EcsWildcard, #EcsAny. */
FLECS_API extern const ecs_entity_t EcsNotQueryable;

/** Event that triggers when an id is added to an entity */
FLECS_API extern const ecs_entity_t EcsOnAdd;

/** Event that triggers when an id is removed from an entity */
FLECS_API extern const ecs_entity_t EcsOnRemove;

/** Event that triggers when a component is set for an entity */
FLECS_API extern const ecs_entity_t EcsOnSet;

/** Event that triggers observer when an entity starts/stops matching a query */
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

/** Remove cleanup policy. Must be used as target in pair with #EcsOnDelete or
 * #EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsRemove;

/** Delete cleanup policy. Must be used as target in pair with #EcsOnDelete or
 * #EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsDelete;

/** Panic cleanup policy. Must be used as target in pair with #EcsOnDelete or
 * #EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsPanic;

/** Mark component as singleton. Singleton components may only be added to 
 * themselves. */
FLECS_API extern const ecs_entity_t EcsSingleton;

/** Mark component as sparse */
FLECS_API extern const ecs_entity_t EcsSparse;

/** Mark component as non-fragmenting */
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

/** Tag used to indicate query is empty.
 * This tag is removed automatically when a query becomes non-empty, and is not
 * automatically re-added when it becomes empty.
 */
FLECS_API extern const ecs_entity_t EcsEmpty;

FLECS_API extern const ecs_entity_t ecs_id(EcsPipeline); /**< Pipeline component id. */
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

FLECS_API extern const ecs_entity_t EcsConstant;    /**< Tag added to enum/bitmask constants. */

/** Value used to quickly check if component is builtin. This is used to quickly
 * filter out tables with builtin components (for example for ecs_delete()) */
#define EcsLastInternalComponentId (ecs_id(EcsPoly))

/** The first user-defined component starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserComponentId (8)

/** The first user-defined entity starts from this id. Ids up to this number
 * are reserved for builtin entities */
#define EcsFirstUserEntityId (FLECS_HI_COMPONENT_ID + 128)

/* When visualized the reserved id ranges look like this:
 * - [1..8]: Builtin components
 * - [9..FLECS_HI_COMPONENT_ID]: Low ids reserved for application components
 * - [FLECS_HI_COMPONENT_ID + 1..EcsFirstUserEntityId]: Builtin entities
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
 * @return A new world
 */
FLECS_API
ecs_world_t* ecs_init(void);

/** Create a new world with just the core module.
 * Same as ecs_init(), but doesn't import modules from addons. This operation is
 * faster than ecs_init() and results in less memory utilization.
 *
 * @return A new tiny world
 */
FLECS_API
ecs_world_t* ecs_mini(void);

/** Create a new world with arguments.
 * Same as ecs_init(), but allows passing in command line arguments. Command line
 * arguments are used to:
 * - automatically derive the name of the application from argv[0]
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
 * This operation can be used in callbacks like type hooks or observers to
 * detect if they are invoked while the world is being deleted.
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

/** Type returned by ecs_get_entities(). */
typedef struct ecs_entities_t {
    const ecs_entity_t *ids; /**< Array with all entity ids in the world. */
    int32_t count;           /**< Total number of entity ids. */
    int32_t alive_count;     /**< Number of alive entity ids. */
} ecs_entities_t;

/** Return entity identifiers in world.
 * This operation returns an array with all entity ids that exist in the world.
 * Note that the returned array will change and may get invalidated as a result
 * of entity creation & deletion.
 * 
 * To iterate all alive entity ids, do:
 * @code
 * ecs_entities_t entities = ecs_get_entities(world);
 * for (int i = 0; i < entities.alive_count; i ++) {
 *   ecs_entity_t id = entities.ids[i];
 * }
 * @endcode
 * 
 * To iterate not-alive ids, do:
 * @code
 * for (int i = entities.alive_count + 1; i < entities.count; i ++) {
 *   ecs_entity_t id = entities.ids[i];
 * }
 * @endcode
 * 
 * The returned array does not need to be freed. Mutating the returned array
 * will return in undefined behavior (and likely crashes).
 * 
 * @param world The world.
 * @return Struct with entity id array.
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
 * ecs_frame_begin().
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

/** Set target frames per second (FPS) for application.
 * Setting the target FPS ensures that ecs_progress() is not invoked faster than
 * the specified FPS. When enabled, ecs_progress() tracks the time passed since
 * the last invocation, and sleeps the remaining time of the frame (if any).
 *
 * This feature ensures systems are ran at a consistent interval, as well as
 * conserving CPU time by not running systems more often than required.
 *
 * Note that ecs_progress() only sleeps if there is time left in the frame. Both
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

/** Set default query flags. 
 * Set a default value for the ecs_filter_desc_t::flags field. Default flags
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
 * optimizations that certain aspects of the world to not change, while also 
 * providing a mechanism for applications to prevent accidental mutations in, 
 * for example, multithreaded applications.
 * 
 * Readonly mode is a stronger version of deferred mode. In deferred mode
 * ECS operations such as add/remove/set/delete etc. are added to a command 
 * queue to be executed later. In readonly mode, operations that could break
 * scheduler logic (such as creating systems, queries) are also disallowed.
 * 
 * Readonly mode itself has a single threaded and a multi threaded mode. In
 * single threaded mode certain mutations on the world are still allowed, for 
 * example:
 * - Entity liveliness operations (such as new, make_alive), so that systems are
 *   able to create new entities.
 * - Implicit component registration, so that this works from systems
 * - Mutations to supporting data structures for the evaluation of uncached 
 *   queries (filters), so that these can be created on the fly.
 * 
 * These mutations are safe in a single threaded applications, but for
 * multithreaded applications the world needs to be entirely immutable. For this
 * purpose multi threaded readonly mode exists, which disallows all mutations on
 * the world. This means that in multi threaded applications, entity liveliness
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
 * ecs_stage_t *stage = ecs_get_stage(world, 1);
 *
 * ecs_readonly_begin(world);
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
 * exclusive access to the world. The functions themselves are not thread safe.
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
 *@endcode
 *
 * @param world The world
 * @param multi_threaded Whether to enable readonly/multi threaded mode.
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
 * @param world The world
 */
FLECS_API
void ecs_readonly_end(
    ecs_world_t *world);

/** Merge world or stage.
 * When automatic merging is disabled, an application can call this
 * operation on either an individual stage, or on the world which will merge
 * all stages. This operation may only be called when staging is not enabled
 * (either after ecs_progress() or after ecs_readonly_end()).
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
 * ecs_defer_begin() and ecs_defer_end() operations are executed at the end
 * of the frame.
 *
 * This operation is thread safe.
 *
 * @param world The world.
 * @return true if world changed from non-deferred mode to deferred mode.
 *
 * @see ecs_defer_end()
 * @see ecs_is_deferred()
 * @see ecs_defer_resume()
 * @see ecs_defer_suspend()
 */
FLECS_API
bool ecs_defer_begin(
    ecs_world_t *world);

/** Test if deferring is enabled for current stage.
 *
 * @param world The world.
 * @return True if deferred, false if not.
 *
 * @see ecs_defer_begin()
 * @see ecs_defer_end()
 * @see ecs_defer_resume()
 * @see ecs_defer_suspend()
 */
FLECS_API
bool ecs_is_deferred(
    const ecs_world_t *world);

/** End block of operations to defer.
 * See ecs_defer_begin().
 *
 * This operation is thread safe.
 *
 * @param world The world.
 * @return true if world changed from deferred mode to non-deferred mode.
 *
 * @see ecs_defer_begin()
 * @see ecs_defer_is_deferred()
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
 * @see ecs_defer_is_deferred()
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
 * @see ecs_defer_is_deferred()
 * @see ecs_defer_suspend()
 */
FLECS_API
void ecs_defer_resume(
    ecs_world_t *world);

/** Configure world to have N stages.
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

/** Get number of configured stages.
 * Return number of stages set by ecs_set_stage_count().
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
 * Note that this function does not(!) create a new world. It simply wraps the
 * existing world in a thread-specific context, which the API knows how to
 * unwrap. The reason the stage is returned as an ecs_world_t is so that it
 * can be passed transparently to the existing API functions, vs. having to
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

/** Create unmanaged stage.
 * Create a stage whose lifecycle is not managed by the world. Must be freed
 * with ecs_stage_free().
 *
 * @param world The world.
 * @return The stage.
 */
FLECS_API
ecs_world_t* ecs_stage_new(
    ecs_world_t *world);

/** Free unmanaged stage.
 *
 * @param stage The stage to free.
 */
FLECS_API
void ecs_stage_free(
    ecs_world_t *stage);

/** Get stage id.
 * The stage id can be used by an application to learn about which stage it is
 * using, which typically corresponds with the worker thread id.
 *
 * @param world The world.
 * @return The stage id.
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
 * @param ctx A pointer to a user defined structure.
 * @param ctx_free A function that is invoked with ctx when the world is freed.
 */
FLECS_API
void ecs_set_ctx(
    ecs_world_t *world,
    void *ctx,
    ecs_ctx_free_t ctx_free);

/** Set a world binding context.
 * Same as ecs_set_ctx() but for binding context. A binding context is intended
 * specifically for language bindings to store binding specific data.
 *
 * @param world The world.
 * @param ctx A pointer to a user defined structure.
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
 *  Returns information about the current Flecs build.
 * 
 * @return A struct with information about the current Flecs build.
 */
FLECS_API
const ecs_build_info_t* ecs_get_build_info(void);

/** Get world info.
 *
 * @param world The world.
 * @return Pointer to the world info. Valid for as long as the world exists.
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

/** Set a range for issuing new entity ids.
 * This function constrains the entity identifiers returned by ecs_new_w() to the
 * specified range. This operation can be used to ensure that multiple processes
 * can run in the same simulation without requiring a central service that
 * coordinates issuing identifiers.
 *
 * If `id_end` is set to 0, the range is infinite. If `id_end` is set to a non-zero
 * value, it has to be larger than `id_start`. If `id_end` is set and ecs_new() is
 * invoked after an id is issued that is equal to `id_end`, the application will
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
 * @return The largest issued entity id.
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

/** Used with ecs_delete_empty_tables(). */
typedef struct ecs_delete_empty_tables_desc_t {
    /** Free table data when generation > clear_generation. */
    uint16_t clear_generation;

    /** Delete table when generation > delete_generation. */
    uint16_t delete_generation;

    /** Amount of time operation is allowed to spend. */
    double time_budget_seconds;
} ecs_delete_empty_tables_desc_t;

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
 * @param desc Configuration parameters.
 * @return Number of deleted tables.
 */
FLECS_API
int32_t ecs_delete_empty_tables(
    ecs_world_t *world,
    const ecs_delete_empty_tables_desc_t *desc);

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
 *
 * @code
 * flecs_poly_is(ptr, ecs_world_t)
 * @endcode
 *
 * This operation only works for poly types.
 *
 * @param object The object to test.
 * @param type The id of the type.
 * @return True if the pointer is of the specified type.
 */
FLECS_API
bool flecs_poly_is_(
    const ecs_poly_t *object,
    int32_t type);

/** Test if pointer is of specified type.
 * @see flecs_poly_is_()
 */
#define flecs_poly_is(object, type)\
    flecs_poly_is_(object, type##_magic)

/** Make a pair id.
 * This function is equivalent to using the ecs_pair() macro, and is added for
 * convenience to make it easier for non C/C++ bindings to work with pairs.
 *
 * @param first The first element of the pair of the pair.
 * @param second The target of the pair.
 * @return A pair id.
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
 * function does not(!) copy the thread name, which means the memory for the 
 * name must remain alive for as long as the thread has exclusive access.
 * 
 * This operation should only be called once per thread. Calling it multiple 
 * times for the same thread will cause a panic.
 * 
 * Note that this feature only works in builds where asserts are enabled. The
 * feature requires the OS API thread_self_ callback to be set.
 * 
 * @param world The world.
 * @param thread_name Name of the thread obtaining exclusive access.
 */
FLECS_API
void ecs_exclusive_access_begin(
    ecs_world_t *world,
    const char *thread_name);

/** End exclusive thread access.
 * This operation should be called after ecs_exclusive_access_begin(). After
 * calling this operation other threads are no longer prevented from mutating
 * the world.
 * 
 * When "lock_world" is set to true, no thread will be able to mutate the world
 * until ecs_exclusive_access_begin is called again. While the world is locked
 * only readonly operations are allowed. For example, ecs_get_id() is allowed,
 * but ecs_get_mut_id() is not allowed.
 * 
 * A locked world can be unlocked by calling ecs_exclusive_access_end again with
 * lock_world set to false. Note that this only works for locked worlds, if\
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

/** Create new entity id.
 * This operation returns an unused entity id. This operation is guaranteed to
 * return an empty entity as it does not use values set by ecs_set_scope() or
 * ecs_set_with().
 *
 * @param world The world.
 * @return The new entity id.
 */
FLECS_API
ecs_entity_t ecs_new(
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
 * values set by ecs_set_scope() or ecs_set_with().
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
 * is passed to the id parameter, no component is added to the new entity.
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
 * points to an internal data structure which changes when new entities are
 * created/deleted.
 *
 * If as a result of the operation triggers are invoked that deletes
 * entities and no entity ids were provided by the application, the returned
 * array of identifiers may be incorrect. To avoid this problem, an application
 * can first call ecs_bulk_init() to create empty entities, copy the array to one
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
 * This operation is the same as ecs_new_w_id(), but creates N entities
 * instead of one.
 *
 * @param world The world.
 * @param component The component to create the entities with.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
FLECS_API
const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t component,
    int32_t count);

/** Clone an entity
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
 * This operation will delete an entity and all of its components. The entity id
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
 * This will delete all entities (tables) that have the specified id. The 
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
 * operation is called from a multithreaded system it will fail.
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
 * that does not have the OrderedChildren trait, it will fail.asm
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

/** Add a (component) id to an entity.
 * This operation adds a single (component) id to an entity. If the entity
 * already has the id, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component id to add.
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

/** Add auto override for component.
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
 * This operation is equivalent to manually adding the id with the AUTO_OVERRIDE
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
 * When a component has the `(OnInstantiate, DontInherit)` trait and is overridden
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
 * This will remove the specified id from all entities (tables). The id may be
 * a wildcard and/or a pair.
 *
 * @param world The world.
 * @param component The component.
 */
FLECS_API
void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t component);

/** Create new entities with specified component.
 * Entities created with ecs_entity_init() will be created with the specified
 * component. This does not apply to entities created with ecs_new().
 * 
 * Only one component can be specified at a time. If this operation is called 
 * while a component is already configured, the new component will override the
 * old component.
 *
 * @param world The world.
 * @param component The component.
 * @return The previously set component.
 * @see ecs_entity_init()
 * @see ecs_set_with()
 */
FLECS_API
ecs_entity_t ecs_set_with(
    ecs_world_t *world,
    ecs_id_t component);

/** Get component set with ecs_set_with().
 * Get the component set with ecs_set_with().
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

/** Enable or disable entity.
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
 * @param component The component to enable/disable.
 * @param enable True to enable the component, false to disable.
 */
FLECS_API
void ecs_enable_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    bool enable);

/** Test if component is enabled.
 * Test whether a component is currently enabled or disabled. This operation
 * will return true when the entity has the component and if it has not been
 * disabled by ecs_enable_component().
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
 * @defgroup getting Getting & Setting
 * Functions for getting/setting components.
 *
 * @{
 */

/** Get an immutable pointer to a component.
 * This operation obtains a const pointer to the requested component. The
 * operation accepts the component entity id.
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
 * operation accepts the component entity id.
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

/** Ensure entity has component, return pointer.
 * This operation returns a mutable pointer to a component. If the entity did
 * not yet have the component, it will be added.
 *
 * If ensure is called when the world is in deferred/readonly mode, the
 * function will:
 * - return a pointer to a temp storage if the component does not yet exist, or
 * - return a pointer to the existing component if it exists
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to get/add.
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
 * A ref is a handle to an entity + component which caches a small amount of
 * data to reduce overhead of repeatedly accessing the component. Use
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

/** Get component from ref.
 * Get component pointer from ref. The ref must be created with ecs_ref_init().
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

/** Update ref.
 * Ensures contents of ref are up to date. Same as ecs_ref_get_id(), but does not
 * return pointer to component id.
 *
 * @param world The world.
 * @param ref The ref.
 */
FLECS_ALWAYS_INLINE FLECS_API
void ecs_ref_update(
    const ecs_world_t *world,
    ecs_ref_t *ref);

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
 * @param component The component to get/add.
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
 * ecs_modified_id(). The operation will not modify the value of the passed in
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
 * This operation tests whether the entity id:
 * - is not 0
 * - has a valid bit pattern
 * - is alive (see ecs_is_alive())
 * 
 * If this operation returns true, it is safe to use the entity with other
 * other operations.
 * 
 * This operation should only be used if an application cannot be sure that an
 * entity is initialized with a valid value. In all other cases where an entity
 * was initialized with a valid value, but the application wants to check if the
 * entity is (still) alive, use ecs_is_alive.
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
 * deleted. Operations that return alive ids are (amongst others) ecs_new(),
 * ecs_new_low_id() and ecs_entity_init(). Ids can be made alive with the 
 * ecs_make_alive() * function.
 *
 * After an id is deleted it can be recycled. Recycled ids are different from
 * the original id in that they have a different generation count. This makes it
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
 * Other than ecs_is_valid(), this operation will panic if the passed in entity
 * id is 0 or has an invalid bit pattern.
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
 * For example, when obtaining the parent id from a `ChildOf` relationship, the parent
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
 * with ecs_new_w() (such as a global constant or an id from a remote application).
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
 *
 * @see ecs_make_alive_id()
 */
FLECS_API
void ecs_make_alive(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Same as ecs_make_alive(), but for components.
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
 * This is different from ecs_make_alive(), which will fail if attempted with an id
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
 * Similar as ecs_is_alive(), but ignores entity generation count.
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
 * and is used to test whether an entity id is alive.
 *
 * This operation overrides the current generation of an entity with the
 * specified generation, which can be useful if an entity is externally managed,
 * like for external pools, savefiles or netcode.
 * 
 * This operation is similar to ecs_make_alive(), except that it will also
 * override the generation of an alive entity.
 *
 * @param world The world.
 * @param entity Entity for which to set the generation with the new generation.
 */
FLECS_API
void ecs_set_version(
    ecs_world_t *world,
    ecs_entity_t entity);

/** @} */

/**
 * @defgroup entity_info Entity Information.
 * Get information from entity.
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
 * @return The table of the entity, NULL if the entity has no components/tags.
 */
FLECS_API
ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Convert type to string.
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

/** Convert table to string.
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

/** Convert entity to string.
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

/** Get parent (target of `ChildOf` relationship) for entity.
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

/** Get the target of a relationship for a given component.
 * This operation returns the first entity that has the provided component by 
 * following the relationship. If the entity itself has the component then it 
 * will be returned. If the component cannot be found on the entity or by 
 * following the relationship, the operation will return 0.
 *
 * This operation can be used to lookup, for example, which prefab is providing
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
 * @param component The component to lookup.
 * @return The entity for which the target has been found.
 */
FLECS_API
ecs_entity_t ecs_get_target_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t component);

/** Return depth for entity in tree for the specified relationship.
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
 * Functions for working with entity names and paths.
 *
 * @{
 */

/** Get the name of an entity.
 * This will return the name stored in `(EcsIdentifier, EcsName)`.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no name.
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
 * @return The type of the entity, NULL if the entity has no name.
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
 * The symbol is stored in (EcsIdentifier, EcsSymbol).
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

/** Set alias for entity.
 * An entity can be looked up using its alias from the root scope without
 * providing the fully qualified name if its parent. An entity can only have
 * a single alias.
 *
 * The symbol is stored in `(EcsIdentifier, EcsAlias)`.
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

/** Lookup an entity by it's path.
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

/** Lookup a child entity by name.
 * Returns an entity that matches the specified name. Only looks for entities in
 * the provided parent. If no parent is provided, look in the current scope (
 * root if no scope is provided).
 *
 * @param world The world.
 * @param parent The parent for which to lookup the child.
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

/** Lookup an entity by its symbol name.
 * This looks up an entity by symbol stored in `(EcsIdentifier, EcsSymbol)`. The
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

/** Write path identifier to buffer.
 * Same as ecs_get_path_w_sep(), but writes result to an ecs_strbuf_t.
 *
 * @param world The world.
 * @param parent The entity from which to create the path.
 * @param child The entity to which to create the path.
 * @param sep The separator to use between path elements.
 * @param prefix The initial character to use for root elements.
 * @param buf The buffer to write to.
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
 * As a result new entities will be created in this scope, and lookups will be
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
 * ecs_set_lookup_path() operation again.
 *
 * @param world The world.
 * @param lookup_path 0-terminated array with entity ids for the lookup path.
 * @return Current lookup path array.
 *
 * @see ecs_get_lookup_path()
 */
FLECS_API
ecs_entity_t* ecs_set_lookup_path(
    ecs_world_t *world,
    const ecs_entity_t *lookup_path);

/** Get current lookup path.
 * Returns value set by ecs_set_lookup_path().
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

/** Get the type info for an component.
 * This function returns the type information for a component. The component can
 * be a regular component or pair. For the rules on how type information is 
 * determined based on a component id, see ecs_get_typeid().
 *
 * @param world The world.
 * @param component The component.
 * @return The type information of the id.
 */
FLECS_API
const ecs_type_info_t* ecs_get_type_info(
    const ecs_world_t *world,
    ecs_id_t component);

/** Register hooks for component.
 * Hooks allow for the execution of user code when components are constructed,
 * copied, moved, destructed, added, removed or set. Hooks can be assigned as
 * as long as a component has not yet been used (added to an entity).
 *
 * The hooks that are currently set can be accessed with ecs_get_type_info().
 *
 * @param world The world.
 * @param component The component for which to register the actions
 * @param hooks Type that contains the component actions.
 */
FLECS_API
void ecs_set_hooks_id(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_type_hooks_t *hooks);

/** Get hooks for component.
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
 * @defgroup ids Ids
 * Functions for working with `ecs_id_t`.
 *
 * @{
 */

/** Returns whether specified component is a tag.
 * This operation returns whether the specified component is a tag (a component
 * without data/size).
 *
 * An id is a tag when:
 * - it is an entity without the EcsComponent component
 * - it has an EcsComponent with size member set to 0
 * - it is a pair where both elements are a tag
 * - it is a pair where the first element has the #EcsPairIsTag tag
 *
 * @param world The world.
 * @param component The component.
 * @return Whether the provided id is a tag.
 */
FLECS_API
bool ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t component);

/** Returns whether specified component is in use.
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
 * This operation returns the type for a component id, if the id is associated
 * with a type. For a regular component with a non-zero size (an entity with the
 * EcsComponent component) the operation will return the component id itself.
 *
 * For an entity that does not have the EcsComponent component, or with an
 * EcsComponent value with size 0, the operation will return 0.
 *
 * For a pair id the operation will return the type associated with the pair, by
 * applying the following queries in order:
 * - The first pair element is returned if it is a component
 * - 0 is returned if the relationship entity has the Tag property
 * - The second pair element is returned if it is a component
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
 * @return Whether the id matches the pattern.
 */
FLECS_API
bool ecs_id_match(
    ecs_id_t component,
    ecs_id_t pattern);

/** Utility to check if component is a pair.
 *
 * @param component The component.
 * @return True if component is a pair.
 */
FLECS_API
bool ecs_id_is_pair(
    ecs_id_t component);

/** Utility to check if component is a wildcard.
 *
 * @param component The component.
 * @return True if component is a wildcard or a pair containing a wildcard.
 */
FLECS_API
bool ecs_id_is_wildcard(
    ecs_id_t component);

/** Utility to check if component is an any wildcard.
 * 
 * @param component The component.
 * @return True if component is an any wildcard or a pair containing an any wildcard.
 */
bool ecs_id_is_any(
    ecs_id_t component);

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
 * @param component The component.
 * @return True if the id is valid.
 */
FLECS_API
bool ecs_id_is_valid(
    const ecs_world_t *world,
    ecs_id_t component);

/** Get flags associated with id.
 * This operation returns the internal flags (see api_flags.h) that are
 * associated with the provided id.
 *
 * @param world The world.
 * @param component The component.
 * @return Flags associated with the id, or 0 if the id is not in use.
 */
FLECS_API
ecs_flags32_t ecs_id_get_flags(
    const ecs_world_t *world,
    ecs_id_t component);

/** Convert component flag to string.
 * This operation converts a component flag to a string. Possible outputs are:
 * 
 * - PAIR
 * - TOGGLE
 * - AUTO_OVERRIDE
 *
 * @param component_flags The component flag.
 * @return The id flag string, or NULL if no valid id is provided.
 */
FLECS_API
const char* ecs_id_flag_str(
    ecs_id_t component_flags);

/** Convert component id to string.
 * This operation converts the provided component id to a string. It can output
 * strings of the following formats:
 * 
 * - "ComponentName"
 * - "FLAG|ComponentName"
 * - "(Relationship, Target)"
 * - "FLAG|(Relationship, Target)"
 * 
 * The PAIR flag never added to the string.
 *
 * @param world The world.
 * @param component The component to convert to a string.
 * @return The component converted to a string.
 */
FLECS_API
char* ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t component);

/** Write component string to buffer.
 * Same as ecs_id_str() but writes result to ecs_strbuf_t.
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

/** Convert string to a component.
 * This operation is the reverse of ecs_id_str(). The FLECS_SCRIPT addon
 * is required for this operation to work.
 *
 * @param world The world.
 * @param expr The string to convert to an id.
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

/** Test whether term ref is set.
 * A term ref is a reference to an entity, component or variable for one of the
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
 * when initializing a 0-initialized array of terms (like in ecs_term_desc_t) as
 * this operation can be used to find the last initialized element.
 *
 * @param term The term.
 * @return True when set, false when not set.
 */
FLECS_API
bool ecs_term_is_initialized(
    const ecs_term_t *term);

/** Is term matched on $this variable.
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
 * @return True if term matches $this, false if not.
 */
FLECS_API
bool ecs_term_match_this(
    const ecs_term_t *term);

/** Is term matched on 0 source.
 * This operation checks whether a term is matched on a 0 source. A 0 source is
 * a term that isn't matched against anything, and can be used just to pass
 * (component) ids to a query iterator.
 *
 * A term has a 0 source when:
 * - ecs_term_t::src::id is 0
 * - ecs_term_t::src::flags has EcsIsEntity set
 *
 * @param term The term.
 * @return True if term has 0 source, false if not.
 */
FLECS_API
bool ecs_term_match_0(
    const ecs_term_t *term);

/** Convert term to string expression.
 * Convert term to a string expression. The resulting expression is equivalent
 * to the same term, with the exception of And & Or operators.
 *
 * @param world The world.
 * @param term The term.
 * @return The term converted to a string.
 */
FLECS_API
char* ecs_term_str(
    const ecs_world_t *world,
    const ecs_term_t *term);

/** Convert query to string expression.
 * Convert query to a string expression. The resulting expression can be
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
 * @brief Find all entities that have a single (component) id.
 * @{
 */

/** Iterate all entities with specified (component id). 
 * This returns an iterator that yields all entities with a single specified
 * component. This is a much lighter weight operation than creating and 
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
 * If the specified id is a component, it is possible to access the component
 * pointer with ecs_field just like with regular queries:
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
 * @return An iterator that iterates all entities with the (component) id.
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

/** Iterate children of parent.
 * This operation is usually equivalent to doing:
 * @code
 * ecs_iter_t it = ecs_each_id(world, ecs_pair(EcsChildOf, parent));
 * @endcode
 * 
 * The only exception is when the parent has the EcsOrderedChildren trait, in
 * which case this operation will return a single result with the ordered 
 * child entity ids.
 * 
 * @param world The world.
 * @param parent The parent.
 * @return An iterator that iterates all children of the parent.
 *
 * @see ecs_each_id()
*/
FLECS_API
ecs_iter_t ecs_children(
    const ecs_world_t *world,
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
 * 
 * @param world The world.
 * @param desc The descriptor (see ecs_query_desc_t)
 * @return The query.
 */
FLECS_API
ecs_query_t* ecs_query_init(
    ecs_world_t *world,
    const ecs_query_desc_t *desc);

/** Delete a query.
 *
 * @param query The query.
 */
FLECS_API
void ecs_query_fini(
    ecs_query_t *query);

/** Find variable index.
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

/** Get variable name.
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

/** Test if variable is an entity.
 * Internally the query engine has entity variables and table variables. When
 * iterating through query variables (by using ecs_query_variable_count()) only
 * the values for entity variables are accessible. This operation enables an
 * application to check if a variable is an entity variable.
 *
 * @param query The query.
 * @param var_id The variable id.
 * @return Whether the variable is an entity variable.
 */
FLECS_API
bool ecs_query_var_is_entity(
    const ecs_query_t *query,
    int32_t var_id);  

/** Create a query iterator.
 * Use an iterator to iterate through the entities that match an entity. Queries
 * can return multiple results, and have to be iterated by repeatedly calling
 * ecs_query_next() until the operation returns false.
 * 
 * Depending on the query, a single result can contain an entire table, a range
 * of entities in a table, or a single entity. Iteration code has an inner and
 * an outer loop. The outer loop loops through the query results, and typically
 * corresponds with a table. The inner loop loops entities in the result.
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

/** Progress query iterator.
 *
 * @param it The iterator.
 * @return True if the iterator has more results, false if not.
 *
 * @see ecs_query_iter()
 */
FLECS_API
bool ecs_query_next(
    ecs_iter_t *it);

/** Match entity with query.
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
 * @param entity The entity to match
 * @param it The iterator with matched data.
 * @return True if entity matches the query, false if not.
 */
FLECS_API
bool ecs_query_has(
    ecs_query_t *query,
    ecs_entity_t entity,
    ecs_iter_t *it);

/** Match table with query.
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
 * @param table The table to match
 * @param it The iterator with matched data.
 * @return True if table matches the query, false if not.
 */
FLECS_API
bool ecs_query_has_table(
    ecs_query_t *query,
    ecs_table_t *table,
    ecs_iter_t *it);

/** Match range with query.
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
 * @param range The range to match
 * @param it The iterator with matched data.
 * @return True if range matches the query, false if not.
 */
FLECS_API
bool ecs_query_has_range(
    ecs_query_t *query,
    ecs_table_range_t *range,
    ecs_iter_t *it);

/** Returns how often a match event happened for a cached query. 
 * This operation can be used to determine whether the query cache has been 
 * updated with new tables.
 * 
 * @param query The query.
 * @return The number of match events happened.
 */
FLECS_API
int32_t ecs_query_match_count(
    const ecs_query_t *query);

/** Convert query to a string.
 * This will convert the query program to a string which can aid in debugging
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

/** Convert query to string with profile.
 * To use this you must set the EcsIterProfile flag on an iterator before
 * starting iteration:
 *
 * @code
 *   it.flags |= EcsIterProfile
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

/** Populate variables from key-value string.
 * Convenience function to set query variables from a key-value string separated
 * by comma's. The string must have the following format:
 *
 * @code
 *   var_a: value, var_b: value
 * @endcode
 *
 * The key-value list may optionally be enclosed in parenthesis.
 * 
 * This function uses the script addon.
 *
 * @param query The query.
 * @param it The iterator for which to set the variables.
 * @param expr The key-value expression.
 * @return Pointer to the next character after the last parsed one.
 */
FLECS_API
const char* ecs_query_args_parse(
    ecs_query_t *query,
    ecs_iter_t *it,
    const char *expr);

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
 * The changed state of a table is reset after it is iterated. If an iterator was
 * not iterated until completion, tables may still be marked as changed.
 *
 * If no iterator is provided the operation will return the changed state of the
 * all matched tables of the query.
 *
 * If an iterator is provided, the operation will return the changed state of
 * the currently returned iterator result. The following preconditions must be
 * met before using an iterator with change detection:
 *
 * - The iterator is a query iterator (created with ecs_query_iter())
 * - The iterator must be valid (ecs_query_next() must have returned true)
 *
 * @param query The query (optional if 'it' is provided).
 * @return true if entities changed, otherwise false.
 */
FLECS_API
bool ecs_query_changed(
    ecs_query_t *query);

/** Get query object.
 * Returns the query object. Can be used to access various information about
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
 * will not update the dirty flags of the table for its out columns.
 *
 * Only valid iterators must be provided (next has to be called at least once &
 * return true) and the iterator must be a query iterator.
 *
 * @param it The iterator result to skip.
 */
FLECS_API
void ecs_iter_skip(
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
 * The group to iterate must be set before the first call to ecs_query_next(). No
 * operations that can add/remove components should be invoked between calling
 * ecs_iter_set_group() and ecs_query_next().
 *
 * @param it The query iterator.
 * @param group_id The group to iterate.
 */
FLECS_API
void ecs_iter_set_group(
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

/** Struct returned by ecs_query_count(). */
typedef struct ecs_query_count_t {
    int32_t results;      /**< Number of results returned by query. */
    int32_t entities;     /**< Number of entities returned by query. */
    int32_t tables;       /**< Number of tables returned by query. Only set for
                             * queries for which the table count can be reliably
                             * determined. */
} ecs_query_count_t;

/** Returns number of entities and results the query matches with.
 * Only entities matching the $this variable as source are counted.
 *
 * @param query The query.
 * @return The number of matched entities.
 */
FLECS_API
ecs_query_count_t ecs_query_count(
    const ecs_query_t *query);

/** Does query return one or more results. 
 * 
 * @param query The query.
 * @return True if query matches anything, false if not.
 */
FLECS_API
bool ecs_query_is_true(
    const ecs_query_t *query);

/** Get query used to populate cache.
 * This operation returns the query that is used to populate the query cache.
 * For queries that are can be entirely cached, the returned query will be 
 * equivalent to the query passed to ecs_query_get_cache_query().
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

/** Send event.
 * This sends an event to matching triggers & is the mechanism used by flecs
 * itself to send `OnAdd`, `OnRemove`, etc events.
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
 * @param world The world.
 * @param desc Event parameters.
 *
 * @see ecs_enqueue()
 */
FLECS_API
void ecs_emit(
    ecs_world_t *world,
    ecs_event_desc_t *desc);

/** Enqueue event.
 * Same as ecs_emit(), but enqueues an event in the command queue instead. The
 * event will be emitted when ecs_defer_end() is called.
 * 
 * If this operation is called when the provided world is not in deferred mode
 * it behaves just like ecs_emit().
 * 
 * @param world The world.
 * @param desc Event parameters.
*/
FLECS_API
void ecs_enqueue(
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
 * @return The observer, or 0 if the operation failed.
 */
FLECS_API
ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc);

/** Get observer object.
 * Returns the observer object. Can be used to access various information about
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
 * This operation is slightly slower than using a type-specific iterator (e.g.
 * ecs_query_next, ecs_query_next) as it has to call a function pointer which
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
 * This is especially useful in combination with fact-checking queries (see the
 * queries addon).
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
 * @code
 * // Query that matches (Eats, *)
 * ecs_query_t *q = ecs_query(world, {
 *   .terms = {
 *     { .first.id = Eats, .second.name = "$food" }
 *   }
 * });
 * 
 * int food_var = ecs_query_find_var(r, "food");
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
 * first call to next.
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

/** Same as ecs_iter_set_var(), but for a range of entities
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
 * @return The variable value.
 */
FLECS_API
ecs_entity_t ecs_iter_get_var(
    ecs_iter_t *it,
    int32_t var_id);

/** Get variable name.
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 * @return The variable name.
 */
FLECS_API
const char* ecs_iter_get_var_name(
    const ecs_iter_t *it,
    int32_t var_id);

/** Get number of variables. 
 * 
 * @param it The iterator.
 * @return The number of variables.
*/
FLECS_API
int32_t ecs_iter_get_var_count(
    const ecs_iter_t *it);

/** Get variable array.
 * 
 * @param it The iterator.
 * @return The variable array (if any).
 */
FLECS_API
ecs_var_t* ecs_iter_get_vars(
    const ecs_iter_t *it);

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
 * @return The variable value.
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
 * @return The variable value.
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

/** Return the group id for the currently iterated result.
 * This operation returns the group id for queries that use group_by. If this
 * operation is called on an iterator that is not iterating a query that uses
 * group_by it will fail.
 * 
 * For queries that use cascade, this operation will return the hierarchy depth
 * of the currently iterated result.
 * 
 * @param it The iterator.
 * @return The group id of the currently iterated result.
 */
FLECS_API
uint64_t ecs_iter_get_group(
    const ecs_iter_t *it);

/** Returns whether current iterator result has changed.
 * This operation must be used in combination with a query that supports change
 * detection (e.g. is cached). The operation returns whether the currently
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

/** Convert iterator to string.
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
 * Progresses an iterator created by ecs_page_iter().
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
 * Progresses an iterator created by ecs_worker_iter().
 *
 * @param it The iterator.
 * @return true if iterator has more results, false if not.
 */
FLECS_API
bool ecs_worker_next(
    ecs_iter_t *it);

/** Get data for field.
 * This operation retrieves a pointer to an array of data that belongs to the
 * term in the query. The index refers to the location of the term in the query,
 * and starts counting from zero.
 *
 * For example, the query `"Position, Velocity"` will return the `Position` array
 * for index 0, and the `Velocity` array for index 1.
 *
 * When the specified field is not owned by the entity this function returns a
 * pointer instead of an array. This happens when the source of a field is not
 * the entity being iterated, such as a shared component (from a prefab), a
 * component from a parent, or another entity. The ecs_field_is_self() operation
 * can be used to test dynamically if a field is owned.
 * 
 * When a field contains a sparse component, use the ecs_field_at function. When
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
 *   for (int32_t i = 0; i < it->count; i ++) {
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

/** Get data for field at specified row.
 * This operation should be used instead of ecs_field_w_size for sparse 
 * component fields. This operation should be called for each returned row in a
 * result. In the following example the Velocity component is sparse:
 * 
 * @code
 * while (ecs_query_next(&it)) {
 *   Position *p = ecs_field(&it, Position, 0);
 *   for (int32_t i = 0; i < it->count; i ++) {
 *     Velocity *v = ecs_field_at(&it, Velocity, 1);
 *     p[i].x += v->x;
 *     p[i].y += v->y;
 *   }
 * }
 * @endcode
 * 
 * @param it the iterator.
 * @param size The size of the field type.
 * @param index The index of the field.
 * @return A pointer to the data of the field.
 */
FLECS_API
void* ecs_field_at_w_size(
    const ecs_iter_t *it,
    size_t size,
    int8_t index,
    int32_t row);

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
    int8_t index);

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
    int8_t index);

/** Test whether field is set.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is set.
 */
FLECS_API
bool ecs_field_is_set(
    const ecs_iter_t *it,
    int8_t index);

/** Return id matched for field.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The id matched for the field.
 */
FLECS_API
ecs_id_t ecs_field_id(
    const ecs_iter_t *it,
    int8_t index);

/** Return index of matched table column.
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
    int8_t index);

/** Return field type size.
 * Return type size of the field. Returns 0 if the field has no data.
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

/** Get type for table.
 * The table type is a vector that contains all component, tag and pair ids.
 *
 * @param table The table.
 * @return The type of the table.
 */
FLECS_API
const ecs_type_t* ecs_table_get_type(
    const ecs_table_t *table);

/** Get type index for component.
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

/** Get column index for component.
 * This operation returns the column index for a component in the table's type. 
 * If the component doesn't have data (it is a tag), the function will return -1.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component.
 * @return The column index of the id, or -1 if not found/not a component.
 */
FLECS_API
int32_t ecs_table_get_column_index(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component);

/** Return number of columns in table.
 * Similar to `ecs_table_get_type(table)->count`, except that the column count
 * only counts the number of components in a table.
 *
 * @param table The table.
 * @return The number of columns in the table.
 */
FLECS_API
int32_t ecs_table_column_count(
    const ecs_table_t *table);

/** Convert type index to column index.
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

/** Convert column index to type index.
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

/** Get column from table by column index.
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

/** Get column from table by component.
 * This operation returns the component array for the provided component.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component for the column.
 * @param offset The index of the first row to return (0 for entire column).
 * @return The component array, or NULL if the index is not a component.
 */
FLECS_API
void* ecs_table_get_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component,
    int32_t offset);

/** Get column size from table.
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

/** Returns the number of entities in the table.
 * This operation returns the number of entities in the table.
 *
 * @param table The table.
 * @return The number of entities in the table.
 */
FLECS_API
int32_t ecs_table_count(
    const ecs_table_t *table);

/** Returns allocated size of table.
 * This operation returns the number of elements allocated in the table 
 * per column.
 * 
 * @param table The table.
 * @return The number of allocated elements in the table.
 */
FLECS_API
int32_t ecs_table_size(
    const ecs_table_t *table);

/** Returns array with entity ids for table.
 * The size of the returned array is the result of ecs_table_count().
 * 
 * @param table The table.
 * @return Array with entity ids for table.
 */
FLECS_API
const ecs_entity_t* ecs_table_entities(
    const ecs_table_t *table);

/** Test if table has component.
 * Same as `ecs_table_get_type_index(world, table, component) != -1`.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component.
 * @return True if the table has the id, false if the table doesn't.
 *
 * @see ecs_table_get_type_index()
 */
FLECS_API
bool ecs_table_has_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t component);

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

/** Get table that has all components of current table plus the specified id.
 * If the provided table already has the provided id, the operation will return
 * the provided table.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component to add.
 * @result The resulting table.
 */
FLECS_API
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component);

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

/** Get table that has all components of current table minus the specified component.
 * If the provided table doesn't have the provided component, the operation will 
 * return the provided table.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component to remove.
 * @result The resulting table.
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

/** Test table for flags.
 * Test if table has all of the provided flags. See
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

/** Check if table has traversable entities.
 * Traversable entities are entities that are used as target in a pair with a
 * relationship that has the Traversable trait.
 * 
 * @param table The table.
 * @return Whether the table has traversable entities.
 */
FLECS_API
bool ecs_table_has_traversable(
    const ecs_table_t *table);

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
 * - `OnAdd` triggers for non-overlapping components in the target table
 * - `OnRemove` triggers for non-overlapping components in the source table.
 *
 * This operation is a faster than adding/removing components individually.
 *
 * The application must explicitly provide the difference in components between
 * tables as the added/removed parameters. This can usually be derived directly
 * from the result of ecs_table_add_id() and ecs_table_remove_id(). These arrays are
 * required to properly execute `OnAdd`/`OnRemove` triggers.
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


/** Search for component in table type.
 * This operation returns the index of first occurrence of the component in the 
 * table type. The component may be a pair or wildcard.
 *
 * When component_out is provided, the function will assign it with the found 
 * component. The found component may be different from the provided component 
 * if it is a wildcard.
 *
 * This is a constant time operation.
 *
 * @param world The world.
 * @param table The table.
 * @param component The component to search for.
 * @param component_out If provided, it will be set to the found component (optional).
 * @return The index of the id in the table type.
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

/** Search for component in table type starting from an offset.
 * This operation is the same as ecs_search(), but starts searching from an offset
 * in the table type.
 *
 * This operation is typically called in a loop where the resulting index is
 * used in the next iteration as offset:
 *
 * @code
 * int32_t index = -1;
 * while ((index = ecs_search_offset(world, table, offset, id, NULL))) {
 *   // do stuff
 * }
 * @endcode
 *
 * Depending on how the operation is used it is either linear or constant time.
 * When the id has the form `(id)` or `(rel, *)` and the operation is invoked as
 * in the above example, it is guaranteed to be constant time.
 *
 * If the provided component has the form `(*, tgt)` the operation takes linear 
 * time. The reason for this is that ids for an target are not packed together, 
 * as they are sorted relationship first.
 *
 * If the component at the offset does not match the provided id, the operation
 * will do a linear search to find a matching id.
 *
 * @param world The world.
 * @param table The table.
 * @param offset Offset from where to start searching.
 * @param component The component to search for.
 * @param component_out If provided, it will be set to the found component (optional).
 * @return The index of the id in the table type.
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

/** Search for component/relationship id in table type starting from an offset.
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
 *   ecs_id(Position), // the component id
 *   EcsIsA,           // the relationship to traverse
 *   0,                // start at depth 0 (the table itself)
 *   0,                // no depth limit
 *   NULL,             // (optional) entity on which component was found
 *   NULL,             // see above
 *   NULL);            // internal type with information about matched id
 * @endcode
 *
 * The operation searches depth first. If a table type has 2 `IsA` relationships, the
 * operation will first search the `IsA` tree of the first relationship.
 *
 * When choosing between ecs_search(), ecs_search_offset() and ecs_search_relation(),
 * the simpler the function the better its performance.
 *
 * @param world The world.
 * @param table The table.
 * @param offset Offset from where to start searching.
 * @param component The component to search for.
 * @param rel The relationship to traverse (optional).
 * @param flags Whether to search EcsSelf and/or EcsUp.
 * @param subject_out If provided, it will be set to the matched entity.
 * @param component_out If provided, it will be set to the found component (optional).
 * @param tr_out Internal datatype.
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
    ecs_entity_t *subject_out,
    ecs_id_t *component_out,
    struct ecs_table_record_t **tr_out);

/** Remove all entities in a table. Does not deallocate table memory. 
 * Retaining table memory can be efficient when planning 
 * to refill the table with operations like ecs_bulk_init
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
 * Construct, destruct, copy and move dynamically created values.
 *
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
 * @param ptr A pointer to the value.
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
 * @ingroup c
 * C APIs for addons.
 *
 * @{
 * @}
 */

#include "flecs/addons/flecs_c.h"

#ifdef __cplusplus
}
#endif

#include "flecs/private/addons.h"

#endif
