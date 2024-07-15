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
#include "storage/entity_index.h"
#include "flecs/datastructures/bitset.h"
#include "storage/table.h"

/* Used in id records to keep track of entities used with id flags */
extern const ecs_entity_t EcsFlag;

#define ECS_MAX_JOBS_PER_WORKER (16)
#define ECS_MAX_DEFER_STACK (8)

/* Magic number for a flecs object */
#define ECS_OBJECT_MAGIC (0x6563736f)

/* Tags associated with poly for (Poly, tag) components */
#define ecs_world_t_tag     invalid
#define ecs_stage_t_tag     invalid
#define ecs_query_t_tag     EcsQuery
#define ecs_observer_t_tag  EcsObserver

/* Mixin kinds */
typedef enum ecs_mixin_kind_t {
    EcsMixinWorld,
    EcsMixinEntity,
    EcsMixinObservable,
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
extern ecs_mixins_t ecs_query_t_mixins;
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
    ecs_block_allocator_t query_impl;
    ecs_block_allocator_t query_cache;
} ecs_stage_allocators_t;

/** Types for deferred operations */
typedef enum ecs_cmd_kind_t {
    EcsCmdClone,
    EcsCmdBulkNew,
    EcsCmdAdd,
    EcsCmdRemove,   
    EcsCmdSet,
    EcsCmdEmplace,
    EcsCmdEnsure,
    EcsCmdModified,
    EcsCmdModifiedNoHook,
    EcsCmdAddModified,
    EcsCmdPath,
    EcsCmdDelete,
    EcsCmdClear,
    EcsCmdOnDeleteAction,
    EcsCmdEnable,
    EcsCmdDisable,
    EcsCmdEvent,
    EcsCmdSkip
} ecs_cmd_kind_t;

/* Entity specific metadata for command in queue */
typedef struct ecs_cmd_entry_t {
    int32_t first;
    int32_t last;                    /* If -1, a delete command was inserted */
} ecs_cmd_entry_t;

typedef struct ecs_cmd_1_t {
    void *value;                     /* Component value (used by set / ensure) */
    ecs_size_t size;                 /* Size of value */
    bool clone_value;                /* Clone entity with value (used for clone) */ 
} ecs_cmd_1_t;

typedef struct ecs_cmd_n_t {
    ecs_entity_t *entities;  
    int32_t count;
} ecs_cmd_n_t;

typedef struct ecs_cmd_t {
    ecs_cmd_kind_t kind;             /* Command kind */
    int32_t next_for_entity;         /* Next operation for entity */    
    ecs_id_t id;                     /* (Component) id */
    ecs_id_record_t *idr;            /* Id record (only for set/mut/emplace) */
    ecs_cmd_entry_t *entry;
    ecs_entity_t entity;             /* Entity id */

    union {
        ecs_cmd_1_t _1;              /* Data for single entity operation */
        ecs_cmd_n_t _n;              /* Data for multi entity operation */
    } is;

    ecs_entity_t system;             /* System that enqueued the command */
} ecs_cmd_t;

/* Data structures that store the command queue */
typedef struct ecs_commands_t {
    ecs_vec_t queue;
    ecs_stack_t stack;          /* Temp memory used by deferred commands */
    ecs_sparse_t entries;       /* <entity, op_entry_t> - command batching */
} ecs_commands_t;

/** Callback used to capture commands of a frame */
typedef void (*ecs_on_commands_action_t)(
    const ecs_stage_t *stage,
    const ecs_vec_t *commands,
    void *ctx);

/** A stage is a context that allows for safely using the API from multiple 
 * threads. Stage pointers can be passed to the world argument of API 
 * operations, which causes the operation to be ran on the stage instead of the
 * world. The features provided by a stage are:
 * 
 *  - A command queue for deferred ECS operations and events
 *  - Thread specific allocators
 *  - Thread specific world state (like current scope, with, current system)
 *  - Thread specific buffers for preventing allocations
 */
struct ecs_stage_t {
    ecs_header_t hdr;

    /* Unique id that identifies the stage */
    int32_t id;

    /* Zero if not deferred, positive if deferred, negative if suspended */
    int32_t defer;

    /* Command queue stack, for nested execution */
    ecs_commands_t *cmd;
    ecs_commands_t cmd_stack[ECS_MAX_DEFER_STACK];
    int32_t cmd_sp;

    /* Thread context */
    ecs_world_t *thread_ctx;         /* Points to stage when a thread stage */
    ecs_world_t *world;              /* Reference to world */
    ecs_os_thread_t thread;          /* Thread handle (0 if no threading is used) */

    /* One-shot actions to be executed after the merge */
    ecs_vec_t post_frame_actions;

    /* Namespacing */
    ecs_entity_t scope;              /* Entity of current scope */
    ecs_entity_t with;               /* Id to add by default to new entities */
    ecs_entity_t base;               /* Currently instantiated top-level base */
    const ecs_entity_t *lookup_path; /* Search path used by lookup operations */

    /* Running system */
    ecs_entity_t system;

    /* Thread specific allocators */
    ecs_stage_allocators_t allocators;
    ecs_allocator_t allocator;

    /* Caches for query creation */
    ecs_vec_t variables;
    ecs_vec_t operations;

    /* Temporary token storage for DSL parser. This allows for parsing and 
     * interpreting a term without having to do allocations. */
    char parser_tokens[1024];
    char *parser_token; /* Pointer to next token */
};

/* Component monitor */
typedef struct ecs_monitor_t {
    ecs_vec_t queries;               /* vector<ecs_query_cache_t*> */
    bool is_dirty;                   /* Should queries be rematched? */
} ecs_monitor_t;

/* Component monitors */
typedef struct ecs_monitor_set_t {
    ecs_map_t monitors;              /* map<id, ecs_monitor_t> */
    bool is_dirty;                   /* Should monitors be evaluated? */
} ecs_monitor_set_t;

/* Data stored for id marked for deletion */
typedef struct ecs_marked_id_t {
    ecs_id_record_t *idr;
    ecs_id_t id;
    ecs_entity_t action;             /* Set explicitly for delete_with, remove_all */
    bool delete_id;
} ecs_marked_id_t;

typedef struct ecs_store_t {
    /* Entity lookup */
    ecs_entity_index_t entity_index;

    /* Tables */
    ecs_sparse_t tables;             /* sparse<table_id, ecs_table_t> */

    /* Table lookup by hash */
    ecs_hashmap_t table_map;         /* hashmap<ecs_type_t, ecs_table_t*> */

    /* Root table */
    ecs_table_t root;

    /* Observers */
    ecs_sparse_t observers;          /* sparse<table_id, ecs_table_t> */

    /* Records cache */
    ecs_vec_t records;

    /* Stack of ids being deleted. */
    ecs_vec_t marked_ids;            /* vector<ecs_marked_ids_t> */
    
    /* Entity ids associated with depth (for flat hierarchies) */
    ecs_vec_t depth_ids;
    ecs_map_t entity_to_depth; /* What it says */
} ecs_store_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

typedef struct ecs_pipeline_state_t ecs_pipeline_state_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    ecs_header_t hdr;

    /* --  Type metadata -- */
    ecs_id_record_t *id_index_lo;
    ecs_map_t id_index_hi;           /* map<id, ecs_id_record_t*> */
    ecs_sparse_t type_info;          /* sparse<type_id, type_info_t> */

    /* -- Cached handle to id records -- */
    ecs_id_record_t *idr_wildcard;
    ecs_id_record_t *idr_wildcard_wildcard;
    ecs_id_record_t *idr_any;
    ecs_id_record_t *idr_isa_wildcard;
    ecs_id_record_t *idr_childof_0;
    ecs_id_record_t *idr_childof_wildcard;
    ecs_id_record_t *idr_identifier_name;

    /* -- Mixins -- */
    ecs_world_t *self;
    ecs_observable_t observable;

    /* Unique id per generated event used to prevent duplicate notifications */
    int32_t event_id;

    /* Is entity range checking enabled? */
    bool range_check_enabled;

    /* --  Data storage -- */
    ecs_store_t store;

    /* --  Pending table event buffers -- */
    ecs_sparse_t *pending_buffer;    /* sparse<table_id, ecs_table_t*> */
    ecs_sparse_t *pending_tables;    /* sparse<table_id, ecs_table_t*> */

    /* Used to track when cache needs to be updated */
    ecs_monitor_set_t monitors;      /* map<id, ecs_monitor_t> */

    /* -- Systems -- */
    ecs_entity_t pipeline;           /* Current pipeline */

    /* -- Identifiers -- */
    ecs_hashmap_t aliases;
    ecs_hashmap_t symbols;

    /* -- Staging -- */
    ecs_stage_t **stages;            /* Stages */
    int32_t stage_count;             /* Number of stages */

    /* Internal callback for command inspection. Only one callback can be set at
     * a time. After assignment the action will become active at the start of 
     * the next frame, set by ecs_frame_begin, and will be reset by 
     * ecs_frame_end. */
    ecs_on_commands_action_t on_commands;
    ecs_on_commands_action_t on_commands_active;
    void *on_commands_ctx;
    void *on_commands_ctx_active;

    /* -- Multithreading -- */
    ecs_os_cond_t worker_cond;       /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;         /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;       /* Mutex for job_cond */
    int32_t workers_running;         /* Number of threads running */
    int32_t workers_waiting;         /* Number of workers waiting on sync */
    ecs_pipeline_state_t* pq;        /* Pointer to the pipeline for the workers to execute */
    bool workers_use_task_api;       /* Workers are short-lived tasks, not long-running threads */

    /* -- Time management -- */
    ecs_time_t world_start_time;     /* Timestamp of simulation start */
    ecs_time_t frame_start_time;     /* Timestamp of frame start */
    ecs_ftime_t fps_sleep;           /* Sleep time to prevent fps overshoot */

    /* -- Metrics -- */
    ecs_world_info_t info;

    /* -- World flags -- */
    ecs_flags32_t flags;

    /* -- Default query flags -- */
    ecs_flags32_t default_query_flags;

    /* Count that increases when component monitors change */
    int32_t monitor_generation;

    /* -- Allocators -- */
    ecs_world_allocators_t allocators; /* Static allocation sizes */
    ecs_allocator_t allocator;       /* Dynamic allocation sizes */

    void *ctx;                       /* Application context */
    void *binding_ctx;               /* Binding-specific context */

    ecs_ctx_free_t ctx_free;         /**< Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /**< Callback to free binding_ctx */

    ecs_vec_t fini_actions;          /* Callbacks to execute when world exits */
};

#endif
