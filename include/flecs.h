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
#ifndef ecs_float_t
#define ecs_float_t float
#endif

/* Customizable precision for time scalar values */
#ifndef ecs_ftime_t
#define ecs_ftime_t ecs_float_t
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

/* The following macros let you customize with which addons Flecs is built.
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
// #define FLECS_C          /* C API convenience macros, always enabled */
#define FLECS_CPP           /* C++ API */
#define FLECS_MODULE        /* Module support */
#define FLECS_PARSER        /* String parser for queries */
#define FLECS_PLECS         /* ECS data definition format */
#define FLECS_RULES         /* Constraint solver for advanced queries */
#define FLECS_SNAPSHOT      /* Snapshot & restore ECS data */
#define FLECS_STATS         /* Access runtime statistics */
#define FLECS_MONITOR       /* Track runtime statistics periodically */
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

#include "flecs/private/api_defines.h"
#include "flecs/private/vector.h"           /* Vector datatype */
#include "flecs/private/map.h"              /* Map */
#include "flecs/private/strbuf.h"           /* String builder */
#include "flecs/os_api.h"  /* Abstraction for operating system functions */

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

/* Maximum number of events to set in static array of observer descriptor */
#define ECS_OBSERVER_DESC_EVENT_COUNT_MAX (8)

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

/* Destructor function for poly objects */
typedef void (*ecs_poly_dtor_t)(
    ecs_poly_t *poly);

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

/** Use this variable to initialize user-allocated filter object */
FLECS_API extern ecs_filter_t ECS_FILTER_INIT;

/** Filters alllow for ad-hoc quick filtering of entity tables. */
struct ecs_filter_t {
    ecs_header_t hdr;
    
    ecs_term_t *terms;         /* Array containing terms for filter */
    int32_t term_count;        /* Number of elements in terms array */
    int32_t term_count_actual; /* Processed count, which folds OR terms */
    
    bool owned;                /* Is filter object owned by filter */
    bool terms_owned;          /* Is terms array owned by filter */

    ecs_flags32_t flags;       /* Filter flags */
    
    char *name;                /* Name of filter (optional) */
    char *variable_names[1];   /* Array with variable names */

    ecs_iterable_t iterable;   /* Iterable mixin */
};

/* An observer reacts to events matching a filter */
struct ecs_observer_t {
    ecs_header_t hdr;
    
    ecs_filter_t filter;

    /* Observer events */
    ecs_entity_t events[ECS_OBSERVER_DESC_EVENT_COUNT_MAX];
    int32_t event_count;   
    
    ecs_iter_action_t callback; /* See ecs_observer_desc_t::callback */
    ecs_run_action_t run;       /* See ecs_observer_desc_t::run */

    void *ctx;                  /* Callback context */
    void *binding_ctx;          /* Binding context (for language bindings) */

    ecs_ctx_free_t ctx_free;    /* Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /* Callback to free binding_ctx */

    ecs_observable_t *observable;  /* Observable for observer */

    int32_t *last_event_id;     /* Last handled event id */

    ecs_id_t register_id;       /* Id observer is registered with (single term observers only) */
    int32_t term_index;         /* Index of the term in parent observer (single term observers only) */

    bool is_monitor;            /* If true, the observer only triggers when the
                                 * filter did not match with the entity before
                                 * the event happened. */

    bool is_multi;              /* If true, the observer triggers on more than one term */

    /* Mixins */
    ecs_world_t *world;
    ecs_entity_t entity;
    ecs_poly_dtor_t dtor;
};

/** Type that contains component lifecycle callbacks. */
struct ecs_type_hooks_t {
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

    void *ctx;                       /* User defined context */
    void *binding_ctx;               /* Language binding context */

    ecs_ctx_free_t ctx_free;         /* Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /* Callback to free binding_ctx */
};

/** Type that contains component information (passed to ctors/dtors/...) */
struct ecs_type_info_t {
    ecs_size_t size;         /* Size of type */
    ecs_size_t alignment;    /* Alignment of type */
    ecs_type_hooks_t hooks;  /* Type hooks */
    ecs_entity_t component;  /* Handle to component (do not set) */
};

/** @} */


#include "flecs/private/api_types.h"        /* Supporting API types */
#include "flecs/private/api_support.h"      /* Supporting API functions */
#include "flecs/private/sparse.h"           /* Sparse set */
#include "flecs/private/hashmap.h"          /* Hashmap */

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

    ecs_entity_desc_t entity;  /* Parameters for component entity */
    ecs_type_info_t type;      /* Parameters for type (size, hooks, ...) */
} ecs_component_desc_t;

/** Used with ecs_filter_init. */
typedef struct ecs_filter_desc_t {
    int32_t _canary;

    /* Terms of the filter. If a filter has more terms than 
     * ECS_TERM_CACHE_SIZE use terms_buffer */
    ecs_term_t terms[ECS_TERM_DESC_CACHE_SIZE];

    /* For filters with lots of terms an outside array can be provided. */
    ecs_term_t *terms_buffer;
    int32_t terms_buffer_count;

    /* External storage to prevent allocation of the filter object */
    ecs_filter_t *storage;

    /* When true, terms returned by an iterator may either contain 1 or N 
     * elements, where terms with N elements are owned, and terms with 1 element 
     * are shared, for example from a parent or base entity. When false, the 
     * iterator will at most return 1 element when the result contains both 
     * owned and shared terms. */ 
    bool instanced;

    /* Flags for advanced usage */
    ecs_flags32_t flags;

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

    /* Callback used for ordering query results. Same as order_by,
     * but more efficient. */
    ecs_sort_table_action_t sort_table;

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

    /* Entity associated with query (optional) */
    ecs_entity_t entity;
} ecs_query_desc_t;

/** Used with ecs_observer_init. */
typedef struct ecs_observer_desc_t {
    int32_t _canary;

    /* Entity to associate with observer */
    ecs_entity_desc_t entity;

    /* Filter for observer */
    ecs_filter_desc_t filter;

    /* Events to observe (OnAdd, OnRemove, OnSet, UnSet) */
    ecs_entity_t events[ECS_OBSERVER_DESC_EVENT_COUNT_MAX];

    /* When observer is created, generate events from existing data. For example,
     * EcsOnAdd Position would match all existing instances of Position.
     * This is only supported for events that are iterable (see EcsIterable) */
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

    /* User context to pass to callback */
    void *ctx;

    /* Context to be used for language bindings */
    void *binding_ctx;
    
    /* Callback to free ctx */
    ecs_ctx_free_t ctx_free;

    /* Callback to free binding_ctx */     
    ecs_ctx_free_t binding_ctx_free;

    /* Observable with which to register the observer */
    ecs_poly_t *observable;

    /* Optional shared last event id for multiple observers. Ensures only one
     * of the observers with the shared id gets triggered for an event */
    int32_t *last_event_id;

    /* Used for internal purposes */
    int32_t term_index;
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

/** Component for storing a poly object */
typedef struct EcsPoly {
    ecs_poly_t *poly;
} EcsPoly;

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

    ecs_ftime_t delta_time_raw;       /* Raw delta time (no time scaling) */
    ecs_ftime_t delta_time;           /* Time passed to or computed by ecs_progress */
    ecs_ftime_t time_scale;           /* Time scale applied to delta_time */
    ecs_ftime_t target_fps;           /* Target fps */
    ecs_ftime_t frame_time_total;     /* Total time spent processing a frame */
    float system_time_total;          /* Total time spent in systems */
    float merge_time_total;           /* Total time spent in merges */
    ecs_ftime_t world_time_total;     /* Time elapsed in simulation */
    ecs_ftime_t world_time_total_raw; /* Time elapsed in simulation (no scaling) */
    
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

    /* -- Defered operation counts -- */
    int32_t new_count;
    int32_t bulk_new_count;
    int32_t delete_count;
    int32_t clear_count;
    int32_t add_count;
    int32_t remove_count;
    int32_t set_count;
    int32_t discard_count;
} ecs_world_info_t;

/** @} */

/* Only include deprecated definitions if deprecated addon is required */
#ifdef FLECS_DEPRECATED
#include "flecs/addons/deprecated.h"
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
FLECS_API extern const ecs_entity_t ecs_id(EcsIdentifier);
FLECS_API extern const ecs_entity_t ecs_id(EcsIterable);
FLECS_API extern const ecs_entity_t ecs_id(EcsPoly);

FLECS_API extern const ecs_entity_t EcsQuery;
FLECS_API extern const ecs_entity_t EcsObserver;

/* System module component ids */
FLECS_API extern const ecs_entity_t EcsSystem;
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

/* Tag to indicate that relation is stored as union. Union relations enable
 * changing the target of a union without switching tables. Union relationships
 * are also marked as exclusive. */
FLECS_API extern const ecs_entity_t EcsUnion;

/* Tag to indicate name identifier */
FLECS_API extern const ecs_entity_t EcsName;

/* Tag to indicate symbol identifier */
FLECS_API extern const ecs_entity_t EcsSymbol;

/* Tag to indicate alias identifier */
FLECS_API extern const ecs_entity_t EcsAlias;

/* Used to express parent-child relationships. */
FLECS_API extern const ecs_entity_t EcsChildOf;

/* Used to express is-a relationships. An IsA relationship indicates that the 
 * subject is a subset of the relation object. For example:
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

/* Used to express dependency relationships */
FLECS_API extern const ecs_entity_t EcsDependsOn;

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

/* Event. Exactly-once observer for when an entity matches/unmatches a filter */
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

/* Event. Triggers when a table becomes empty (doesn't emit on creation). */
FLECS_API extern const ecs_entity_t EcsOnTableEmpty;

/* Event. Triggers when a table becomes non-empty. */
FLECS_API extern const ecs_entity_t EcsOnTableFill;

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
 * EcsOnDeleteObject. Panic actions are ignored after ecs_quit is called. */
FLECS_API extern const ecs_entity_t EcsPanic;

/* Used like (EcsDefaultChildComponent, Component). When added to an entity,
 * this informs serialization formats which component to use when a value is
 * assigned to an entity without specifying the component. This is intended as
 * a hint, serialization formats are not required to use it. Adding this 
 * component does not change the behavior of core ECS operations. */
FLECS_API extern const ecs_entity_t EcsDefaultChildComponent;

/* Tag used to indicate query is empty */
FLECS_API extern const ecs_entity_t EcsEmpty;

/* Pipeline module tags */
FLECS_API extern const ecs_entity_t ecs_id(EcsPipeline);
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

/* Value used to quickly check if component is builtin. This is used to quickly
 * filter out tables with builtin components (for example for ecs_delete) */
#define EcsLastInternalComponentId (ecs_id(EcsPoly))

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
    const ecs_table_t *table);

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
    const ecs_world_t *world,
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
 */

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

/** Get column from table.
 * This operation returns the component array for the provided index.
 * 
 * @param table The table.
 * @return The component array, or NULL if the index is not a component.
 */
void* ecs_table_get_column(
    ecs_table_t *table,
    int32_t index);

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
    int32_t row_2
);

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

/** @} */

#include "flecs/addons/flecs_c.h"
#include "flecs/private/addons.h"

#ifdef __cplusplus
}
#endif

#endif
