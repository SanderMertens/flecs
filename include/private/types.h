#ifndef REFLECS_TYPES_PRIVATE_H
#define REFLECS_TYPES_PRIVATE_H

#include <stdlib.h>
#include <pthread.h>

#include "../reflecs.h"
#include "../util/array.h"
#include "../util/map.h"

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

#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_THREAD_MAGIC (0x65637374)


/* -- Builtin component types -- */

/** Metadata of an explicitly created family (identified by an entity id) */
typedef struct EcsTypeComponent {
    EcsType family;    /* Preserved nested families */
    EcsType resolved;  /* Resolved nested families */
} EcsTypeComponent;

/** Metadata of a component */
typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

/** Type that is used by systems to indicate where to fetch a component from */
typedef enum EcsSystemExprElemKind {
    EcsFromEntity,          /* Get component from entity (default) */
    EcsFromContainer,       /* Get component from container */
    EcsFromSystem,          /* Get component from system */
    EcsFromId               /* Get entity by id */
} EcsSystemExprElemKind;

/** Type describing an operator used in an signature of a system signature */
typedef enum EcsSystemExprOperKind {
    EcsOperAnd = 0,
    EcsOperOr = 1,
    EcsOperNot = 2,
    EcsOperOptional = 3,
    EcsOperLast = 4
} EcsSystemExprOperKind;

/** Callback used by the system signature expression parser */
typedef EcsResult (*ecs_parse_action)(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component,
    void *ctx);

/** Type that describes a single column in the system signature */
typedef struct EcsSystemColumn {
    EcsSystemExprElemKind kind;       /* Element kind (Entity, Component) */
    EcsSystemExprOperKind oper_kind;  /* Operator kind (AND, OR, NOT) */
    union {
        EcsType family;             /* Used for OR operator */
        EcsEntity component;          /* Used for AND operator */
    } is;
} EcsSystemColumn;

/** Type that stores a reference to components of external entities (prefabs) */
typedef struct EcsSystemRef {
    EcsEntity entity;
    EcsEntity component;
} EcsSystemRef;

/** Base type for a system */
typedef struct EcsSystem {
    EcsSystemAction action;    /* Callback to be invoked for matching rows */
    const char *signature;     /* Signature with which system was created */
    EcsArray *columns;         /* Column components */
    EcsType not_from_entity; /* Exclude components from entity */
    EcsType not_from_component; /* Exclude components from components */
    EcsType and_from_entity; /* Which components are required from entity */
    EcsSystemKind kind;        /* Kind of system */
    float time_spent;          /* Time spent on running system */
    bool enabled;              /* Is system enabled or not */
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
 * as prefabs / containers are part of the entity family, which in turn 
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
    EcsEntity entity;          /* Entity id of system, used for ordering */
    EcsArray *components;      /* Computed component list per matched table */
    EcsArray *inactive_tables; /* Inactive tables */
    EcsArray *jobs;            /* Jobs for this system */
    EcsArray *tables;          /* Table index + refs index + column offsets */
    EcsArray *refs;            /* Columns that point to other entities */
    EcsArrayParams table_params; /* Parameters for tables array */
    EcsArrayParams component_params; /* Parameters for components array */
    EcsArrayParams ref_params; /* Parameters for tables array */
    EcsType and_from_system; /* Used to auto-add components to system */
    float period;              /* Minimum period inbetween system invocations */
    float time_passed;         /* Time passed since last invocation */
} EcsColSystem;

/** A row system is a system that is ran on 1..n entities for which a certain 
 * operation has been invoked. The system kind determines on what kind of
 * operation the row system is invoked. Example operations are ecs_add,
 * ecs_remove and ecs_set. */
typedef struct EcsRowSystem {
    EcsSystem base;
    EcsArray *components;       /* Components in order of signature */
} EcsRowSystem;


/* -- Private types -- */

/** A table column describes a single column in a table (archetype) */
typedef struct EcsTableColumn {
    EcsArray *data;               /* Column data */
    uint16_t size;                /* Column size (saves component lookups) */
} EcsTableColumn;

/** A table is the Reflecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing column systems */
typedef struct EcsTable {
    EcsArray *family;             /* Reference to family_index entry */
    EcsTableColumn *columns;      /* Columns storing components of array */
    EcsArray *frame_systems;      /* Frame systems matched with table */
    EcsType type_id;          /* Identifies table family in family_index */
 } EcsTable;
 
/** The EcsRow struct is a 64-bit value that describes in which table
 * (identified by a type_id) is stored, at which index. Entries in the 
 * world::entity_index are of type EcsRow. */
typedef struct EcsRow {
    EcsType type_id;          /* Identifies a family (and table) in world */
    uint32_t index;               /* Index of the entity in its table */
} EcsRow;

/** Supporting type that internal functions pass around to ensure that data
 * related to an entity is only looked up once. */
typedef struct EcsEntityInfo {
    EcsEntity entity;
    EcsType type_id;
    uint32_t index;
    EcsTable *table;
    EcsTableColumn *columns;
} EcsEntityInfo;

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows reflecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
typedef struct EcsStage {
    EcsMap *add_stage;            /* Entities with components to add */
    EcsMap *remove_stage;         /* Entities with components to remove */
    EcsMap *remove_merge;         /* All removed components before merge */
    EcsArray *delete_stage;       /* Deleted entities while in progress */
    EcsMap *entity_stage;         /* Entities committed while in progress */
    EcsMap *data_stage;           /* Arrays with staged component values */
    EcsMap *family_stage;         /* Families created while >1 threads running*/
    EcsArray *table_db_stage;     /* Tables created while >1 threads running */
    EcsMap *table_stage;          /* Index for table stage */
} EcsStage;

/** A type describing a unit of work to be executed by a worker thread. */ 
typedef struct EcsJob {
    EcsEntity system;             /* System handle */
    EcsColSystem *system_data;  /* System to run */
    uint32_t table_index;         /* Current SystemTable */
    uint32_t start_index;         /* Start index in row chunk */
    uint32_t row_count;           /* Total number of rows to process */
} EcsJob;

/** A type desribing a worker thread. When a system is invoked by a worker
 * thread, it receives a pointer to an EcsThread instead of a pointer to an 
 * EcsWorld (provided by the EcsRows type). When this EcsThread is passed down
 * into the reflecs API, the API functions are able to tell whether this is an
 * EcsThread or an EcsWorld by looking at the 'magic' number. This allows the
 * API to transparently resolve the stage to which updates should be written,
 * without requiring different API calls when working in multi threaded mode. */
typedef struct EcsThread {
    uint32_t magic;               /* Magic number to verify thread pointer */
    uint32_t job_count;           /* Number of jobs scheduled for thread */
    EcsWorld *world;              /* Reference to world */
    EcsJob *jobs[ECS_MAX_JOBS_PER_WORKER]; /* Array with jobs */
    EcsStage *stage;              /* Stage for thread */
    pthread_t thread;             /* Thread handle */
} EcsThread;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct EcsWorld {
    uint32_t magic;               /* Magic number to verify world pointer */
    float delta_time;           /* Time passed to or computed by ecs_progress */
    void *context;                /* Application context */

    EcsArray *table_db;           /* All tables (archetypes) in the world */


    /* -- Column systems -- */

    EcsArray *on_load_systems;    /* Systems executed at start of frame */
    EcsArray *pre_frame_systems;  /* Systems executed before frame systems */
    EcsArray *on_frame_systems;   /* Frame systems */
    EcsArray *post_frame_systems; /* Systems executed after frame systems */
    EcsArray *on_store_systems;   /* Systems executed at end of frame */
    EcsArray *on_demand_systems;  /* On demand systems */
    EcsArray *inactive_systems;   /* Frame systems with empty tables */


    /* -- Row systems -- */

    EcsArray *add_systems;        /* Systems invoked on ecs_stage_add */
    EcsArray *remove_systems;     /* Systems invoked on ecs_stage_remove */
    EcsArray *set_systems;        /* Systems invoked on ecs_set */


    /* -- Tasks -- */

    EcsArray *tasks;              /* Periodic actions not invoked on entities */
    EcsArray *fini_tasks;         /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    EcsMap *entity_index;         /* Maps entity handle to EcsRow  */
    EcsMap *table_index;          /* Identifies a table by type_id */
    EcsMap *family_index;         /* References to component families */
    EcsMap *family_handles;       /* Index to families created by API */
    EcsMap *prefab_index;         /* Index to find prefabs in families */
    EcsMap *family_sys_add_index; /* Index to find add row systems for family */
    EcsMap *family_sys_remove_index; /* Index to find remove row systems for family */
    EcsMap *family_sys_set_index; /* Index to find set row systems for family */


    /* -- Staging -- */

    EcsStage stage;               /* Stage of main thread */
    EcsArray *stage_db;           /* Stage storage (one for each worker) */


    /* -- Multithreading -- */

    EcsArray *worker_threads;     /* Worker threads */
    pthread_cond_t thread_cond;   /* Signal that worker threads can start */
    pthread_mutex_t thread_mutex; /* Mutex for thread condition */
    pthread_cond_t job_cond;      /* Signal that worker thread job is done */
    pthread_mutex_t job_mutex;    /* Mutex for protecting job counter */
    uint32_t jobs_finished;       /* Number of jobs finished */
    uint32_t threads_running;     /* Number of threads running */

    EcsEntity last_handle;        /* Last issued handle */


    /* -- Handles to builtin components families -- */
    EcsType t_component;
    EcsType t_type;
    EcsType t_prefab;
    EcsType t_row_system;
    EcsType t_col_system;


    /* -- Time management -- */

    uint32_t tick;                /* Number of computed frames by world */
    struct timespec frame_start;  /* Starting timestamp of frame */
    float frame_time;             /* Time spent processing a frame */
    float system_time;            /* Time spent processing systems */
    float target_fps;             /* Target fps */
    float fps_sleep;              /* Sleep time to prevent fps overshoot */


    /* -- World state -- */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
    bool in_progress;             /* Is world being progressed */
    bool is_merging;              /* Is world currently being merged */
    bool auto_merge;              /* Are stages auto-merged by ecs_progress */
    bool measure_frame_time;      /* Time spent on each frame */
    bool measure_system_time;     /* Time spent by each system */
    bool should_quit;             /* Did a system signal that app should quit */
};


/* Parameters for various array types */
extern const EcsArrayParams handle_arr_params;
extern const EcsArrayParams stage_arr_params;
extern const EcsArrayParams table_arr_params;
extern const EcsArrayParams thread_arr_params;
extern const EcsArrayParams job_arr_params;
extern const EcsArrayParams column_arr_params;


#endif
