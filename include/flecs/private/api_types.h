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
typedef struct ecs_query_cache_match_t ecs_query_cache_match_t;

/* Cached query group */
typedef struct ecs_query_cache_group_t ecs_query_cache_group_t;

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
    uint64_t last_observer_id;
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
    ecs_entity_t entity;         /* Entity */
    ecs_entity_t id;             /* Component id */
    uint64_t table_id;           /* Table id for detecting ABA issues */
    uint32_t table_version_fast; /* Fast change detection w/false positives */
    uint16_t table_version;      /* Change detection */
    ecs_record_t *record;        /* Entity index record */
    void *ptr;                   /* Cached component pointer */
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
    const struct ecs_table_cache_hdr_t *cur, *next;
    bool iter_fill;
    bool iter_empty;
} ecs_table_cache_iter_t;

/** Each iterator */
typedef struct ecs_each_iter_t {
    ecs_table_cache_iter_t it;

    /* Storage for iterator fields */
    ecs_id_t ids;
    ecs_entity_t sources;
    ecs_size_t sizes;
    int32_t columns;
    const ecs_table_record_t* trs;
} ecs_each_iter_t;

typedef struct ecs_query_op_profile_t {
    int32_t count[2]; /* 0 = enter, 1 = redo */
} ecs_query_op_profile_t;

/** Query iterator */
typedef struct ecs_query_iter_t {
    struct ecs_var_t *vars;                   /* Variable storage */
    const struct ecs_query_var_t *query_vars; /* Query variable metadata */
    const struct ecs_query_op_t *ops;         /* Query plan operations */
    struct ecs_query_op_ctx_t *op_ctx;        /* Operation-specific state */
    uint64_t *written;

    /* Cached iteration */
    ecs_query_cache_group_t *group;           /* Currently iterated group */
    ecs_vec_t *tables;                        /* Currently iterated table vector (vec<ecs_query_cache_match_t>) */
    ecs_vec_t *all_tables;                    /* Different from .tables if iterating wildcard matches (vec<ecs_query_cache_match_t>) */
    ecs_query_cache_match_t *elem;            /* Current cache entry */
    int32_t cur, all_cur;                     /* Indices into tables & all_tables */

    ecs_query_op_profile_t *profile;

    int16_t op;                               /* Currently iterated query plan operation (index into ops) */
    bool iter_single_group;
} ecs_query_iter_t;

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
    ecs_stack_cursor_t *stack_cursor; /* Stack cursor to restore to */
} ecs_iter_private_t;

/* Data structures that store the command queue */
typedef struct ecs_commands_t {
    ecs_vec_t queue;
    ecs_stack_t stack;          /* Temp memory used by deferred commands */
    ecs_sparse_t entries;       /* <entity, op_entry_t> - command batching */
} ecs_commands_t;

#ifdef __cplusplus
}
#endif

#endif
