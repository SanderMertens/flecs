/**
 * @file private_types.h
 * @brief Private types.
 */

#ifndef FLECS_PRIVATE_TYPES_H
#define FLECS_PRIVATE_TYPES_H

#ifndef __MACH__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "flecs.h"
#include "datastructures/entity_index.h"
#include "datastructures/stack_allocator.h"
#include "flecs/private/bitset.h"
#include "flecs/private/switch_list.h"

/* Used in id records to keep track of entities used with id flags */
extern const ecs_entity_t EcsFlag;

#define ECS_MAX_JOBS_PER_WORKER (16)

/* Magic number for a flecs object */
#define ECS_OBJECT_MAGIC (0x6563736f)

/* Tags associated with poly for (Poly, tag) components */
#define ecs_world_t_tag     invalid
#define ecs_stage_t_tag     invalid
#define ecs_query_t_tag     EcsQuery
#define ecs_rule_t_tag      EcsQuery
#define ecs_table_t_tag     invalid
#define ecs_filter_t_tag    EcsQuery
#define ecs_observer_t_tag  EcsObserver

/* Mixin kinds */
typedef enum ecs_mixin_kind_t {
    EcsMixinWorld,
    EcsMixinEntity,
    EcsMixinObservable,
    EcsMixinIterable,
    EcsMixinDtor,
    EcsMixinMax
} ecs_mixin_kind_t;

/* The mixin array contains pointers to mixin members for different kinds of
 * flecs objects. This allows the API to retrieve data from an object regardless
 * of its type. Each mixin array is only stored once per type */
struct ecs_mixins_t {
    const char *type_name; /* Include name of mixin type so debug code doesn't
                            * need to know about every object */
    ecs_size_t elems[EcsMixinMax];                        
};

/* Mixin tables */
extern ecs_mixins_t ecs_world_t_mixins;
extern ecs_mixins_t ecs_stage_t_mixins;
extern ecs_mixins_t ecs_filter_t_mixins;
extern ecs_mixins_t ecs_query_t_mixins;
extern ecs_mixins_t ecs_trigger_t_mixins;
extern ecs_mixins_t ecs_observer_t_mixins;

/* Types that have no mixins */
#define ecs_table_t_mixins (&(ecs_mixins_t){ NULL })

/* Scope for flecs internals, like observers used for builtin features */
extern const ecs_entity_t EcsFlecsInternals;

/** Type used for internal string hashmap */
typedef struct ecs_hashed_string_t {
    char *value;
    ecs_size_t length;
    uint64_t hash;
} ecs_hashed_string_t;

/* Table event type for notifying tables of world events */
typedef enum ecs_table_eventkind_t {
    EcsTableTriggersForId,
    EcsTableNoTriggersForId,
} ecs_table_eventkind_t;

typedef struct ecs_table_event_t {
    ecs_table_eventkind_t kind;

    /* Query event */
    ecs_query_t *query;

    /* Component info event */
    ecs_entity_t component;

    /* Event match */
    ecs_entity_t event;

    /* If the nubmer of fields gets out of hand, this can be turned into a union
     * but since events are very temporary objects, this works for now and makes
     * initializing an event a bit simpler. */
} ecs_table_event_t;

/** Stage-specific component data */
struct ecs_data_t {
    ecs_vec_t entities;          /* Entity identifiers */
    ecs_vec_t records;           /* Ptrs to records in main entity index */
    ecs_vec_t *columns;          /* Component columns */
    ecs_switch_t *sw_columns;    /* Switch columns */
    ecs_bitset_t *bs_columns;    /* Bitset columns */
};

/** Cache of added/removed components for non-trivial edges between tables */
#define ECS_TABLE_DIFF_INIT { .added = {0}}

typedef struct ecs_table_diff_t {
    ecs_type_t added;         /* Components added between tables */
    ecs_type_t removed;       /* Components removed between tables */
} ecs_table_diff_t;

/** Builder for table diff. The table diff type itself doesn't use ecs_vec_t to
 * conserve memory on table edges (a type doesn't have the size field), whereas
 * a vec for the builder is more convenient to use & has allocator support. */
typedef struct ecs_table_diff_builder_t {
    ecs_vec_t added;
    ecs_vec_t removed;
} ecs_table_diff_builder_t;

/** Edge linked list (used to keep track of incoming edges) */
typedef struct ecs_graph_edge_hdr_t {
    struct ecs_graph_edge_hdr_t *prev;
    struct ecs_graph_edge_hdr_t *next;
} ecs_graph_edge_hdr_t;

/** Single edge. */
typedef struct ecs_graph_edge_t {
    ecs_graph_edge_hdr_t hdr;
    ecs_table_t *from;       /* Edge source table */
    ecs_table_t *to;         /* Edge destination table */
    ecs_table_diff_t *diff;  /* Index into diff vector, if non trivial edge */
    ecs_id_t id;             /* Id associated with edge */
} ecs_graph_edge_t;

/* Edges to other tables. */
typedef struct ecs_graph_edges_t {
    ecs_graph_edge_t *lo; /* Small array optimized for low edges */
    ecs_map_t hi;  /* Map for hi edges (map<id, edge_t>) */
} ecs_graph_edges_t;

/* Table graph node */
typedef struct ecs_graph_node_t {
    /* Outgoing edges */
    ecs_graph_edges_t add;    
    ecs_graph_edges_t remove; 

    /* Incoming edges (next = add edges, prev = remove edges) */
    ecs_graph_edge_hdr_t refs;
} ecs_graph_node_t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing queries */
struct ecs_table_t {
    uint64_t id;                     /* Table id in sparse set */
    ecs_type_t type;                 /* Identifies table type in type_index */
    ecs_flags32_t flags;             /* Flags for testing table properties */
    uint16_t storage_count;          /* Number of components (excluding tags) */
    uint16_t generation;             /* Used for table cleanup */

    struct ecs_table_record_t *records; /* Array with table records */
    ecs_table_t *storage_table;      /* Table without tags */
    ecs_id_t *storage_ids;           /* Component ids (prevent indirection) */
    int32_t *storage_map;            /* Map type <-> data type
                                      *  - 0..count(T):         type -> data_type
                                      *  - count(T)..count(S):  data_type -> type
                                      */

    ecs_graph_node_t node;           /* Graph node */
    ecs_data_t data;                 /* Component storage */
    ecs_type_info_t **type_info;     /* Cached type info */

    int32_t *dirty_state;            /* Keep track of changes in columns */

    int16_t sw_count;
    int16_t sw_offset;
    int16_t bs_count;
    int16_t bs_offset;

    int32_t refcount;                /* Increased when used as storage table */
    int32_t lock;                    /* Prevents modifications */
    int32_t observed_count;          /* Number of observed entities in table */
    uint16_t record_count;           /* Table record count including wildcards */
};

/** Must appear as first member in payload of table cache */
typedef struct ecs_table_cache_hdr_t {
    struct ecs_table_cache_t *cache;
    ecs_table_t *table;
    struct ecs_table_cache_hdr_t *prev, *next;
    bool empty;
} ecs_table_cache_hdr_t;

/** Linked list of tables in table cache */
typedef struct ecs_table_cache_list_t {
    ecs_table_cache_hdr_t *first;
    ecs_table_cache_hdr_t *last;
    int32_t count;
} ecs_table_cache_list_t;

/** Table cache */
typedef struct ecs_table_cache_t {
    ecs_map_t index; /* <table_id, T*> */
    ecs_table_cache_list_t tables;
    ecs_table_cache_list_t empty_tables;
} ecs_table_cache_t;

/* Sparse query column */
typedef struct flecs_switch_term_t {
    ecs_switch_t *sw_column;
    ecs_entity_t sw_case; 
    int32_t signature_column_index;
} flecs_switch_term_t;

/* Bitset query column */
typedef struct flecs_bitset_term_t {
    ecs_bitset_t *bs_column;
    int32_t column_index;
} flecs_bitset_term_t;

/* Entity filter. This filters the entities of a matched table, for example when
 * it has disabled components or union relationships (switch). */
typedef struct ecs_entity_filter_t {
    ecs_vec_t sw_terms;       /* Terms with switch (union) entity filter */
    ecs_vec_t bs_terms;       /* Terms with bitset (toggle) entity filter */
    bool has_filter;
} ecs_entity_filter_t;

typedef struct ecs_entity_filter_iter_t {
    ecs_entity_filter_t *entity_filter;
    int32_t *columns;
    ecs_table_range_t range;
    int32_t bs_offset;
    int32_t sw_offset;
    int32_t sw_smallest;
} ecs_entity_filter_iter_t;

typedef struct ecs_query_table_match_t ecs_query_table_match_t;

/** List node used to iterate tables in a query.
 * The list of nodes dictates the order in which tables should be iterated by a
 * query. A single node may refer to the table multiple times with different
 * offset/count parameters, which enables features such as sorting. */
struct ecs_query_table_node_t {
    ecs_query_table_node_t *next, *prev;
    ecs_table_t *table;              /* The current table. */
    uint64_t group_id;             /* Value used to organize tables in groups */
    int32_t offset;                  /* Starting point in table  */
    int32_t count;                   /* Number of entities to iterate in table */
    ecs_query_table_match_t *match;  /* Reference to the match */
};

/** Type containing data for a table matched with a query. 
 * A single table can have multiple matches, if the query contains wildcards. */
struct ecs_query_table_match_t {
    ecs_query_table_node_t node; /* Embedded list node */

    int32_t *columns;         /* Mapping from query fields to table columns */
    int32_t *storage_columns; /* Mapping from query fields to storage columns */
    ecs_id_t *ids;            /* Resolved (component) ids for current table */
    ecs_entity_t *sources;    /* Subjects (sources) of ids */
    ecs_size_t *sizes;        /* Sizes for ids for current table */
    ecs_vec_t refs;           /* Cached components for non-this terms */
    ecs_entity_filter_t entity_filter; /* Entity specific filters */

    /* Next match in cache for same table (includes empty tables) */
    ecs_query_table_match_t *next_match;

    int32_t *monitor;         /* Used to monitor table for changes */
};

/** A single table can occur multiple times in the cache when a term matches
 * multiple table columns. */
typedef struct ecs_query_table_t {
    ecs_table_cache_hdr_t hdr;       /* Header for ecs_table_cache_t */
    ecs_query_table_match_t *first;  /* List with matches for table */
    ecs_query_table_match_t *last;   /* Last discovered match for table */
    int32_t rematch_count;           /* Track whether table was rematched */
} ecs_query_table_t;

/** Points to the beginning & ending of a query group */
typedef struct ecs_query_table_list_t {
    ecs_query_table_node_t *first;
    ecs_query_table_node_t *last;
    ecs_query_group_info_t info;
} ecs_query_table_list_t;

/* Query event type for notifying queries of world events */
typedef enum ecs_query_eventkind_t {
    EcsQueryTableMatch,
    EcsQueryTableRematch,
    EcsQueryTableUnmatch,
    EcsQueryOrphan
} ecs_query_eventkind_t;

typedef struct ecs_query_event_t {
    ecs_query_eventkind_t kind;
    ecs_table_t *table;
    ecs_query_t *parent_query;
} ecs_query_event_t;

/* Query level block allocators have sizes that depend on query field count */
typedef struct ecs_query_allocators_t {
    ecs_block_allocator_t columns;
    ecs_block_allocator_t ids;
    ecs_block_allocator_t sources;
    ecs_block_allocator_t sizes;
    ecs_block_allocator_t monitors;
} ecs_query_allocators_t;

/** Query that is automatically matched against tables */
struct ecs_query_t {
    ecs_header_t hdr;

    /* Query filter */
    ecs_filter_t filter;

    /* Tables matched with query */
    ecs_table_cache_t cache;

    /* Linked list with all matched non-empty tables, in iteration order */
    ecs_query_table_list_t list;

    /* Contains head/tail to nodes of query groups (if group_by is used) */
    ecs_map_t groups;

    /* Table sorting */
    ecs_entity_t order_by_component;
    ecs_order_by_action_t order_by;
    ecs_sort_table_action_t sort_table;
    ecs_vector_t *table_slices;

    /* Table grouping */
    ecs_entity_t group_by_id;
    ecs_group_by_action_t group_by;
    ecs_group_create_action_t on_group_create;
    ecs_group_delete_action_t on_group_delete;
    void *group_by_ctx;
    ecs_ctx_free_t group_by_ctx_free;

    /* Subqueries */
    ecs_query_t *parent;
    ecs_vector_t *subqueries;

    /* Flags for query properties */
    ecs_flags32_t flags;

    /* Monitor generation */
    int32_t monitor_generation;

    int32_t cascade_by;         /* Identify cascade column */
    int32_t match_count;        /* How often have tables been (un)matched */
    int32_t prev_match_count;   /* Track if sorting is needed */
    int32_t rematch_count;      /* Track which tables were added during rematch */

    /* Mixins */
    ecs_iterable_t iterable;
    ecs_poly_dtor_t dtor;

    /* Query-level allocators */
    ecs_query_allocators_t allocators;
};

/** All observers for a specific (component) id */
typedef struct ecs_event_id_record_t {
    /* Triggers for Self */
    ecs_map_t self;    /* map<trigger_id, trigger_t> */
    ecs_map_t self_up; /* map<trigger_id, trigger_t> */
    ecs_map_t up;      /* map<trigger_id, trigger_t> */

    ecs_map_t observers;     /* map<trigger_id, trigger_t> */

    /* Triggers for SuperSet, SubSet */
    ecs_map_t set_observers; /* map<trigger_id, trigger_t> */

    /* Triggers for Self with non-This subject */
    ecs_map_t entity_observers; /* map<trigger_id, trigger_t> */

    /* Number of active observers for (component) id */
    int32_t observer_count;
} ecs_event_id_record_t;

/* World level allocators are for operations that are not multithreaded */
typedef struct ecs_world_allocators_t {
    ecs_map_params_t ptr;
    ecs_map_params_t query_table_list;
    ecs_block_allocator_t query_table;
    ecs_block_allocator_t query_table_match;
    ecs_block_allocator_t graph_edge_lo;
    ecs_block_allocator_t graph_edge;
    ecs_block_allocator_t id_record;
    ecs_block_allocator_t id_record_chunk;
    ecs_block_allocator_t table_diff;
    ecs_block_allocator_t sparse_chunk;
    ecs_block_allocator_t hashmap;

    /* Temporary vectors used for creating table diff id sequences */
    ecs_table_diff_builder_t diff_builder;
} ecs_world_allocators_t;

/* Stage level allocators are for operations that can be multithreaded */
typedef struct ecs_stage_allocators_t {
    ecs_stack_t iter_stack;
    ecs_stack_t deser_stack;
    ecs_block_allocator_t cmd_entry_chunk;
} ecs_stage_allocators_t;

/** Types for deferred operations */
typedef enum ecs_cmd_kind_t {
    EcsOpClone,
    EcsOpBulkNew,
    EcsOpAdd,
    EcsOpRemove,   
    EcsOpSet,
    EcsOpEmplace,
    EcsOpMut,
    EcsOpModified,
    EcsOpAddModified,
    EcsOpDelete,
    EcsOpClear,
    EcsOpOnDeleteAction,
    EcsOpEnable,
    EcsOpDisable,
    EcsOpSkip
} ecs_cmd_kind_t;

typedef struct ecs_cmd_1_t {
    void *value;                /* Component value (used by set / get_mut) */
    ecs_size_t size;            /* Size of value */
    bool clone_value;           /* Clone entity with value (used for clone) */ 
} ecs_cmd_1_t;

typedef struct ecs_cmd_n_t {
    ecs_entity_t *entities;  
    int32_t count;
} ecs_cmd_n_t;

typedef struct ecs_cmd_t {
    ecs_cmd_kind_t kind;   /* Command kind */
    int32_t next_for_entity;    /* Next operation for entity */    
    ecs_id_t id;                /* (Component) id */
    ecs_id_record_t *idr;       /* Id record (only for set/mut/emplace) */
    ecs_entity_t entity;        /* Entity id */

    union {
        ecs_cmd_1_t _1;    /* Data for single entity operation */
        ecs_cmd_n_t _n;    /* Data for multi entity operation */
    } is;
} ecs_cmd_t;

/* Entity specific metadata for command in defer queue */
typedef struct ecs_cmd_entry_t {
    int32_t first;
    int32_t last; /* If -1, a delete command was inserted */
} ecs_cmd_entry_t;

/** A stage is a context that allows for safely using the API from multiple 
 * threads. Stage pointers can be passed to the world argument of API 
 * operations, which causes the operation to be ran on the stage instead of the
 * world. */
struct ecs_stage_t {
    ecs_header_t hdr;

    /* Unique id that identifies the stage */
    int32_t id;

    /* Deferred command queue */
    int32_t defer;
    ecs_vec_t commands;
    ecs_stack_t defer_stack;    /* Temp memory used by deferred commands */
    ecs_sparse_t cmd_entries;   /* <entity, op_entry_t> - command combining */

    /* Thread context */
    ecs_world_t *thread_ctx;     /* Points to stage when a thread stage */
    ecs_world_t *world;          /* Reference to world */
    ecs_os_thread_t thread;      /* Thread handle (0 if no threading is used) */

    /* One-shot actions to be executed after the merge */
    ecs_vector_t *post_frame_actions;

    /* Namespacing */
    ecs_entity_t scope;          /* Entity of current scope */
    ecs_entity_t with;           /* Id to add by default to new entities */
    ecs_entity_t base;           /* Currently instantiated top-level base */
    ecs_entity_t *lookup_path;   /* Search path used by lookup operations */

    /* Properties */
    bool auto_merge;             /* Should this stage automatically merge? */
    bool async;                  /* Is stage asynchronous? (write only) */

    /* Thread specific allocators */
    ecs_stage_allocators_t allocators;
    ecs_allocator_t allocator;
};

/* Component monitor */
typedef struct ecs_monitor_t {
    ecs_vector_t *queries;       /* vector<ecs_query_t*> */
    bool is_dirty;               /* Should queries be rematched? */
} ecs_monitor_t;

/* Component monitors */
typedef struct ecs_monitor_set_t {
    ecs_map_t monitors;          /* map<id, ecs_monitor_t> */
    bool is_dirty;               /* Should monitors be evaluated? */
} ecs_monitor_set_t;

/* Data stored for id marked for deletion */
typedef struct ecs_marked_id_t {
    ecs_id_record_t *idr;
    ecs_id_t id;
    ecs_entity_t action; /* Set explicitly for delete_with, remove_all */
    bool delete_id;
} ecs_marked_id_t;

typedef struct ecs_store_t {
    /* Entity lookup */
    ecs_sparse_t entity_index;   /* sparse<entity, ecs_record_t> */

    /* Table lookup by id */
    ecs_sparse_t tables;         /* sparse<table_id, ecs_table_t> */

    /* Table lookup by hash */
    ecs_hashmap_t table_map;     /* hashmap<ecs_type_t, ecs_table_t*> */

    /* Root table */
    ecs_table_t root;

    /* Records cache */
    ecs_vector_t *records;

    /* Stack of ids being deleted. */
    ecs_vector_t *marked_ids;    /* vector<ecs_marked_ids_t> */
} ecs_store_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    ecs_header_t hdr;

    /* --  Type metadata -- */
    ecs_id_record_t *id_index_lo;
    ecs_map_t id_index_hi;       /* map<id, ecs_id_record_t*> */
    ecs_sparse_t type_info;      /* sparse<type_id, type_info_t> */

    /* -- Cached handle to id records -- */
    ecs_id_record_t *idr_wildcard;
    ecs_id_record_t *idr_wildcard_wildcard;
    ecs_id_record_t *idr_any;
    ecs_id_record_t *idr_isa_wildcard;
    ecs_id_record_t *idr_childof_0;

    /* -- Mixins -- */
    ecs_world_t *self;
    ecs_observable_t observable;
    ecs_iterable_t iterable;

    /* Unique id per generated event used to prevent duplicate notifications */
    int32_t event_id;

    /* Is entity range checking enabled? */
    bool range_check_enabled;

    /* --  Data storage -- */
    ecs_store_t store;

    /* --  Pending table event buffers -- */
    ecs_sparse_t *pending_buffer;  /* sparse<table_id, ecs_table_t*> */
    ecs_sparse_t *pending_tables;  /* sparse<table_id, ecs_table_t*> */

    /* Used to track when cache needs to be updated */
    ecs_monitor_set_t monitors;  /* map<id, ecs_monitor_t> */

    /* -- Systems -- */
    ecs_entity_t pipeline;       /* Current pipeline */

    /* -- Identifiers -- */
    ecs_hashmap_t aliases;
    ecs_hashmap_t symbols;

    /* -- Staging -- */
    ecs_stage_t *stages;         /* Stages */
    int32_t stage_count;         /* Number of stages */

    /* -- Multithreading -- */
    ecs_os_cond_t worker_cond;   /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;     /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;   /* Mutex for job_cond */
    int32_t workers_running;     /* Number of threads running */
    int32_t workers_waiting;     /* Number of workers waiting on sync */

    /* -- Time management -- */
    ecs_time_t world_start_time; /* Timestamp of simulation start */
    ecs_time_t frame_start_time; /* Timestamp of frame start */
    ecs_ftime_t fps_sleep;       /* Sleep time to prevent fps overshoot */

    /* -- Metrics -- */
    ecs_world_info_t info;

    /* -- World flags -- */
    ecs_flags32_t flags;

    /* Count that increases when component monitors change */
    int32_t monitor_generation;

    /* -- Allocators -- */
    ecs_world_allocators_t allocators; /* Static allocation sizes */
    ecs_allocator_t allocator;         /* Dynamic allocation sizes */

    void *context;               /* Application context */
    ecs_vector_t *fini_actions;  /* Callbacks to execute when world exits */
};

#endif
