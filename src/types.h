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

#include <flecs.h>
#include <flecs/util/dbg.h>

#define ECS_WORLD_INITIAL_TABLE_COUNT (2)
#define ECS_WORLD_INITIAL_ENTITY_COUNT (2)
#define ECS_WORLD_INITIAL_STAGING_COUNT (0)
#define ECS_WORLD_INITIAL_COL_SYSTEM_COUNT (1)
#define ECS_WORLD_INITIAL_OTHER_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_ADD_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_REMOVE_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_SET_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_PREFAB_COUNT (0)
#define ECS_MAP_INITIAL_NODE_COUNT (4)
#define ECS_TABLE_INITIAL_ROW_COUNT (0)
#define ECS_SYSTEM_INITIAL_TABLE_COUNT (0)
#define ECS_MAX_JOBS_PER_WORKER (16)

/* This is _not_ the max number of entities that can be of a given type. This 
 * constant defines the maximum number of components, prefabs and parents can be
 * in one type. This limit serves two purposes: detect errors earlier (assert on
 * very large types) and allow for more efficient allocation strategies (like
 * using alloca for temporary buffers). */
#define ECS_MAX_ENTITIES_IN_TYPE (256)

#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_THREAD_MAGIC (0x65637374)


/* -- Builtin component types -- */

/* For prefabs with child entities, the parent prefab must be marked so that
 * flecs knows not to share components from it, as adding a prefab as a parent
 * is stored in the same way as adding a prefab for sharing components.
 * There are two mechanisms required to accomplish this. The first one is to set
 * the 'parent' member in the EcsPrefab component, for the child entity of the
 * prefab. This acts as a front-end for another mechanism, that ensures that
 * child entities for different prefab parents are added to different tables. As
 * a result of setting a parent in EcsPrefab, Flecs will:
 * 
 *  - Add the prefab to the entity type
 *  - Find or create a prefab parent flag entity
 *  - Set the EcsPrefabParent component on the prefab parent flag entity
 *  - Add the prefab parent flag entity to the child
 * 
 * The last step ensures that the type of the child entity is associated with at
 * most one prefab parent. If the mechanism would just rely on the EcsPrefab
 * parent field, it would theoretically be possible that childs for different
 * prefab parents end up in the same table.
 */
typedef struct EcsPrefabParent {
    ecs_entity_t parent;
} EcsPrefabParent;

typedef struct ecs_builder_op_t {
    const char *id;
    ecs_type_t type;
} ecs_builder_op_t;

typedef struct EcsPrefabBuilder {
    ecs_vector_t *ops; /* ecs_builder_op_t */
} EcsPrefabBuilder;

typedef enum ecs_system_expr_inout_kind_t {
    EcsInOut,
    EcsIn,
    EcsOut
} ecs_system_expr_inout_kind_t;

/** Type that is used by systems to indicate where to fetch a component from */
typedef enum ecs_system_expr_elem_kind_t {
    EcsFromSelf,            /* Get component from self (default) */
    EcsFromOwned,           /* Get owned component from self */
    EcsFromShared,          /* Get shared component from self */
    EcsFromContainer,       /* Get component from container */
    EcsFromSystem,          /* Get component from system */
    EcsFromEmpty,           /* Get entity handle by id */
    EcsFromEntity,          /* Get component from other entity */
    EcsCascade              /* Walk component in cascading (hierarchy) order */
} ecs_system_expr_elem_kind_t;

/** Type describing an operator used in an signature of a system signature */
typedef enum ecs_system_expr_oper_kind_t {
    EcsOperAnd = 0,
    EcsOperOr = 1,
    EcsOperNot = 2,
    EcsOperOptional = 3,
    EcsOperLast = 4
} ecs_system_expr_oper_kind_t;

/** Callback used by the system signature expression parser */
typedef int (*ecs_parse_action_t)(
    ecs_world_t *world,
    const char *id,
    const char *expr,
    int column,
    ecs_system_expr_elem_kind_t elem_kind,
    ecs_system_expr_oper_kind_t oper_kind,
    ecs_system_expr_inout_kind_t inout_kind,
    const char *component,
    const char *source,
    void *ctx);

/** Type that describes a single column in the system signature */
typedef struct ecs_system_column_t {
    ecs_system_expr_elem_kind_t kind;       /* Element kind (Entity, Component) */
    ecs_system_expr_oper_kind_t oper_kind;  /* Operator kind (AND, OR, NOT) */
    ecs_system_expr_inout_kind_t inout_kind;     /* Is component read or written */
    union {
        ecs_type_t type;             /* Used for OR operator */
        ecs_entity_t component;      /* Used for AND operator */
    } is;
    ecs_entity_t source;             /* Source entity (used with FromEntity) */
} ecs_system_column_t;

/** A table column describes a single column in a table (archetype) */
struct ecs_table_column_t {
    ecs_vector_t *data;              /* Column data */
    uint16_t size;                   /* Column size (saves component lookups) */
};

#define EcsTableIsStaged  (1)
#define EcsTableIsPrefab (2)
#define EcsTableHasPrefab (4)
#define EcsTableHasBuiltins (8)

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing column systems */
struct ecs_table_t {
    ecs_table_column_t *columns;      /* Columns storing components of array */
    ecs_vector_t *frame_systems;      /* Frame systems matched with table */
    ecs_type_t type;                  /* Identifies table type in type_index */
    uint32_t flags;                   /* Flags for testing table properties */
};

/** Cached reference to a component in an entity */
struct ecs_reference_t {
    ecs_entity_t entity;
    ecs_entity_t component;
    void *cached_ptr;
};

/** Type containing data for a table matched with a system */
typedef struct ecs_matched_table_t {
    ecs_table_t *table;             /* Reference to the table */
    int32_t *columns;               /* Mapping of system columns to table */
    ecs_entity_t *components;       /* Actual components of system columns */
    ecs_vector_t *references;       /* Reference columns and cached pointers */
    int32_t depth;                  /* Depth of table (when using CASCADE) */
} ecs_matched_table_t;

/** Keep track of how many [in] columns are active for [out] columns of OnDemand
 * systems. */
typedef struct ecs_on_demand_out_t {
    ecs_entity_t system;    /* Handle to system */
    uint32_t count;         /* Total number of times [out] columns are used */
} ecs_on_demand_out_t;

/** Keep track of which OnDemand systems are matched with which [in] columns */
typedef struct ecs_on_demand_in_t {
    int32_t count;         /* Number of active systems with [in] column */
    ecs_vector_t *systems;  /* Systems that have this column as [out] column */
} ecs_on_demand_in_t;

/** Base type for a system */
typedef struct EcsSystem {
    ecs_system_action_t action;    /* Callback to be invoked for matching rows */
    char *signature;         /* Signature with which system was created */
    ecs_vector_t *columns;         /* Column components */
    void *ctx;                     /* User data */

    /* Precomputed types for quick comparisons */
    ecs_type_t not_from_self;      /* Exclude components from self */
    ecs_type_t not_from_owned;     /* Exclude components from self only if owned */
    ecs_type_t not_from_shared;     /* Exclude components from self only if shared */
    ecs_type_t not_from_component; /* Exclude components from components */
    ecs_type_t and_from_self;      /* Which components are required from entity */
    ecs_type_t and_from_owned;      /* Which components are required from entity */
    ecs_type_t and_from_shared;      /* Which components are required from entity */
    ecs_type_t and_from_system;    /* Used to auto-add components to system */
    
    EcsSystemKind kind;            /* Kind of system */
    int32_t cascade_by;            /* CASCADE column index */
    int64_t invoke_count;          /* Number of times system was invoked */
    double time_spent;             /* Time spent on running system */
    bool enabled;                  /* Is system enabled or not */
    bool has_refs;                 /* Does the system have reference columns */
    bool needs_tables;             /* Does the system need table matching */
    bool match_prefab;             /* Should this system match prefabs */
    bool match_disabled;           /* Should this system match disabled entities */
} EcsSystem;

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
 * in the 'inactive_tables' array. This prevents the system from iterating over
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
typedef struct EcsColSystem {
    EcsSystem base;
    ecs_entity_t entity;                  /* Entity id of system, used for ordering */
    ecs_vector_t *jobs;                   /* Jobs for this system */
    ecs_vector_t *tables;                 /* Vector with matched tables */
    ecs_vector_t *inactive_tables;        /* Inactive tables */
    ecs_on_demand_out_t *on_demand;       /* Keep track of [out] column refs */
    ecs_system_status_action_t status_action; /* Status action */
    void *status_ctx;                     /* User data for status action */
    ecs_vector_params_t column_params;    /* Parameters for table_columns */
    ecs_vector_params_t component_params; /* Parameters for components */
    ecs_vector_params_t ref_params;       /* Parameters for refs */
    float period;                         /* Minimum period inbetween system invocations */
    float time_passed;                    /* Time passed since last invocation */
} EcsColSystem;

/** A row system is a system that is ran on 1..n entities for which a certain 
 * operation has been invoked. The system kind determines on what kind of
 * operation the row system is invoked. Example operations are ecs_add,
 * ecs_remove and ecs_set. */
typedef struct EcsRowSystem {
    EcsSystem base;
    ecs_vector_t *components;       /* Components in order of signature */
} EcsRowSystem;
 
/** The ecs_row_t struct is a 64-bit value that describes in which table
 * (identified by a type) is stored, at which index. Entries in the 
 * world::entity_index are of type ecs_row_t. */
typedef struct ecs_row_t {
    ecs_type_t type;              /* Identifies a type (and table) in world */
    int32_t index;                /* Index of the entity in its table */
} ecs_row_t;

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

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows flecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
typedef struct ecs_stage_t {
    /* If this is not main stage, 
     * changes to the entity index 
     * are buffered here */
    ecs_map_t *entity_index;       /* Entity lookup table for (table, row) */

    /* If this is not a thread
     * stage, these are the same
     * as the main stage */
    ecs_type_node_t type_root;     /* Hierarchical type store (& first link) */
    ecs_type_link_t *last_link;    /* Link to last registered type */
    ecs_chunked_t *tables;         /* Tables created while >1 threads running */
    ecs_map_t *table_index;        /* Lookup table by type */

    /* These occur only in
     * temporary stages, and
     * not on the main stage */
    ecs_map_t *data_stage;         /* Arrays with staged component values */
    ecs_map_t *remove_merge;       /* All removed components before merge */

    /* Keep track of changes so
     * code knows when entity
     * info is invalidated */
    uint32_t commit_count;
    ecs_type_t from_type;
    ecs_type_t to_type;
    
    /* Is entity range checking enabled? */
    bool range_check_enabled;
} ecs_stage_t;

/** Supporting type that internal functions pass around to ensure that data
 * related to an entity is only looked up once. */
typedef struct ecs_entity_info_t {
    ecs_entity_t entity;
    ecs_type_t type;
    int32_t index;
    ecs_table_t *table;
    ecs_table_column_t *columns;
    bool is_watched;

    /* Used for determining if ecs_entity_info_t should be invalidated */
    ecs_stage_t *stage;
    uint32_t commit_count;
} ecs_entity_info_t;

/** A type describing a unit of work to be executed by a worker thread. */ 
typedef struct ecs_job_t {
    ecs_entity_t system;          /* System handle */
    EcsColSystem *system_data;    /* System to run */
    uint32_t offset;              /* Start index in row chunk */
    uint32_t limit;               /* Total number of rows to process */
} ecs_job_t;

/** A type desribing a worker thread. When a system is invoked by a worker
 * thread, it receives a pointer to an ecs_thread_t instead of a pointer to an 
 * ecs_world_t (provided by the ecs_rows_t type). When this ecs_thread_t is passed down
 * into the flecs API, the API functions are able to tell whether this is an
 * ecs_thread_t or an ecs_world_t by looking at the 'magic' number. This allows the
 * API to transparently resolve the stage to which updates should be written,
 * without requiring different API calls when working in multi threaded mode. */
typedef struct ecs_thread_t {
    uint32_t magic;                           /* Magic number to verify thread pointer */
    uint32_t job_count;                       /* Number of jobs scheduled for thread */
    ecs_world_t *world;                       /* Reference to world */
    ecs_job_t *jobs[ECS_MAX_JOBS_PER_WORKER]; /* Array with jobs */
    ecs_stage_t *stage;                       /* Stage for thread */
    ecs_os_thread_t thread;                   /* Thread handle */
    uint16_t index;                           /* Index of thread */
} ecs_thread_t;

/* World snapshot */
struct ecs_snapshot_t {
    ecs_map_t *entity_index;
    ecs_chunked_t *tables;
    ecs_entity_t last_handle;
    ecs_filter_t filter;
};

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    uint32_t magic;               /* Magic number to verify world pointer */
    float delta_time;             /* Time passed to or computed by ecs_progress */
    void *context;                /* Application context */

    /* -- Column systems -- */

    ecs_vector_t *on_load_systems;  
    ecs_vector_t *post_load_systems;  
    ecs_vector_t *pre_update_systems;  
    ecs_vector_t *on_update_systems;   
    ecs_vector_t *on_validate_systems; 
    ecs_vector_t *post_update_systems; 
    ecs_vector_t *pre_store_systems; 
    ecs_vector_t *on_store_systems;   
    ecs_vector_t *manual_systems;  
    ecs_vector_t *inactive_systems;

    /* -- OnDemand systems -- */
    
    ecs_map_t *on_activate_components; /* Trigger on activate of [in] column */
    ecs_map_t *on_enable_components; /* Trigger on enable of [in] column */


    /* -- Row systems -- */

    ecs_vector_t *add_systems;        /* Systems invoked on ecs_stage_add */
    ecs_vector_t *remove_systems;     /* Systems invoked on ecs_stage_remove */
    ecs_vector_t *set_systems;        /* Systems invoked on ecs_set */


    /* -- Tasks -- */

    ecs_vector_t *fini_tasks;         /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    ecs_map_t *type_sys_add_index;    /* Index to find add row systems for type */
    ecs_map_t *type_sys_remove_index; /* Index to find remove row systems for type*/
    ecs_map_t *type_sys_set_index;    /* Index to find set row systems for type */
    
    ecs_map_t *prefab_parent_index;   /* Index to find flag for prefab parent */
    ecs_map_t *type_handles;          /* Handles to named types */


    /* -- Staging -- */

    ecs_stage_t main_stage;          /* Main storage */
    ecs_stage_t temp_stage;          /* Stage for when processing systems */
    ecs_vector_t *worker_stages;     /* Stages for worker threads */


    /* -- Multithreading -- */

    ecs_vector_t *worker_threads;    /* Worker threads */
    ecs_os_cond_t thread_cond;       /* Signal that worker threads can start */
    ecs_os_mutex_t thread_mutex;     /* Mutex for thread condition */
    ecs_os_cond_t job_cond;          /* Signal that worker thread job is done */
    ecs_os_mutex_t job_mutex;        /* Mutex for protecting job counter */
    uint32_t jobs_finished;          /* Number of jobs finished */
    uint32_t threads_running;        /* Number of threads running */

    ecs_entity_t last_handle;        /* Last issued handle */
    ecs_entity_t min_handle;         /* First allowed handle */
    ecs_entity_t max_handle;         /* Last allowed handle */


    /* -- Handles to builtin components types -- */

    ecs_type_t t_component;
    ecs_type_t t_type;
    ecs_type_t t_prefab;
    ecs_type_t t_row_system;
    ecs_type_t t_col_system;
    ecs_type_t t_builtins;


    /* -- Time management -- */

    ecs_time_t world_start_time;  /* Timestamp of simulation start */
    ecs_time_t frame_start_time;  /* Timestamp of frame start */
    float target_fps;             /* Target fps */
    float fps_sleep;              /* Sleep time to prevent fps overshoot */


    /* -- Metrics -- */

    double frame_time_total;      /* Total time spent in processing a frame */
    double system_time_total;     /* Total time spent in periodic systems */
    double merge_time_total;      /* Total time spent in merges */
    double world_time_total;      /* Time elapsed since first frame */
    uint32_t frame_count_total;   /* Total number of frames */


    /* -- Settings from command line arguments -- */

    int arg_fps;
    int arg_threads;


    /* -- World state -- */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
    bool in_progress;             /* Is world being progressed */
    bool is_merging;              /* Is world currently being merged */
    bool auto_merge;              /* Are stages auto-merged by ecs_progress */
    bool measure_frame_time;      /* Time spent on each frame */
    bool measure_system_time;     /* Time spent by each system */
    bool should_quit;             /* Did a system signal that app should quit */
    bool should_match;            /* Should tablea be rematched */
    bool should_resolve;          /* If a table reallocd, resolve system refs */
}; 


/* Parameters for various array types */
extern const ecs_vector_params_t handle_arr_params;
extern const ecs_vector_params_t stage_arr_params;
extern const ecs_vector_params_t table_arr_params;
extern const ecs_vector_params_t thread_arr_params;
extern const ecs_vector_params_t job_arr_params;
extern const ecs_vector_params_t builder_params;
extern const ecs_vector_params_t system_column_params;
extern const ecs_vector_params_t matched_table_params;
extern const ecs_vector_params_t matched_column_params;
extern const ecs_vector_params_t reference_params;
extern const ecs_vector_params_t ptr_params;

#endif
