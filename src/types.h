#ifndef FLECS_TYPES_PRIVATE_H
#define FLECS_TYPES_PRIVATE_H

#ifndef __MACH__
#define _POSIX_C_SOURCE 200809L
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
#include "flecs/private/entity_index.h"
#include "flecs/private/table.h"
#include "flecs/private/strbuf.h"

#define ECS_MAX_JOBS_PER_WORKER (16)

/** Entity id's higher than this number will be stored in a map instead of a
 * sparse set. Increasing this value can improve performance at the cost of
 * (significantly) higher memory usage. */
#define ECS_HI_ENTITY_ID (1000000)

/** These values are used to verify validity of the pointers passed into the API
 * and to allow for passing a thread as a world to some API calls (this allows
 * for transparently passing thread context to API functions) */
#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_THREAD_MAGIC (0x65637374)

/* Maximum number of entities that can be added in a single operation. 
 * Increasing this value will increase consumption of stack space. */
#define ECS_MAX_ADD_REMOVE (32)

/* Maximum length of an entity name, including 0 terminator */
#define ECS_MAX_NAME_LENGTH (64)

/* Simple bitmask structure to store a set of components. This is used amongst
 * others to keep track of which components have been overridden from a base. */
typedef uint64_t ecs_comp_mask_t[ECS_HI_COMPONENT_ID / 64];

/** Callback used by the system signature expression parser. */
typedef int (*ecs_parse_action_t)(
    ecs_world_t *world,                 
    const char *id,
    const char *expr,
    int column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t flags,
    const char *component,
    const char *source,
    void *ctx);

/** Instruction data for pipeline.
 * This type is the element type in the "ops" vector of a pipeline and contains
 * information about the set of systems that need to be ran before a merge. */
typedef struct ecs_pipeline_op_t {
    int32_t count;              /**< Number of systems to run before merge */
} ecs_pipeline_op_t;

/** A component array in a table. */
struct ecs_column_t {
    ecs_vector_t *data;         /**< Column data */
    uint16_t size;              /**< Column element size */
    uint16_t alignment;         /**< Column element alignment */
};

/** Stage-specific component data */
struct ecs_data_t {
    ecs_vector_t *entities;     /**< Entity identifiers */
    ecs_vector_t *record_ptrs;  /**< Pointers to records in main entity index */
    ecs_column_t *columns;      /**< Component data */
    bool marked_dirty;          /**< Was table marked dirty by stage? */  
};

/** Small footprint data structure for storing data associated with a table. */
typedef struct ecs_table_leaf_t {
    ecs_table_t *table;
    ecs_type_t type;
    ecs_data_t *data;
} ecs_table_leaf_t;

/** Flags for quickly checking for special properties of a table. */
typedef enum ecs_table_flags_t {
    EcsTableHasBuiltins = 1,        /**< Does table have builtin components */
    EcsTableIsPrefab = 2,           /**< Does the table store prefabs */
    EcsTableHasBase = 4,          /**< Does the table type has INSTANCEOF */
    EcsTableHasParent = 8,          /**< Does the table type has CHILDOF */
    EcsTableHasComponentData = 16,  /**< Does the table have component data */
    EcsTableHasXor = 32,            /**< Does the table type has XOR */
    EcsTableIsDisabled = 64         /**< Does the table type has EcsDisabled */
} ecs_table_flags_t;

/** Edge used for traversing the table graph. */
typedef struct ecs_edge_t {
    ecs_table_t *add;               /**< Edges traversed when adding */
    ecs_table_t *remove;            /**< Edges traversed when removing */
} ecs_edge_t;

/** Quey matched with table with backref to query table administration.
 * This type is used to store a matched query together with the array index of
 * where the table is stored in the query administration. This type is used when
 * an action that originates on a table needs to invoke a query (system) and a
 * fast lookup is required for the query administration, as is the case with
 * OnSet and Monitor systems. */
typedef struct ecs_matched_query_t {
    ecs_query_t *query;             /**< The query matched with the table */
    int32_t matched_table_index;    /**< Table index in the query type */
} ecs_matched_query_t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing column systems */
struct ecs_table_t {
    ecs_type_t type;                 /**< Identifies table type in type_index */

    ecs_edge_t *lo_edges;            /**< Edges to low entity ids */
    ecs_map_t *hi_edges;             /**< Edges to high entity ids */

    ecs_vector_t *data;              /**< Data per stage */

    ecs_vector_t *queries;           /**< Queries matched with table */
    ecs_vector_t *monitors;          /**< Monitor systems matched with table */
    ecs_vector_t **on_set;           /**< OnSet systems, broken up by column */
    ecs_vector_t *on_set_all;        /**< All OnSet systems */
    ecs_vector_t *on_set_override;   /**< All OnSet systems with overrides */
    ecs_vector_t *un_set_all;        /**< All OnSet systems */

    int32_t *dirty_state;            /**< Keep track of changes in columns */
    int32_t alloc_count;             /**< Increases when columns are reallocd */

    ecs_table_flags_t flags;         /**< Flags for testing table properties */
    int32_t column_count;            /**< Number of data columns in table */
};

/** Type containing data for a table matched with a query. */
typedef struct ecs_matched_table_t {
    ecs_table_t *table;            /**< Reference to the table */
    int32_t *columns;              /**< Mapping of system columns to table */
    ecs_entity_t *components;      /**< Actual components of system columns */
    ecs_vector_t *references;      /**< Reference columns and cached pointers */
    int32_t rank;                  /**< Rank used to sort tables */
    int32_t *monitor;              /**< Used to monitor table for changes */
} ecs_matched_table_t;

/** Type storing an entity range within a table.
 * This type is used for iterating in orer across archetypes. A sorting function
 * constructs a list of the ranges across archetypes that are in order so that
 * when the query iterates over the archetypes, it only needs to iterate the
 * list of ranges. */
typedef struct ecs_table_range_t {
    ecs_matched_table_t *table;     /**< Reference to the matched table */
    int32_t start_row;              /**< Start of range  */
    int32_t count;                  /**< Number of entities in range */
} ecs_table_range_t;

#define EcsQueryNeedsTables (1)      /* Query needs matching with tables */ 
#define EcsQueryMonitor (2)          /* Query needs to be registered as a monitor */
#define EcsQueryOnSet (4)            /* Query needs to be registered as on_set system */
#define EcsQueryUnSet (8)            /* Query needs to be registered as un_set system */
#define EcsQueryMatchDisabled (16)   /* Does query match disabled */
#define EcsQueryMatchPrefab (32)     /* Does query match prefabs */
#define EcsQueryHasRefs (64)         /* Does query have references */
#define EcsQueryHasTraits (128)      /* Does query have traits */

#define EcsQueryNoActivation (EcsQueryMonitor | EcsQueryOnSet | EcsQueryUnSet)

/** Query that is automatically matched against active tables */
struct ecs_query_t {
    /* Signature of query */
    ecs_sig_t sig;

    /* Reference to world */
    ecs_world_t *world;

    /* Tables matched with query */
    ecs_vector_t *tables;
    ecs_vector_t *empty_tables;

    /* Handle to system (optional) */
    ecs_entity_t system;   

    /* Used for sorting */
    ecs_entity_t sort_on_component;
    ecs_compare_action_t compare;   
    ecs_vector_t *table_ranges;     

    /* Used for table sorting */
    ecs_entity_t rank_on_component;
    ecs_rank_type_action_t group_table;

    /* The query kind determines how it is registered with tables */
    int8_t flags;

    int32_t cascade_by;         /* Identify CASCADE column */
    int32_t match_count;        /* How often have tables been (un)matched */
    int32_t prev_match_count;   /* Used to track if sorting is needed */
};

/** Keep track of how many [in] columns are active for [out] columns of OnDemand
 * systems. */
typedef struct ecs_on_demand_out_t {
    ecs_entity_t system;    /* Handle to system */
    int32_t count;         /* Total number of times [out] columns are used */
} ecs_on_demand_out_t;

/** Keep track of which OnDemand systems are matched with which [in] columns */
typedef struct ecs_on_demand_in_t {
    int32_t count;         /* Number of active systems with [in] column */
    ecs_vector_t *systems;  /* Systems that have this column as [out] column */
} ecs_on_demand_in_t;

/** A column system is a system that is ran periodically (default = every frame)
 * on all entities that match the system signature expression. Column systems
 * are prematched with tables (archetypes) that match the system signature
 * expression. Each time a column system is invoked, it iterates over the 
 * matched list of tables (the 'tables' member). 
 *
 * For each table, the system stores a list of the components that were matched
 * with the system. This list may differ from the component list of the table,
 * when OR expressions or optional expressions are used.
 * 
 * A column system keeps track of tables that are empty. These tables are stored
 * in the 'empty_tables' array. This prevents the system from iterating over
 * tables in the main loop that have no data.
 * 
 * For each table, a column system stores an index that translates between the
 * a column in the system signature, and the matched table. This information is
 * stored, alongside with an index that identifies the table, in the 'tables'
 * member. This is an array of an array of integers, per table.
 * 
 * Additionally, the 'tables' member contains information on where a component
 * should be fetched from. By default, components are fetched from an entity,
 * but a system may specify that a component must be resolved from a container,
 * or must be fetched from a prefab. In this case, the index to lookup a table
 * column from system column contains a negative number, which identifies an
 * element in the 'refs' array.
 * 
 * The 'refs' array contains elements of type 'EcsRef', and stores references
 * to external entities. References can vary per table, but not per entity/row,
 * as prefabs / containers are part of the entity type, which in turn 
 * identifies the table in which the entity is stored.
 * 
 * The 'period' and 'time_passed' members are used for periodic systems. An
 * application may specify that a system should only run at a specific interval, 
 * like once per second. This interval is stored in the 'period' member. Each
 * time the system is evaluated but not ran, the delta_time is added to the 
 * time_passed member, until it exceeds 'period'. In that case, the system is
 * ran, and 'time_passed' is decreased by 'period'. 
 */
typedef struct EcsSystem {
    ecs_iter_action_t action;       /* Callback to be invoked for matching it */
    void *ctx;                      /* Userdata for system */

    ecs_entity_t entity;                  /* Entity id of system, used for ordering */
    ecs_query_t *query;                   /* System query */
    ecs_on_demand_out_t *on_demand;       /* Keep track of [out] column refs */
    ecs_system_status_action_t status_action; /* Status action */
    void *status_ctx;                     /* User data for status action */    
    ecs_entity_t tick_source;             /* Tick source associated with system */
    
    int32_t invoke_count;                 /* Number of times system is invoked */
    float time_spent;                     /* Time spent on running system */
    float time_passed;                    /* Time passed since last invocation */
    bool has_out_columns;                 /* True if system has out columns */
} EcsSystem;

#define ECS_TYPE_DB_MAX_CHILD_NODES (256)
#define ECS_TYPE_DB_BUCKET_COUNT (256)

/** The ecs_type_node_t type is a node in a hierarchical structure that allows
 * for quick lookups of types. A node represents a type, and its direct children
 * represent types with one additional entity. For example, given a node [A],
 * [A, B] would be a child node.
 * 
 * Child nodes are looked up directly using the entity id. For example, node [A]
 * will be stored at root.nodes[A]. Children entity ids are offset by their 
 * parent, such that [A, B] is stored at root.nodes[A].nodes[B - A].
 * 
 * If the offset exceeds ECS_TYPE_DB_MAX_CHILD_NODES, the type will be stored in
 * the types map. This map is keyed by the hash of the type relative to its
 * parent. For example, the hash for type [A, B, C] will be computed on [B, C]
 * if its parent is [A]. */
typedef struct ecs_type_link_t {
    ecs_type_t type;                /* type of current node */
    struct ecs_type_link_t *next;   /* next link (for iterating linearly) */
} ecs_type_link_t;

typedef struct ecs_type_node_t {
    ecs_vector_t *nodes;    /* child nodes - <ecs_entity_t, ecs_type_node_t> */
    ecs_vector_t **types;   /* child types w/large entity offsets - <hash, vector<ecs_type_link_t>> */
    ecs_type_link_t link;     
} ecs_type_node_t;

struct ecs_ei_t {
    ecs_sparse_t *lo;       /* Low entity ids are stored in a sparse set */
    ecs_map_t *hi;          /* To save memory high ids are stored in a map */
};

typedef enum ecs_op_kind_t {
    EcsOpNone,
    EcsOpAdd,
    EcsOpRemove,   
    EcsOpSet,
} ecs_op_kind_t;

typedef struct ecs_op_t {
    ecs_op_kind_t kind;
    ecs_entity_t entity;
    ecs_entities_t components;
    ecs_entity_t component;
    void *value;
    size_t size;
} ecs_op_t;

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows flecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
struct ecs_stage_t {
    /* This points to the world pointer associated with the stage. Even though
     * stages belong to the same world, when multithreaded, an application will
     * receive a pointer not to the world, but to a thread. This allows for
     * transparently passing the thread context without having to fallback on
     * more expensive methods such as thread local storage. This world pointer
     * is stored in the stage, so that it can be easily passed around when for
     * example invoking callbacks, and prevents the API from passing around two
     * world pointers (or constantly obtaining the real world when needed). */
    ecs_world_t *world;

    /* If this is not main stage, 
     * changes to the entity index 
     * are buffered here */
    ecs_ei_t entity_index; /* Entity lookup table for (table, row) */

    /* If this is not a thread
     * stage, these are the same
     * as the main stage */
    ecs_sparse_t *tables;          /* Tables created while >1 threads running */
    ecs_table_t root;              /* Root table */
    ecs_vector_t *dirty_tables;    /* Tables that need merging */

    /* Namespacing */
    ecs_table_t *scope_table;      /* Table for current scope */
    ecs_entity_t scope;            /* Entity of current scope */

    int32_t id;                    /* Unique id that identifies the stage */

    /* Are operations deferred? */
    int32_t defer;
    ecs_vector_t *defer_queue;

    /* One-shot actions to be executed after the merge */
    ecs_vector_t *post_frame_actions;

    /* Is entity range checking enabled? */
    bool range_check_enabled;
};

/** Supporting type to store looked up or derived entity data */
typedef struct ecs_entity_info_t {
    ecs_record_t *record;       /* Main stage record in entity index */
    ecs_table_t *table;         /* Table. Not set if entity is empty */
    ecs_data_t *data;           /* Stage-specific table columns */
    int32_t row;                /* Actual row (stripped from is_watched bit) */
    bool is_watched;            /* Is entity being watched */
} ecs_entity_info_t;

/** A type desribing a worker thread. When a system is invoked by a worker
 * thread, it receives a pointer to an ecs_thread_t instead of a pointer to an 
 * ecs_world_t (provided by the ecs_iter_t type). When this ecs_thread_t is passed down
 * into the flecs API, the API functions are able to tell whether this is an
 * ecs_thread_t or an ecs_world_t by looking at the 'magic' number. This allows the
 * API to transparently resolve the stage to which updates should be written,
 * without requiring different API calls when working in multi threaded mode. */
typedef struct ecs_thread_t {
    int32_t magic;                           /* Magic number to verify thread pointer */
    ecs_world_t *world;                       /* Reference to world */
    ecs_stage_t *stage;                       /* Stage for thread */
    ecs_os_thread_t thread;                   /* Thread handle */
    uint16_t index;                           /* Index of thread */
} ecs_thread_t;

/** Component-specific data */
typedef struct ecs_c_info_t {
    ecs_vector_t *on_add;       /* Systems ran after adding this component */
    ecs_vector_t *on_remove;    /* Systems ran after removing this component */

    EcsComponentLifecycle lifecycle; /* Component lifecycle callbacks */
} ecs_c_info_t;

/* Component monitors */
typedef struct ecs_component_monitor_t {
    bool dirty_flags[ECS_HI_COMPONENT_ID];
    ecs_vector_t *monitors[ECS_HI_COMPONENT_ID];
    bool rematch;
} ecs_component_monitor_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    int32_t magic;               /* Magic number to verify world pointer */
    void *context;               /* Application context */
    ecs_vector_t *fini_actions;  /* Callbacks to execute when world exits */

    ecs_c_info_t c_info[ECS_HI_COMPONENT_ID]; /* Component callbacks & triggers */
    ecs_map_t *t_info;                        /* Tag triggers */


    /* --  Queries -- */

    /* Persistent queries registered with the world. Persistent queries are
     * stateful and automatically matched with existing and new tables. */
    ecs_sparse_t *queries;

    /* Keep track of components that were added/removed to/from monitored
     * entities. Monitored entities are entities that a query has matched with
     * specifically, as is the case with PARENT / CASCADE columns, FromEntity
     * columns and columns matched from prefabs. 
     * When these entities change type, queries may have to be rematched.
     * Queries register themselves as component monitors for specific components
     * and when these components change they are rematched. The component 
     * monitors are evaluated during a merge. */
    ecs_component_monitor_t component_monitors;

    /* Parent monitors are like normal component monitors except that the
     * conditions under which a parent component is flagged dirty is different.
     * Parent component flags are marked dirty when an entity that is a parent
     * adds or removes a CHILDOF flag. In that case, every component of that
     * parent will be marked dirty. This allows column modifiers like CASCADE
     * to correctly determine when the depth ranking of a table has changed. */
    ecs_component_monitor_t parent_monitors; 


    /* -- Systems -- */
    
    ecs_entity_t pipeline;             /* Current pipeline */
    ecs_map_t *on_activate_components; /* Trigger on activate of [in] column */
    ecs_map_t *on_enable_components;   /* Trigger on enable of [in] column */
    ecs_vector_t *fini_tasks;          /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    ecs_map_t *type_handles;          /* Handles to named types */


    /* -- Staging -- */

    ecs_stage_t stage;               /* Main storage */
    ecs_stage_t temp_stage;          /* Stage for when processing systems */
    ecs_vector_t *worker_stages;     /* Stages for worker threads */
    int32_t stage_count;            /* Number of stages in world */


    /* -- Hierarchy administration -- */

    ecs_map_t *child_tables;        /* Child tables per parent entity */
    const char *name_prefix;        /* Remove prefix from C names in modules */


    /* -- Multithreading -- */

    ecs_vector_t *workers;           /* Worker threads */
    
    ecs_os_cond_t worker_cond;       /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;         /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;       /* Mutex for job_cond */
    int32_t workers_running;         /* Number of threads running */
    int32_t workers_waiting;         /* Number of workers waiting on sync */


    /* -- Time management -- */

    ecs_time_t world_start_time;  /* Timestamp of simulation start */
    ecs_time_t frame_start_time;  /* Timestamp of frame start */
    float fps_sleep;              /* Sleep time to prevent fps overshoot */


    /* -- Metrics -- */

    ecs_world_info_t stats;


    /* -- Settings from command line arguments -- */

    int arg_fps;
    int arg_threads;


    /* -- World lock -- */

    ecs_os_mutex_t mutex;         /* Locks the world if locking enabled */
    ecs_os_mutex_t thr_sync;      /* Used to signal threads at end of frame */
    ecs_os_cond_t thr_cond;       /* Used to signal threads at end of frame */


    /* -- World state -- */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
    bool in_progress;             /* Is world being progressed */
    bool is_merging;              /* Is world currently being merged */
    bool auto_merge;              /* Are stages auto-merged by ecs_progress */
    bool measure_frame_time;      /* Time spent on each frame */
    bool measure_system_time;     /* Time spent by each system */
    bool should_quit;             /* Did a system signal that app should quit */
    bool locking_enabled;         /* Lock world when in progress */    
};

#endif
