#ifndef FLECS_TYPES_PRIVATE_H
#define FLECS_TYPES_PRIVATE_H

#ifndef __MACH__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#ifdef _MSC_VER
//FIXME
#else
#include <sys/param.h>  /* attempt to define endianness */
#endif
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

#include "flecs.h"
#include "datastructures/entity_index.h"
#include "flecs/private/bitset.h"
#include "flecs/private/sparse.h"
#include "flecs/private/switch_list.h"
#include "flecs/private/hashmap.h"
#include "flecs/type.h"

#define ECS_MAX_JOBS_PER_WORKER (16)

/** These values are used to verify validity of the pointers passed into the API
 * and to allow for passing a thread as a world to some API calls (this allows
 * for transparently passing thread context to API functions) */
#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_STAGE_MAGIC (0x65637374)

/* Maximum number of entities that can be added in a single operation. 
 * Increasing this value will increase consumption of stack space. */
#define ECS_MAX_ADD_REMOVE (32)

/** Type used for internal string hashmap */
typedef struct ecs_string_t {
    char *value;
    ecs_size_t length;
    uint64_t hash;
} ecs_string_t;

/** Component-specific data */
typedef struct ecs_type_info_t {
    EcsComponentLifecycle lifecycle; /* Component lifecycle callbacks */
    ecs_entity_t component;
    ecs_size_t size;
    ecs_size_t alignment;
    bool lifecycle_set;
} ecs_type_info_t;

/* Table event type for notifying tables of world events */
typedef enum ecs_table_eventkind_t {
    EcsTableQueryMatch,
    EcsTableQueryUnmatch,
    EcsTableTriggerMatch,
    EcsTableComponentInfo
} ecs_table_eventkind_t;

typedef struct ecs_table_event_t {
    ecs_table_eventkind_t kind;

    /* Query event */
    ecs_query_t *query;
    int32_t matched_table_index;

    /* Component info event */
    ecs_entity_t component;

    /* Trigger match */
    ecs_entity_t event;

    /* If the nubmer of fields gets out of hand, this can be turned into a union
     * but since events are very temporary objects, this works for now and makes
     * initializing an event a bit simpler. */
} ecs_table_event_t;    

/** A component column. */
struct ecs_column_t {
    ecs_vector_t *data;          /* Column data */
    int16_t size;                /* Column element size */
    int16_t alignment;           /* Column element alignment */
};

/** A switch column. */
typedef struct ecs_sw_column_t {
    ecs_switch_t *data;          /* Column data */
    ecs_type_t type;             /* Switch type */
} ecs_sw_column_t;

/** A bitset column. */
typedef struct ecs_bs_column_t {
    ecs_bitset_t data;           /* Column data */
} ecs_bs_column_t;

/** Stage-specific component data */
struct ecs_data_t {
    ecs_vector_t *entities;      /* Entity identifiers */
    ecs_vector_t *record_ptrs;   /* Ptrs to records in main entity index */
    ecs_column_t *columns;       /* Component columns */
    ecs_sw_column_t *sw_columns; /* Switch columns */
    ecs_bs_column_t *bs_columns; /* Bitset columns */
};

/** Flags for quickly checking for special properties of a table. */
#define EcsTableHasBuiltins         1u    /* Does table have builtin components */
#define EcsTableIsPrefab            2u    /* Does the table store prefabs */
#define EcsTableHasIsA              4u    /* Does the table type has IsA */
#define EcsTableHasModule           8u    /* Does the table have module data */
#define EcsTableHasXor              32u   /* Does the table type has XOR */
#define EcsTableIsDisabled          64u   /* Does the table type has EcsDisabled */
#define EcsTableHasCtors            128u
#define EcsTableHasDtors            256u
#define EcsTableHasCopy             512u
#define EcsTableHasMove             1024u
#define EcsTableHasOnAdd            2048u
#define EcsTableHasOnRemove         4096u
#define EcsTableHasOnSet            8192u
#define EcsTableHasUnSet            16384u
#define EcsTableHasMonitors         32768u
#define EcsTableHasSwitch           65536u
#define EcsTableHasDisabled         131072u

/* Composite constants */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasSwitch | EcsTableHasDisabled)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasSwitch | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet | EcsTableHasMonitors)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet | EcsTableHasMonitors)

/** Edge used for traversing the table graph. */
typedef struct ecs_edge_t {
    ecs_table_t *add;            /* Edges traversed when adding */
    ecs_table_t *remove;         /* Edges traversed when removing */
} ecs_edge_t;

/** Quey matched with table with backref to query table administration.
 * This type is used to store a matched query together with the array index of
 * where the table is stored in the query administration. This type is used when
 * an action that originates on a table needs to invoke a query (system) and a
 * fast lookup is required for the query administration, as is the case with
 * OnSet and Monitor systems. */
typedef struct ecs_matched_query_t {
    ecs_query_t *query;          /* The query matched with the table */
    int32_t matched_table_index; /* Table index in the query type */
} ecs_matched_query_t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing queries */
struct ecs_table_t {
    uint64_t id;                     /* Table id in sparse set */
    ecs_type_t type;                 /* Identifies table type in type_index */
    ecs_flags32_t flags;             /* Flags for testing table properties */
    int32_t column_count;            /* Number of data columns in table */

    ecs_data_t storage;              /* Component storage */
    ecs_type_info_t **c_info;        /* Cached pointers to component info */

    ecs_edge_t *lo_edges;            /* Edges to other tables */
    ecs_map_t *hi_edges;

    ecs_vector_t *queries;           /* Queries matched with table */
    ecs_vector_t *monitors;          /* Monitor systems matched with table */
    ecs_vector_t **on_set;           /* OnSet systems, broken up by column */
    ecs_vector_t *on_set_all;        /* All OnSet systems */
    ecs_vector_t *on_set_override;   /* All OnSet systems with overrides */
    ecs_vector_t *un_set_all;        /* All UnSet systems */

    int32_t *dirty_state;            /* Keep track of changes in columns */
    int32_t alloc_count;             /* Increases when columns are reallocd */

    int32_t sw_column_count;
    int32_t sw_column_offset;
    int32_t bs_column_count;
    int32_t bs_column_offset;

    int32_t lock;
};

/* Sparse query column */
typedef struct flecs_sparse_column_t {
    ecs_sw_column_t *sw_column;
    ecs_entity_t sw_case; 
    int32_t signature_column_index;
} flecs_sparse_column_t;

/* Bitset query column */
typedef struct flecs_bitset_column_t {
    ecs_bs_column_t *bs_column;
    int32_t column_index;
} flecs_bitset_column_t;

/** Type containing data for a table matched with a query. */
typedef struct ecs_matched_table_t {
    int32_t *columns;         /* Mapping from query terms to table columns */
    ecs_table_t *table;       /* The current table. */
    ecs_data_t *data;         /* Table component data */
    ecs_id_t *ids;            /* Resolved (component) ids for current table */
    ecs_entity_t *subjects;   /* Subjects (sources) of ids */
    ecs_size_t *sizes;        /* Sizes for ids for current table */
    ecs_ref_t *references;    /* Cached components for non-this terms */

    ecs_vector_t *sparse_columns;  /* Column ids of sparse columns */
    ecs_vector_t *bitset_columns;  /* Column ids with disabled flags */
    int32_t *monitor;              /* Used to monitor table for changes */
    int32_t rank;                  /* Rank used to sort tables */
} ecs_matched_table_t;

/** Type used to track location of table in queries' table lists.
 * When a table becomes empty or non-empty a signal is sent to a query, which
 * moves the table to or from an empty list. While this ensures that when 
 * iterating no time is spent on iterating over empty tables, doing a linear
 * search for the table in either list can take a significant amount of time if
 * a query is matched with many tables.
 *
 * To avoid a linear search, the query has a map with table indices that can
 * return the location of the table in either list in constant time.
 *
 * If a table is matched multiple times by a query, such as can happen when a
 * query matches pairs, a table can occupy multiple indices.
 */
typedef struct ecs_table_indices_t {
    int32_t *indices; /* If indices are negative, table is in empty list */
    int32_t count;
} ecs_table_indices_t;

/** Type storing an entity range within a table.
 * This type is used for iterating in orer across archetypes. A sorting function
 * constructs a list of the ranges across archetypes that are in order so that
 * when the query iterates over the archetypes, it only needs to iterate the
 * list of ranges. */
typedef struct ecs_table_slice_t {
    ecs_matched_table_t *table;      /* Reference to the matched table */
    int32_t start_row;               /* Start of range  */
    int32_t count;                   /* Number of entities in range */
} ecs_table_slice_t;

#define EcsQueryNeedsTables (1)      /* Query needs matching with tables */ 
#define EcsQueryMonitor (2)          /* Query needs to be registered as a monitor */
#define EcsQueryOnSet (4)            /* Query needs to be registered as on_set system */
#define EcsQueryUnSet (8)            /* Query needs to be registered as un_set system */
#define EcsQueryMatchDisabled (16)   /* Does query match disabled */
#define EcsQueryMatchPrefab (32)     /* Does query match prefabs */
#define EcsQueryHasRefs (64)         /* Does query have references */
#define EcsQueryHasTraits (128)      /* Does query have pairs */
#define EcsQueryIsSubquery (256)     /* Is query a subquery */
#define EcsQueryIsOrphaned (512)     /* Is subquery orphaned */
#define EcsQueryHasOutColumns (1024) /* Does query have out columns */
#define EcsQueryHasOptional (2048)   /* Does query have optional columns */

#define EcsQueryNoActivation (EcsQueryMonitor | EcsQueryOnSet | EcsQueryUnSet)

/* Query event type for notifying queries of world events */
typedef enum ecs_query_eventkind_t {
    EcsQueryTableMatch,
    EcsQueryTableEmpty,
    EcsQueryTableNonEmpty,
    EcsQueryTableRematch,
    EcsQueryTableUnmatch,
    EcsQueryOrphan
} ecs_query_eventkind_t;

typedef struct ecs_query_event_t {
    ecs_query_eventkind_t kind;
    ecs_table_t *table;
    ecs_query_t *parent_query;
} ecs_query_event_t;

/** Query that is automatically matched against active tables */
struct ecs_query_t {
    /* Signature of query */
    ecs_filter_t filter;

    /* Reference to world */
    ecs_world_t *world;

    /* Tables matched with query */
    ecs_vector_t *tables;
    ecs_vector_t *empty_tables;
    ecs_map_t *table_indices;

    /* Handle to system (optional) */
    ecs_entity_t system;   

    /* Used for sorting */
    ecs_entity_t order_by_component;
    ecs_order_by_action_t order_by;
    ecs_vector_t *table_slices;     

    /* Used for table sorting */
    ecs_entity_t group_by_id;
    ecs_group_by_action_t group_by;
    void *group_by_ctx;
    ecs_ctx_free_t group_by_ctx_free;

    /* Subqueries */
    ecs_query_t *parent;
    ecs_vector_t *subqueries;

    /* The query kind determines how it is registered with tables */
    ecs_flags32_t flags;

    uint64_t id;                /* Id of query in query storage */
    int32_t cascade_by;         /* Identify CASCADE column */
    int32_t match_count;        /* How often have tables been (un)matched */
    int32_t prev_match_count;   /* Used to track if sorting is needed */

    bool needs_reorder;         /* Whether next iteration should reorder */
    bool constraints_satisfied; /* Are all term constraints satisfied */
};

/** Event mask */
#define EcsEventAdd    (1)
#define EcsEventRemove (2)

/** Triggers for a specific id */
typedef struct ecs_id_trigger_t {
    ecs_map_t *on_add_triggers;
    ecs_map_t *on_remove_triggers;
    ecs_map_t *on_set_triggers;
    ecs_map_t *un_set_triggers;
} ecs_id_trigger_t;

/** Keep track of how many [in] columns are active for [out] columns of OnDemand
 * systems. */
typedef struct ecs_on_demand_out_t {
    ecs_entity_t system;    /* Handle to system */
    int32_t count;          /* Total number of times [out] columns are used */
} ecs_on_demand_out_t;

/** Keep track of which OnDemand systems are matched with which [in] columns */
typedef struct ecs_on_demand_in_t {
    int32_t count;          /* Number of active systems with [in] column */
    ecs_vector_t *systems;  /* Systems that have this column as [out] column */
} ecs_on_demand_in_t;

/** Types for deferred operations */
typedef enum ecs_op_kind_t {
    EcsOpNew,
    EcsOpClone,
    EcsOpBulkNew,
    EcsOpAdd,
    EcsOpRemove,   
    EcsOpSet,
    EcsOpMut,
    EcsOpModified,
    EcsOpDelete,
    EcsOpClear,
    EcsOpEnable,
    EcsOpDisable
} ecs_op_kind_t;

typedef struct ecs_op_1_t {
    ecs_entity_t entity;        /* Entity id */
    void *value;                /* Value (used for set / get_mut) */
    ecs_size_t size;            /* Size of value */
    bool clone_value;           /* Clone entity with value (used for clone) */ 
} ecs_op_1_t;

typedef struct ecs_op_n_t {
    ecs_entity_t *entities;  
    int32_t count;
} ecs_op_n_t;

typedef struct ecs_op_t {
    ecs_op_kind_t kind;         /* Operation kind */    
    ecs_entity_t component;     /* Single component (components.count = 1) */
    ecs_ids_t components;       /* Multiple components */
    union {
        ecs_op_1_t _1;
        ecs_op_n_t _n;
    } is;
} ecs_op_t;

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows flecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
struct ecs_stage_t {
    int32_t magic;               /* Magic number to verify thread pointer */
    int32_t id;                  /* Unique id that identifies the stage */

    /* Are operations deferred? */
    int32_t defer;
    ecs_vector_t *defer_queue;

    ecs_world_t *thread_ctx;     /* Points to stage when a thread stage */
    ecs_world_t *world;          /* Reference to world */
    ecs_os_thread_t thread;      /* Thread handle (0 if no threading is used) */

    /* One-shot actions to be executed after the merge */
    ecs_vector_t *post_frame_actions;

    /* Namespacing */
    ecs_table_t *scope_table;    /* Table for current scope */
    ecs_entity_t scope;          /* Entity of current scope */
    ecs_entity_t with;           /* Id to add by default to new entities */

    /* Properties */
    bool auto_merge;             /* Should this stage automatically merge? */
    bool asynchronous;           /* Is stage asynchronous? (write only) */
};

/* Component monitor */
typedef struct ecs_monitor_t {
    ecs_vector_t *queries;       /* vector<ecs_query_t*> */
    bool is_dirty;               /* Should queries be rematched? */
} ecs_monitor_t;

/* Component monitors */
typedef struct ecs_monitor_set_t {
    ecs_map_t *monitors;         /* map<id, ecs_monitor_t> */
    bool is_dirty;               /* Should monitors be evaluated? */
} ecs_monitor_set_t;

/* Relation monitors. TODO: implement generic monitor mechanism */
typedef struct ecs_relation_monitor_t {
    ecs_map_t *monitor_sets;     /* map<relation_id, ecs_monitor_set_t> */
    bool is_dirty;               /* Should monitor sets be evaluated? */
} ecs_relation_monitor_t;

/* Payload for table index which returns all tables for a given component, with
 * the column of the component in the table. */
typedef struct ecs_table_record_t {
    ecs_table_t *table;
    int32_t column;
    int32_t count;
} ecs_table_record_t;

/* Payload for id index which contains all datastructures for an id. */
struct ecs_id_record_t {
    /* All tables that contain the id */
    ecs_map_t *table_index;         /* map<table_id, ecs_table_record_t> */

    ecs_entity_t on_delete;         /* Cleanup action for removing id */
    ecs_entity_t on_delete_object;  /* Cleanup action for removing object */
};

typedef struct ecs_store_t {
    /* Entity lookup */
    ecs_sparse_t *entity_index; /* sparse<entity, ecs_record_t> */

    /* Table lookup by id */
    ecs_sparse_t *tables;       /* sparse<table_id, ecs_table_t> */

    /* Table lookup by hash */
    ecs_hashmap_t table_map;    /* hashmap<ecs_ids_t, ecs_table_t*> */

    /* Root table */
    ecs_table_t root;
} ecs_store_t;

/** Supporting type to store looked up or derived entity data */
typedef struct ecs_entity_info_t {
    ecs_record_t *record;       /* Main stage record in entity index */
    ecs_table_t *table;         /* Table. Not set if entity is empty */
    ecs_data_t *data;           /* Stage-specific table columns */
    int32_t row;                /* Actual row (stripped from is_watched bit) */
    bool is_watched;            /* Is entity being watched */
} ecs_entity_info_t;

/** Supporting type to store looked up component data in specific table */
typedef struct ecs_column_info_t {
    ecs_entity_t id;
    const ecs_type_info_t *ci;
    int32_t column;
} ecs_column_info_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

/* Alias */
typedef struct ecs_alias_t {
    char *name;
    ecs_entity_t entity;
} ecs_alias_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    int32_t magic;               /* Magic number to verify world pointer */

    /* --  Type metadata -- */

    ecs_map_t *id_index;         /* map<id, ecs_id_record_t> */
    ecs_map_t *id_triggers;      /* map<id, ecs_id_trigger_t> */
    ecs_sparse_t *type_info;     /* sparse<type_id, type_info_t> */

    /* Is entity range checking enabled? */
    bool range_check_enabled;


    /* --  Data storage -- */

    ecs_store_t store;


    /* --  Storages for API objects -- */

    ecs_sparse_t *queries;       /* sparse<query_id, ecs_query_t> */
    ecs_sparse_t *triggers;      /* sparse<query_id, ecs_trigger_t> */
    ecs_sparse_t *observers;     /* sparse<query_id, ecs_observer_t> */
    

    /* Keep track of components that were added/removed to/from monitored
     * entities. Monitored entities are entities that a query has matched with
     * specifically, as is the case with PARENT / CASCADE columns, FromEntity
     * columns and columns matched from prefabs. 
     * When these entities change type, queries may have to be rematched.
     * Queries register themselves as component monitors for specific components
     * and when these components change they are rematched. The component 
     * monitors are evaluated during a merge. */
    ecs_relation_monitor_t monitors;

    /* -- Systems -- */
    
    ecs_entity_t pipeline;             /* Current pipeline */
    ecs_map_t *on_activate_components; /* Trigger on activate of [in] column */
    ecs_map_t *on_enable_components;   /* Trigger on enable of [in] column */
    ecs_vector_t *fini_tasks;          /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    ecs_map_t *type_handles;     /* Handles to named types */


    /* -- Aliasses -- */

    ecs_hashmap_t aliases;
    ecs_hashmap_t symbols;


    /* -- Staging -- */

    ecs_stage_t stage;           /* Main storage */
    ecs_vector_t *worker_stages; /* Stages for threads */


    /* -- Hierarchy administration -- */

    const char *name_prefix;     /* Remove prefix from C names in modules */


    /* -- Multithreading -- */

    ecs_os_cond_t worker_cond;   /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;     /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;   /* Mutex for job_cond */
    int32_t workers_running;     /* Number of threads running */
    int32_t workers_waiting;     /* Number of workers waiting on sync */


    /* -- Time management -- */

    ecs_time_t world_start_time; /* Timestamp of simulation start */
    ecs_time_t frame_start_time; /* Timestamp of frame start */
    FLECS_FLOAT fps_sleep;       /* Sleep time to prevent fps overshoot */


    /* -- Metrics -- */

    ecs_world_info_t stats;


    /* -- Settings from command line arguments -- */

    int arg_fps;
    int arg_threads;


    /* -- World lock -- */

    ecs_os_mutex_t mutex;        /* Locks the world if locking enabled */
    ecs_os_mutex_t thr_sync;     /* Used to signal threads at end of frame */
    ecs_os_cond_t thr_cond;      /* Used to signal threads at end of frame */


    /* -- Defered operation count -- */
    
    int32_t new_count;
    int32_t bulk_new_count;
    int32_t delete_count;
    int32_t clear_count;
    int32_t add_count;
    int32_t remove_count;
    int32_t set_count;
    int32_t discard_count;


    /* -- World state -- */

    bool quit_workers;           /* Signals worker threads to quit */
    bool is_readonly;            /* Is world being progressed */
    bool is_fini;                /* Is the world being cleaned up? */
    bool measure_frame_time;     /* Time spent on each frame */
    bool measure_system_time;    /* Time spent by each system */
    bool should_quit;            /* Did a system signal that app should quit */
    bool locking_enabled;        /* Lock world when in progress */ 

    void *context;               /* Application context */
    ecs_vector_t *fini_actions;  /* Callbacks to execute when world exits */
};

#endif
