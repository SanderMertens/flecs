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

/** Table data */
typedef struct ecs_data_t ecs_data_t;

/* Cached query table data */
typedef struct ecs_query_cache_table_match_t ecs_query_cache_table_match_t;

////////////////////////////////////////////////////////////////////////////////
//// Non-opaque types
////////////////////////////////////////////////////////////////////////////////

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
    ecs_event_record_t on_wildcard;
    ecs_sparse_t events;  /* sparse<event, ecs_event_record_t> */
};

/** Record for entity index */
struct ecs_record_t {
    ecs_id_record_t *idr; /* Id record to (*, entity) for target entities */
    ecs_table_t *table;   /* Identifies a type (and table) in world */
    uint32_t row;         /* Table row of the entity */
    int32_t dense;        /* Index in dense array of entity index */    
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
    uint64_t table_id;      /* Table id for detecting ABA issues */
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
    struct ecs_table_cache_hdr_t *next_list;
} ecs_table_cache_iter_t;

/** Each iterator */
typedef struct ecs_each_iter_t {
    ecs_table_cache_iter_t it;

    /* Storage for iterator fields */
    ecs_id_t ids;
    ecs_entity_t sources;
    ecs_size_t sizes;
    int32_t columns;
    void *ptrs;
} ecs_each_iter_t;

typedef struct ecs_query_op_profile_t {
    int32_t count[2]; /* 0 = enter, 1 = redo */
} ecs_query_op_profile_t;

/** Query iterator */
typedef struct ecs_query_iter_t {
    const ecs_query_t *query;
    struct ecs_var_t *vars;               /* Variable storage */
    const struct ecs_query_var_t *query_vars;
    const struct ecs_query_op_t *ops;
    struct ecs_query_op_ctx_t *op_ctx;    /* Operation-specific state */
    ecs_query_cache_table_match_t *node, *prev, *last; /* For cached iteration */
    uint64_t *written;
    int32_t skip_count;

    ecs_query_op_profile_t *profile;

    int16_t op;
    int16_t sp;
} ecs_query_iter_t;

/* Bits for tracking whether a cache was used/whether the array was allocated.
 * Used by flecs_iter_init, flecs_iter_validate and ecs_iter_fini. 
 * Constants are named to enable easy macro substitution. */
#define flecs_iter_cache_ids           (1u << 0u)
#define flecs_iter_cache_columns       (1u << 1u)
#define flecs_iter_cache_sources       (1u << 2u)
#define flecs_iter_cache_ptrs          (1u << 3u)
#define flecs_iter_cache_variables     (1u << 4u)
#define flecs_iter_cache_all           (255)

/* Inline iterator arrays to prevent allocations for small array sizes */
typedef struct ecs_iter_cache_t {
    ecs_stack_cursor_t *stack_cursor; /* Stack cursor to restore to */
    ecs_flags8_t used;       /* For which fields is the cache used */
    ecs_flags8_t allocated;  /* Which fields are allocated */
} ecs_iter_cache_t;

/* Private iterator data. Used by iterator implementations to keep track of
 * progress & to provide builtin storage. */
typedef struct ecs_iter_private_t {
    union {
        ecs_query_iter_t query;
        ecs_page_iter_t page;
        ecs_worker_iter_t worker;
        ecs_each_iter_t each;
    } iter;                       /* Iterator specific data */

    void *entity_iter;            /* Query applied after matching a table */
    ecs_iter_cache_t cache;       /* Inline arrays to reduce allocations */
} ecs_iter_private_t;

#ifdef __cplusplus
}
#endif

#endif

