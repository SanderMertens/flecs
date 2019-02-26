#ifndef REFLECS_TYPES_PRIVATE_H
#define REFLECS_TYPES_PRIVATE_H

#include <stdlib.h>
#include <pthread.h>

#include <include/reflecs.h>
#include "../util/array.h"
#include "../util/map.h"

#define ECS_WORLD_INITIAL_TABLE_COUNT (2)
#define ECS_WORLD_INITIAL_ENTITY_COUNT (2)
#define ECS_WORLD_INITIAL_STAGING_COUNT (0)
#define ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT (1)
#define ECS_WORLD_INITIAL_OTHER_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_INIT_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_DEINIT_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_SET_SYSTEM_COUNT (0)
#define ECS_WORLD_INITIAL_PREFAB_COUNT (0)
#define ECS_MAP_INITIAL_NODE_COUNT (4)
#define ECS_TABLE_INITIAL_ROW_COUNT (0)
#define ECS_SYSTEM_INITIAL_TABLE_COUNT (0)
#define ECS_MAX_JOBS_PER_WORKER (16)

#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_THREAD_MAGIC (0x65637374)

/** A family identifies a set of components */
typedef uint32_t EcsFamily;

/* -- Builtin component types -- */

typedef struct EcsFamilyComponent {
    EcsFamily family;    /* Preserved nested families */
    EcsFamily resolved;  /* Resolved nested families */
} EcsFamilyComponent;

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef enum EcsSystemExprElemKind {
    EcsFromEntity,          /* Get component from entity (default) */
    EcsFromContainer,       /* Get component from container */
    EcsFromSystem,          /* Get component from system */
    EcsFromId               /* Get entity by id */
} EcsSystemExprElemKind;

typedef enum EcsSystemExprOperKind {
    EcsOperAnd = 0,
    EcsOperOr = 1,
    EcsOperNot = 2,
    EcsOperOptional = 3,
    EcsOperLast = 4
} EcsSystemExprOperKind;

typedef EcsResult (*ecs_parse_action)(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component,
    void *ctx);

typedef struct EcsSystemColumn {
    EcsSystemExprElemKind kind;       /* Element kind (Entity, Component) */
    EcsSystemExprOperKind oper_kind;  /* Operator kind (AND, OR, NOT) */
    union {
        EcsFamily family;             /* Used for OR operator */
        EcsEntity component;          /* Used for AND operator */
    } is;
} EcsSystemColumn;

typedef struct EcsSystemRef {
    EcsEntity entity;
    EcsEntity component;
} EcsSystemRef;

typedef struct EcsSystem {
    EcsSystemAction action;    /* Callback to be invoked for matching rows */
    const char *signature;     /* Signature with which system was created */
    EcsArray *columns;         /* Column components */
    EcsFamily not_from_entity; /* Exclude components from entity */
    EcsFamily not_from_component; /* Exclude components from components */
    EcsEntity ctx_handle;      /* User-defined context for system */
    EcsSystemKind kind;        /* Kind of system */
    float time_spent;          /* Time spent on running system */
    bool enabled;              /* Is system enabled or not */
} EcsSystem;

typedef struct EcsTableSystem {
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
    EcsFamily and_from_entity; /* Used to match init / deinit notifications */
    EcsFamily and_from_system; /* Used to auto-add components to system */
    float period;              /* Minimum period inbetween system invocations */
    float time_passed;         /* Time passed since last invocation */
} EcsTableSystem;

typedef struct EcsRowSystem {
    EcsSystem base;
    EcsArray *components;       /* Components in order of signature */
} EcsRowSystem;

/* -- Private types -- */

typedef struct EcsTableColumn {
    EcsArray *data;               /* Column data */
    uint16_t size;         /* Column size (saves component lookups) */
} EcsTableColumn;

typedef struct EcsTable {
    EcsArray *family;             /* Reference to family_index entry */
    EcsTableColumn *columns;      /* Columns storing components of array */
    EcsArray *frame_systems;      /* Frame systems matched with table */
    EcsFamily family_id;          /* Identifies table family in family_index */
} EcsTable;

typedef struct EcsRow {
    EcsFamily family_id;          /* Identifies a family (and table) in world */
    uint32_t index;               /* Index of the entity in its table */
} EcsRow;

typedef struct EcsEntityInfo {
    EcsEntity entity;
    EcsFamily family_id;
    uint32_t index;
    EcsTable *table;
    EcsTableColumn *columns;
} EcsEntityInfo;

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

typedef struct EcsJob {
    EcsEntity system;             /* System handle */
    EcsTableSystem *system_data;  /* System to run */
    uint32_t table_index;         /* Current SystemTable */
    uint32_t start_index;         /* Start index in row chunk */
    uint32_t row_count;           /* Total number of rows to process */
} EcsJob;

typedef struct EcsThread {
    uint32_t magic;               /* Magic number to verify thread pointer */
    uint32_t job_count;           /* Number of jobs scheduled for thread */
    EcsWorld *world;              /* Reference to world */
    EcsJob *jobs[ECS_MAX_JOBS_PER_WORKER]; /* Array with jobs */
    EcsStage *stage;              /* Stage for thread */
    pthread_t thread;             /* Thread handle */
} EcsThread;

struct EcsWorld {
    uint32_t magic;               /* Magic number to verify world pointer */

    float delta_time;           /* Time passed to or computed by ecs_progress */

    void *context;                /* Application context */

    EcsArray *table_db;           /* All tables in the world */

    EcsArray *on_load_systems;    /* Systems executed at start of frame */
    EcsArray *pre_frame_systems;  /* Systems executed before frame systems */
    EcsArray *on_frame_systems;   /* Frame systems */
    EcsArray *post_frame_systems; /* Systems executed after frame systems */
    EcsArray *on_store_systems;   /* Systems executed at end of frame */
    EcsArray *on_demand_systems;  /* On demand systems */
    EcsArray *inactive_systems;   /* Frame systems with empty tables */

    EcsMap *add_systems;          /* Systems invoked on ecs_stage_add */
    EcsMap *remove_systems;       /* Systems invoked on ecs_stage_remove */
    EcsMap *set_systems;          /* Systems invoked on ecs_set */
    EcsArray *tasks;              /* Periodic actions not invoked on entities */
    EcsArray *fini_tasks;         /* Tasks to execute on ecs_fini */

    EcsMap *entity_index;         /* Maps entity handle to EcsRow  */
    EcsMap *table_index;          /* Identifies a table by family_id */
    EcsMap *family_index;         /* References to component families */
    EcsMap *family_handles;       /* Index to explicitly created families */
    EcsMap *prefab_index;         /* Index for finding prefabs in families */

    EcsStage stage;              /* Stage of main thread */

    EcsArray *worker_threads;     /* Worker threads */
    EcsArray *stage_db;           /* Stage storage (one for each worker) */
    pthread_cond_t thread_cond;   /* Signal that worker threads can start */
    pthread_mutex_t thread_mutex; /* Mutex for thread condition */
    pthread_cond_t job_cond;      /* Signal that worker thread job is done */
    pthread_mutex_t job_mutex;    /* Mutex for protecting job counter */
    uint32_t jobs_finished;       /* Number of jobs finished */
    uint32_t threads_running;     /* Number of threads running */

    EcsEntity last_handle;        /* Last issued handle */
    EcsEntity deinit_table_system; /* Handle to internal deinit system */
    EcsEntity deinit_row_system;  /* Handle to internal deinit system */

    EcsFamily component_family;   /* EcsComponent, EcsId */
    EcsFamily table_system_family; /* EcsTableSystem, EcsId */
    EcsFamily row_system_family;  /* EcsRowSystem, EcsId */
    EcsFamily family_family;      /* EcsFamily, EcsId */
    EcsFamily prefab_family;      /* EcsPrefab, EcsId */

    uint32_t tick;                /* Number of computed frames by world */
    struct timespec frame_start;  /* Starting timestamp of frame */
    float frame_time;             /* Time spent processing a frame */
    float system_time;            /* Time spent processing systems */
    float target_fps;             /* Target fps */
    float fps_sleep;              /* Sleep time to prevent fps overshoot */

    /* The diff between frame_time and system_time is time spent on merging */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
    bool in_progress;             /* Is world being progressed */
    bool is_merging;              /* Is world currently being merged */
    bool auto_merge;              /* Are stages auto-merged by ecs_progress */
    bool measure_frame_time;      /* Time spent on each frame */
    bool measure_system_time;     /* Time spent by each system */
    bool should_quit;             /* Did a system signal that app should quit */
};

extern const EcsArrayParams handle_arr_params;
extern const EcsArrayParams stage_arr_params;
extern const EcsArrayParams table_arr_params;
extern const EcsArrayParams thread_arr_params;
extern const EcsArrayParams job_arr_params;
extern const EcsArrayParams column_arr_params;


#endif
