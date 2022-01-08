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

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Opaque types
////////////////////////////////////////////////////////////////////////////////

/** A stage enables modification while iterating and from multiple threads */
typedef struct ecs_stage_t ecs_stage_t;

/** A table is where entities and components are stored */
typedef struct ecs_table_t ecs_table_t;

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

struct ecs_record_t {
    ecs_table_t *table;  /* Identifies a type (and table) in world */
    uint32_t row;        /* Table row of the entity */
};

/** Cached reference. */
struct ecs_ref_t {
    ecs_entity_t entity;    /* Entity of the reference */
    ecs_entity_t component; /* Component of the reference */
    void *table;            /* Last known table */
    uint32_t row;           /* Last known location in table */
    int32_t alloc_count;    /* Last known alloc count of table */
    ecs_record_t *record;   /* Pointer to record, if in main stage */
    const void *ptr;        /* Cached ptr */
};

/** Array of entity ids that, other than a type, can live on the stack */
typedef struct ecs_ids_t {
    ecs_id_t *array;        /* An array with entity ids */
    int32_t count;          /* The number of entities in the array */
    int32_t size;           /* The size of the array */
} ecs_ids_t;

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

/** Term-iterator specific data */
typedef struct ecs_term_iter_t {
    ecs_term_t term;
    ecs_id_record_t *self_index;
    ecs_id_record_t *set_index;

    ecs_id_record_t *cur;
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
    EcsIterEvalIndex,
    EcsIterEvalChain,
    EcsIterEvalCondition,
    EcsIterEvalNone
} ecs_iter_kind_t;

/** Filter-iterator specific data */
typedef struct ecs_filter_iter_t {
    ecs_filter_t filter;
    ecs_iter_kind_t kind; 
    ecs_term_iter_t term_iter;
    int32_t matches_left;
} ecs_filter_iter_t;

/** Query-iterator specific data */
typedef struct ecs_query_iter_t {
    ecs_query_t *query;
    ecs_query_table_node_t *node, *prev;
    int32_t sparse_smallest;
    int32_t sparse_first;
    int32_t bitset_first;
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
    struct ecs_rule_reg_t *registers;    /* Variable storage (tables, entities) */
    ecs_entity_t *variables;             /* Variable storage for iterator (entities only) */
    struct ecs_rule_op_ctx_t *op_ctx;    /* Operation-specific state */
    
    int32_t *columns;                    /* Column indices */
    
    ecs_entity_t entity;                 /* Result in case of 1 entity */

    bool redo;
    int32_t op;
    int32_t sp;
} ecs_rule_iter_t;

/* Inline arrays for queries with small number of components */
typedef struct ecs_iter_cache_t {
    ecs_id_t ids[ECS_TERM_CACHE_SIZE];
    int32_t columns[ECS_TERM_CACHE_SIZE];
    ecs_entity_t subjects[ECS_TERM_CACHE_SIZE];
    ecs_size_t sizes[ECS_TERM_CACHE_SIZE];
    void *ptrs[ECS_TERM_CACHE_SIZE];
    int32_t match_indices[ECS_TERM_CACHE_SIZE];

    bool ids_alloc;
    bool columns_alloc;
    bool subjects_alloc;
    bool sizes_alloc;
    bool ptrs_alloc;
    bool match_indices_alloc;
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

/** Iterator.
 */
struct ecs_iter_t {
    /* World */
    ecs_world_t *world;           /* The world */
    ecs_world_t *real_world;      /* Actual world. This differs from world when in staged mode */

    /* Matched data */
    ecs_entity_t *entities;       /* Entity identifiers */
    void **ptrs;                  /* Pointers to components. Array if from this, pointer if not. */
    ecs_size_t *sizes;            /* Component sizes */
    ecs_table_t *table;           /* Current table */
    ecs_type_t type;              /* Current type */
    ecs_table_t *other_table;     /* Prev or next table when adding/removing */
    ecs_id_t *ids;                /* (Component) ids */
    ecs_entity_t *variables;      /* Values of variables (if any) */
    int32_t *columns;             /* Query term to table column mapping */
    ecs_entity_t *subjects;       /* Subject (source) entities */
    int32_t *match_indices;       /* Indices of current match for term. Allows an iterator to iterate
                                   * all permutations of wildcards in query. */
    ecs_ref_t *references;        /* Cached refs to components (if iterating a cache) */

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
    bool is_valid;                /* Set to true after first next() */
    bool is_filter;               /* When true, data fields are not set */
    bool is_instanced;            /* When true, owned terms are always returned as arrays */
    bool has_shared;              /* Iterator may set this when iterator has shared terms */
    bool table_only;              /* If false, iterator does not expose table data */

    ecs_entity_t interrupted_by;  /* When set, system execution is interrupted */

    ecs_iter_private_t priv;      /* Private data */

    /* Chained iterators */
    ecs_iter_next_action_t next;  /* Function to progress iterator */
    ecs_iter_fini_action_t fini;  /* Function to cleanup iterator resources */
    ecs_iter_t *chain_it;         /* Optional, allows for creating iterator chains */
};

////////////////////////////////////////////////////////////////////////////////
//// Function types
////////////////////////////////////////////////////////////////////////////////

typedef struct EcsComponentLifecycle EcsComponentLifecycle;

/** Constructor/destructor. Used for initializing / deinitializing components. */
typedef void (*ecs_xtor_t)(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx);

/** Copy is invoked when a component is copied into another component. */
typedef void (*ecs_copy_t)(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    const void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx);

/** Move is invoked when a component is moved to another component. */
typedef void (*ecs_move_t)(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx);

/** Copy ctor */
typedef void (*ecs_copy_ctor_t)(
    ecs_world_t *world,
    ecs_entity_t component,
    const EcsComponentLifecycle *callbacks,
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    const void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx);

/** Move ctor */
typedef void (*ecs_move_ctor_t)(
    ecs_world_t *world,
    ecs_entity_t component,
    const EcsComponentLifecycle *callbacks,
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx);

#ifdef __cplusplus
}
#endif

#endif

