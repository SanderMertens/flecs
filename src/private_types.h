#ifndef FLECS_TYPES_PRIVATE_H
#define FLECS_TYPES_PRIVATE_H

#ifndef __MACH__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <stdlib.h>
#include <limits.h>
#ifndef NDEBUG
#include <stdio.h> /* easier debugging, throws warning in release for printfs */
#endif

#include "flecs.h"
#include "datastructures/entity_index.h"
#include "flecs/private/bitset.h"
#include "flecs/private/switch_list.h"

#define ECS_MAX_JOBS_PER_WORKER (16)
#define ECS_MAX_ADD_REMOVE (32)

/* Magic number for a flecs object */
#define ECS_OBJECT_MAGIC (0x6563736f)

/* Magic number to identify the type of the object */
#define ECS_ecs_world_t_MAGIC     (0x65637377)
#define ECS_ecs_stage_t_MAGIC     (0x65637373)
#define ECS_ecs_query_t_MAGIC     (0x65637371)
#define ECS_ecs_rule_t_MAGIC      (0x65637375)
#define ECS_ecs_table_t_MAGIC     (0x65637374)
#define ECS_ecs_filter_t_MAGIC    (0x65637366)
#define ECS_ecs_trigger_t_MAGIC   (0x65637372)
#define ECS_ecs_observer_t_MAGIC  (0x65637362)

/* Mixin kinds */
typedef enum ecs_mixin_kind_t {
    EcsMixinWorld,
    EcsMixinObservable,
    EcsMixinIterable,
    EcsMixinBase,        /* If mixin can't be found in object, look in base */
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

/* Types that have no mixins */
#define ecs_table_t_mixins (&(ecs_mixins_t){ NULL })

/** Type used for internal string hashmap */
typedef struct ecs_hashed_string_t {
    char *value;
    ecs_size_t length;
    uint64_t hash;
} ecs_hashed_string_t;

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
    EcsTableTriggersForId,
    EcsTableNoTriggersForId,
    EcsTableComponentInfo
} ecs_table_eventkind_t;

typedef struct ecs_table_event_t {
    ecs_table_eventkind_t kind;

    /* Query event */
    ecs_query_t *query;

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
    ecs_table_t *type;           /* Table with switch type */
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
#define EcsTableHasIsA              4u    /* Does the table have IsA relation */
#define EcsTableHasChildOf          8u    /* Does the table type ChildOf relation */
#define EcsTableHasPairs            16u   /* Does the table type have pairs */
#define EcsTableHasModule           32u   /* Does the table have module data */
#define EcsTableHasXor              64u   /* Does the table type has XOR */
#define EcsTableIsDisabled          128u   /* Does the table type has EcsDisabled */
#define EcsTableHasCtors            256u
#define EcsTableHasDtors            512u
#define EcsTableHasCopy             1024u
#define EcsTableHasMove             2048u
#define EcsTableHasOnAdd            4096u
#define EcsTableHasOnRemove         8192u
#define EcsTableHasOnSet            16384u
#define EcsTableHasUnSet            32768u
#define EcsTableHasSwitch           65536u
#define EcsTableHasDisabled         131072u

/* Composite constants */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasSwitch | EcsTableHasDisabled)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasSwitch | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet)

/** Cache of added/removed components for non-trivial edges between tables */
typedef struct ecs_table_diff_t {
    ecs_ids_t added;         /* Components added between tables */
    ecs_ids_t removed;       /* Components removed between tables */
    ecs_ids_t on_set;        /* OnSet from exposing/adding base components */
    ecs_ids_t un_set;        /* UnSet from hiding/removing base components */   
} ecs_table_diff_t;

/** Single edge. */
typedef struct ecs_edge_t {
    ecs_table_t *next;       /* Edge traversed when adding */
    int32_t diff_index;      /* Index into diff vector, if non trivial edge */
} ecs_edge_t;

/* Edges to other tables. */
typedef struct ecs_graph_edges_t {
    ecs_edge_t *lo; /* Small array optimized for low edges */
    ecs_map_t *hi;  /* Map for hi edges */
} ecs_graph_edges_t;

/* Table graph node */
typedef struct ecs_graph_node_t {
    /* Add & remove edges to other tables */
    ecs_graph_edges_t add;
    ecs_graph_edges_t remove;

    /* Metadata that keeps track of id diffs for non-trivial edges */ 
    ecs_vector_t *diffs;
} ecs_graph_node_t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing queries */
struct ecs_table_t {
    uint64_t id;                     /* Table id in sparse set */
    ecs_type_t type;                 /* Identifies table type in type_index */
    ecs_flags32_t flags;             /* Flags for testing table properties */

    ecs_table_t *storage_table;      /* Table w/type without tags */
    ecs_type_t storage_type;         /* Storage table type (prevents indirection) */
    int32_t *storage_map;            /* Map type <-> storage type
                                      *  - 0..count(T):         type -> storage_type
                                      *  - count(T)..count(S):  storage_type -> type
                                      */
                                     
    ecs_graph_node_t node;           /* Graph node */
    ecs_data_t storage;              /* Component storage */
    ecs_type_info_t **c_info;        /* Cached pointers to component info */

    int32_t *dirty_state;            /* Keep track of changes in columns */
    int32_t alloc_count;             /* Increases when columns are reallocd */

    int32_t sw_column_count;
    int32_t sw_column_offset;
    int32_t bs_column_count;
    int32_t bs_column_offset;

    int32_t lock;
    int32_t refcount;
};

/** Table cache */
typedef struct ecs_table_cache_t {
    ecs_map_t *index; /* <table_id, index> */
    ecs_vector_t *tables;
    ecs_vector_t *empty_tables;
    ecs_size_t size;
    ecs_poly_t *parent;
    void(*free_payload)(ecs_poly_t*, void*);
} ecs_table_cache_t;

/** Must appear as first member in payload of table cache */
typedef struct ecs_table_cache_hdr_t {
    ecs_table_t *table;
    bool empty;
} ecs_table_cache_hdr_t;

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

typedef struct ecs_query_table_match_t ecs_query_table_match_t;

/** List node used to iterate tables in a query.
 * The list of nodes dictates the order in which tables should be iterated by a
 * query. A single node may refer to the table multiple times with different
 * offset/count parameters, which enables features such as sorting. */
struct ecs_query_table_node_t {
    ecs_query_table_match_t *match;  /* Reference to the match */
    int32_t offset;                  /* Starting point in table  */
    int32_t count;                   /* Number of entities to iterate in table */
    ecs_query_table_node_t *next, *prev;
};

/** Type containing data for a table matched with a query. 
 * A single table can have multiple matches, if the query contains wildcards. */
struct ecs_query_table_match_t {
    ecs_query_table_node_t node; /* Embedded list node */

    ecs_table_t *table;       /* The current table. */
    int32_t *columns;         /* Mapping from query terms to table columns */
    ecs_id_t *ids;            /* Resolved (component) ids for current table */
    ecs_entity_t *subjects;   /* Subjects (sources) of ids */
    ecs_size_t *sizes;        /* Sizes for ids for current table */
    ecs_ref_t *references;    /* Cached components for non-this terms */

    ecs_vector_t *sparse_columns;  /* Column ids of sparse columns */
    ecs_vector_t *bitset_columns;  /* Column ids with disabled flags */

    uint64_t group_id;        /* Value used to organize tables in groups */

    /* Next match in cache for same table (includes empty tables) */
    ecs_query_table_match_t *next_match;

    int32_t *monitor;                /* Used to monitor table for changes */
};

/** A single table can occur multiple times in the cache when a term matches
 * multiple table columns. */
typedef struct ecs_query_table_t {
    ecs_table_cache_hdr_t hdr;       /* Header for ecs_table_cache_t */
    ecs_query_table_match_t *first;  /* List with matches for table */
    ecs_query_table_match_t *last;   /* Last discovered match for table */
} ecs_query_table_t;

/** Points to the beginning & ending of a query group */
typedef struct ecs_query_table_list_t {
    ecs_query_table_node_t *first;
    ecs_query_table_node_t *last;
    int32_t count;
} ecs_query_table_list_t;

#define EcsQueryNeedsTables (1)      /* Query needs matching with tables */ 
#define EcsQueryMatchDisabled (16)   /* Does query match disabled */
#define EcsQueryMatchPrefab (32)     /* Does query match prefabs */
#define EcsQueryHasRefs (64)         /* Does query have references */
#define EcsQueryHasTraits (128)      /* Does query have pairs */
#define EcsQueryIsSubquery (256)     /* Is query a subquery */
#define EcsQueryIsOrphaned (512)     /* Is subquery orphaned */
#define EcsQueryHasOutColumns (1024) /* Does query have out columns */
#define EcsQueryHasOptional (2048)   /* Does query have optional columns */
#define EcsQueryHasMonitor (4096)    /* Does query track changes */

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

/** Query that is automatically matched against tables */
struct ecs_query_t {
    ecs_header_t hdr;

    /* Query filter */
    ecs_filter_t filter;

    /* Query observer */
    ecs_entity_t observer;

    /* Tables matched with query */
    ecs_table_cache_t cache;

    /* Linked list with all matched non-empty tables, in iteration order */
    ecs_query_table_list_t list;

    /* Contains head/tail to nodes of query groups (if group_by is used) */
    ecs_map_t *groups;

    /* Handle to system (optional) */
    ecs_entity_t system;

    /* Used for sorting */
    ecs_entity_t order_by_component;
    ecs_order_by_action_t order_by;
    ecs_vector_t *table_slices;     

    /* Used for grouping */
    ecs_entity_t group_by_id;
    ecs_group_by_action_t group_by;
    void *group_by_ctx;
    ecs_ctx_free_t group_by_ctx_free;

    /* Subqueries */
    ecs_query_t *parent;
    ecs_vector_t *subqueries;

    /* Flags for query properties */
    ecs_flags32_t flags;

    uint64_t id;                /* Id of query in query storage */
    int32_t cascade_by;         /* Identify cascade column */
    int32_t match_count;        /* How often have tables been (un)matched */
    int32_t prev_match_count;   /* Used to track if sorting is needed */
    bool constraints_satisfied; /* Are all term constraints satisfied */

    /* Mixins */
    ecs_world_t *world;
    ecs_iterable_t iterable;
};

/** All triggers for a specific (component) id */
typedef struct ecs_event_id_record_t {
    /* Triggers for Self */
    ecs_map_t *triggers; /* map<trigger_id, trigger_t> */

    /* Triggers for SuperSet, SubSet */
    ecs_map_t *set_triggers; /* map<trigger_id, trigger_t> */

    /* Triggers for Self with non-This subject */
    ecs_map_t *entity_triggers; /* map<trigger_id, trigger_t> */

    /* Number of active triggers for (component) id */
    int32_t trigger_count;
} ecs_event_id_record_t;

/** All triggers for a specific event */
typedef struct ecs_event_record_t {
    ecs_map_t *event_ids;     /* map<id, ecs_event_id_record_t> */
} ecs_event_record_t;

/** Types for deferred operations */
typedef enum ecs_defer_op_kind_t {
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
    EcsOpOnDeleteAction,
    EcsOpEnable,
    EcsOpDisable
} ecs_defer_op_kind_t;

typedef struct ecs_defer_op_1_t {
    ecs_entity_t entity;        /* Entity id */
    void *value;                /* Value (used for set / get_mut) */
    ecs_size_t size;            /* Size of value */
    bool clone_value;           /* Clone entity with value (used for clone) */ 
} ecs_defer_op_1_t;

typedef struct ecs_defer_op_n_t {
    ecs_entity_t *entities;  
    int32_t count;
} ecs_defer_op_n_t;

typedef struct ecs_defer_op_t {
    ecs_defer_op_kind_t kind;         /* Operation kind */    
    ecs_id_t id;                /* (Component) id */
    union {
        ecs_defer_op_1_t _1;
        ecs_defer_op_n_t _n;
    } is;
} ecs_defer_op_t;

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows flecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
struct ecs_stage_t {
    ecs_header_t hdr;

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
    ecs_entity_t scope;          /* Entity of current scope */
    ecs_entity_t with;           /* Id to add by default to new entities */
    ecs_entity_t base;           /* Currently instantiated top-level base */
    ecs_entity_t *lookup_path;   /* Search path used by lookup operations */

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
    ecs_table_cache_hdr_t hdr;
    ecs_table_t *table;
    int32_t column;
    int32_t count;
} ecs_table_record_t;

/* Payload for id index which contains all datastructures for an id. */
struct ecs_id_record_t {
    /* Cache with all tables that contain the id */
    ecs_table_cache_t cache; /* table_cache<ecs_table_record_t> */

    /* All tables for which an outgoing (add) edge to the id was created */
    ecs_map_t *add_refs;

    /* All tables for which an incoming (remove) edge to the id was created */
    ecs_map_t *remove_refs;

    /* Flags for id */
    ecs_flags32_t flags;

    uint64_t id; /* Id to element in storage */
};

/* Convenience struct to iterate table array for id */
typedef struct ecs_table_iter_t {
    const ecs_table_record_t *begin;
    const ecs_table_record_t *end;
    const ecs_table_record_t *cur;
} ecs_table_iter_t;

typedef struct ecs_store_t {
    /* Entity lookup */
    ecs_sparse_t *entity_index; /* sparse<entity, ecs_record_t> */

    /* Table lookup by id */
    ecs_sparse_t *tables;       /* sparse<table_id, ecs_table_t> */

    /* Table lookup by hash */
    ecs_hashmap_t table_map;    /* hashmap<ecs_ids_t, ecs_table_t*> */

    /* Root table */
    ecs_table_t root;

    /* Reusable id sequence storage to prevent having to do allocs
     * when generating an id list for a new table */
    ecs_ids_t id_cache;
} ecs_store_t;

/** Supporting type to store looked up or derived entity data */
typedef struct ecs_entity_info_t {
    ecs_record_t *record;       /* Main stage record in entity index */
    ecs_table_t *table;         /* Table. Not set if entity is empty */
    ecs_data_t *data;           /* Stage-specific table columns */
    int32_t row;                /* Row in table */
    uint32_t row_flags;         /* Row flags (used to track observables) */
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

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    ecs_header_t hdr;

    /* --  Type metadata -- */
    ecs_map_t *id_index;         /* map<id, ecs_id_record_t*> */
    ecs_sparse_t *type_info;     /* sparse<type_id, type_info_t> */

    /* Cached handle to (IsA, *) */
    ecs_id_record_t *idr_isa_wildcard;

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


    /* --  Storages for API objects -- */

    ecs_sparse_t *queries;         /* sparse<query_id, ecs_query_t> */
    ecs_sparse_t *triggers;        /* sparse<query_id, ecs_trigger_t> */
    ecs_sparse_t *observers;       /* sparse<query_id, ecs_observer_t> */
    ecs_sparse_t *id_records;      /* sparse<idr_id, ecs_id_record_t> */


    /* --  Pending table event buffers -- */

    ecs_sparse_t *pending_buffer;  /* sparse<table_id, ecs_table_t*> */
    ecs_sparse_t *pending_tables;  /* sparse<table_id, ecs_table_t*> */
    

    /* Keep track of components that were added/removed to/from monitored
     * entities. Monitored entities are entities that are directly referenced by
     * a query, either explicitly (e.g. Position(Foo)) or implicitly 
     * (Position(supser)).
     * When these entities change type, queries may have to be rematched.
     * Queries register themselves as component monitors for specific components
     * and when these components change they are rematched. The component 
     * monitors are evaluated during a merge. */
    ecs_relation_monitor_t monitors;


    /* -- Systems -- */

    ecs_entity_t pipeline;             /* Current pipeline */
    ecs_vector_t *fini_tasks;          /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    ecs_map_t *type_handles;     /* Handles to named types */


    /* -- Identifiers -- */

    ecs_hashmap_t aliases;
    ecs_hashmap_t symbols;
    const char *name_prefix;     /* Remove prefix from C names in modules */


    /* -- Staging -- */

    ecs_stage_t stage;           /* Main stage */
    ecs_vector_t *worker_stages; /* Stages for threads */


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
