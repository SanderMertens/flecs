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

////////////////////////////////////////////////////////////////////////////////
//// Non-opaque types
////////////////////////////////////////////////////////////////////////////////

struct ecs_record_t {
    ecs_table_t *table;  /* Identifies a type (and table) in world */
    int32_t row;         /* Table row of the entity */
};

/** Cached reference. */
struct ecs_ref_t {
    ecs_entity_t entity;    /* Entity of the reference */
    ecs_entity_t component; /* Component of the reference */
    void *table;            /* Last known table */
    int32_t row;            /* Last known location in table */
    int32_t alloc_count;    /* Last known alloc count of table */
    ecs_record_t *record;   /* Pointer to record, if in main stage */
    const void *ptr;        /* Cached ptr */
};

/** Array of entity ids that, other than a type, can live on the stack */
typedef struct ecs_ids_t {
    ecs_entity_t *array;    /* An array with entity ids */
    int32_t count;          /* The number of entities in the array */
} ecs_ids_t;

typedef struct ecs_page_cursor_t {
    int32_t first;
    int32_t count;
} ecs_page_cursor_t;

typedef struct ecs_page_iter_t {
    int32_t offset;
    int32_t limit;
    int32_t remaining;
} ecs_page_iter_t;

/** Table specific data for iterators */
typedef struct ecs_iter_table_t {
    int32_t *columns;         /* Mapping from query terms to table columns */
    ecs_table_t *table;       /* The current table. */
    ecs_data_t *data;         /* Table component data */
    ecs_entity_t *components; /* Components in current table */
    ecs_type_t *types;        /* Components in current table */
    ecs_ref_t *references;    /* References to entities (from query) */
} ecs_iter_table_t;

/** Term-iterator specific data */
typedef struct ecs_term_iter_t {
    ecs_term_t *term;
    ecs_id_record_t *self_index;
    ecs_id_record_t *set_index;
    
    ecs_map_iter_t iter;
    bool iter_set;

    /* Storage */
    ecs_id_t id;
    int32_t column;
    ecs_entity_t subject;
    ecs_size_t size;
    void *ptr;
} ecs_term_iter_t;

typedef enum ecs_filter_iter_kind_t {
    EcsFilterIterEvalIndex,
    EcsFilterIterEvalNone
} ecs_filter_iter_kind_t;

/** Filter-iterator specific data */
typedef struct ecs_filter_iter_t {
    ecs_filter_t filter;
    ecs_filter_iter_kind_t kind;

    /* For EcsFilterIterEvalIndex */ 
    ecs_term_iter_t term_iter;
    int32_t min_term_index;
} ecs_filter_iter_t;

/** Query-iterator specific data */
typedef struct ecs_query_iter_t {
    ecs_query_t *query;
    ecs_page_iter_t page_iter;
    int32_t index;
    int32_t sparse_smallest;
    int32_t sparse_first;
    int32_t bitset_first;
} ecs_query_iter_t;

/** Query-iterator specific data */
typedef struct ecs_snapshot_iter_t {
    ecs_filter_t filter;
    ecs_vector_t *tables; /* ecs_table_leaf_t */
    int32_t index;
} ecs_snapshot_iter_t;  

/* Inline arrays for queries with small number of components */
typedef struct ecs_iter_cache_t {
    ecs_id_t ids[ECS_TERM_CACHE_SIZE];
    int32_t columns[ECS_TERM_CACHE_SIZE];
    ecs_entity_t subjects[ECS_TERM_CACHE_SIZE];
    ecs_size_t sizes[ECS_TERM_CACHE_SIZE];
    void *ptrs[ECS_TERM_CACHE_SIZE];

    bool ids_alloc;
    bool columns_alloc;
    bool subjects_alloc;
    bool sizes_alloc;
    bool ptrs_alloc;
} ecs_iter_cache_t;

/** The ecs_iter_t struct allows applications to iterate tables.
 * Queries and filters, among others, allow an application to iterate entities
 * that match a certain set of components. Because of how data is stored 
 * internally, entities with a given set of components may be stored in multiple
 * consecutive arrays, stored across multiple tables. The ecs_iter_t type 
 * enables iteration across tables. */
struct ecs_iter_t {
    ecs_world_t *world;           /* The world */
    ecs_world_t *real_world;      /* Actual world. This differs from world when using threads.  */
    ecs_entity_t system;          /* The current system (if applicable) */
    ecs_entity_t event;           /* The event (if applicable) */
    ecs_id_t event_id;            /* The (component) id for the event */
    ecs_entity_t self;            /* Self entity (if set) */

    ecs_table_t *table;           /* Current table */
    ecs_type_t type;              /* Current type */

    ecs_id_t *ids;                /* (Component) ids */
    int32_t *columns;             /* Query term to table column mapping */
    ecs_entity_t *subjects;       /* Subject (source) entities */
    ecs_size_t *sizes;            /* Component sizes */
    void **ptrs;                  /* Pointers to components. Array if from this, pointer if not. */

    ecs_ref_t *references;

    ecs_term_t *terms;            /* Terms of query being evaluated */
    int32_t table_count;          /* Active table count for query */
    int32_t inactive_table_count; /* Inactive table count for query */
    int32_t term_count;           /* Number of terms in query */
    int32_t term_index;           /* Index of term that triggered an event.
                                   * This field will be set to the 'index' field
                                   * of a trigger/observer term. */
    
    void *table_columns;          /* Table component data */
    ecs_entity_t *entities;       /* Entity identifiers */

    void *param;                  /* Param passed to ecs_run */
    void *ctx;                    /* System context */
    void *binding_ctx;            /* Binding context */
    FLECS_FLOAT delta_time;       /* Time elapsed since last frame */
    FLECS_FLOAT delta_system_time;/* Time elapsed since last system invocation */
    FLECS_FLOAT world_time;       /* Time elapsed since start of simulation */

    int32_t frame_offset;         /* Offset relative to frame */
    int32_t offset;               /* Offset relative to current table */
    int32_t count;                /* Number of entities to process by system */
    int32_t total_count;          /* Total number of entities in table */

    bool is_valid;                /* Set to true after first next() */

    ecs_ids_t *triggered_by;      /* Component(s) that triggered the system */
    ecs_entity_t interrupted_by;  /* When set, system execution is interrupted */

    union {
        ecs_term_iter_t term;
        ecs_filter_iter_t filter;
        ecs_query_iter_t query;
        ecs_snapshot_iter_t snapshot;
    } iter;                       /* Iterator specific data */

    ecs_iter_cache_t cache;       /* Inline arrays to reduce allocations */
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

/** Invoked when setting a component */
typedef void (*ecs_on_set_t)(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx);

#ifdef __cplusplus
}
#endif

#endif

