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
    ecs_event_record_t un_set;
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

/* Cursor to stack allocator. Type is public to allow for white box testing. */
struct ecs_stack_page_t;

typedef struct ecs_stack_cursor_t {
    struct ecs_stack_cursor_t *prev;
    struct ecs_stack_page_t *page;
    int16_t sp;
    bool is_free;
#ifdef FLECS_DEBUG
    struct ecs_stack_t *owner;
#endif
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

/** Rule-iterator specific data */
typedef struct ecs_query_iter_t {
    const ecs_query_t *query;
    struct ecs_var_t *vars;               /* Variable storage */
    const struct ecs_query_var_t *query_vars;
    const struct ecs_query_op_t *ops;
    struct ecs_query_op_ctx_t *op_ctx;    /* Operation-specific state */
    ecs_query_cache_table_match_t *node, *prev, *last; /* For cached iteration */
    uint64_t *written;
    int32_t skip_count;

#ifdef FLECS_DEBUG
    ecs_query_op_profile_t *profile;
#endif

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

/** Iterator */
struct ecs_iter_t {
    /* World */
    ecs_world_t *world;           /* The world */
    ecs_world_t *real_world;      /* Actual world. This differs from world when in readonly mode */

    /* Matched data */
    ecs_entity_t *entities;       /* Entity identifiers */
    void **ptrs;                  /* Pointers to components. Array if from this, pointer if not. */
    const ecs_size_t *sizes;      /* Component sizes */
    ecs_table_t *table;           /* Current table */
    ecs_table_t *other_table;     /* Prev or next table when adding/removing */
    ecs_id_t *ids;                /* (Component) ids */
    ecs_var_t *variables;         /* Values of variables (if any) */
    int32_t *columns;             /* Query term to table column mapping */
    ecs_entity_t *sources;        /* Entity on which the id was matched (0 if same as entities) */
    ecs_ref_t *references;        /* Cached refs to components (if iterating a cache) */
    ecs_flags64_t constrained_vars; /* Bitset that marks constrained variables */
    ecs_flags64_t set_fields;     /* Fields that are set */
    uint64_t group_id;            /* Group id for table, if group_by is used */
    int32_t field_count;          /* Number of fields in iterator */
    ecs_termset_t shared_fields;  /* Bitset with shared fields */
    ecs_termset_t up_fields;      /* Bitset with fields matched through up traversal */

    /* Input information */
    ecs_entity_t system;          /* The system (if applicable) */
    ecs_entity_t event;           /* The event (if applicable) */
    ecs_id_t event_id;            /* The (component) id for the event */
    int32_t event_cur;            /* Unique event id. Used to dedup observer calls */

    /* Query information */
    const ecs_query_t *query;     /* Query being evaluated */
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
    ecs_iter_action_t set_var;    /* Invoked after setting variable (optionally set) */
    ecs_iter_fini_action_t fini;  /* Function to cleanup iterator resources */
    ecs_iter_t *chain_it;         /* Optional, allows for creating iterator chains */
};

#ifdef __cplusplus
}
#endif

#endif

