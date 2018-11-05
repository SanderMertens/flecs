#ifndef REFLECS_TYPES_PRIVATE_H
#define REFLECS_TYPES_PRIVATE_H

#include <stdlib.h>
#include <pthread.h>

#include <reflecs/reflecs.h>
#include "array.h"
#include "map.h"

#define ECS_WORLD_INITIAL_TABLE_COUNT (8)
#define ECS_WORLD_INITIAL_ENTITY_COUNT (64)
#define ECS_WORLD_INITIAL_STAGING_COUNT (4)
#define ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT (4)
#define ECS_WORLD_INITIAL_OTHER_SYSTEM_COUNT (4)
#define ECS_WORLD_INITIAL_PREFAB_COUNT (4)
#define ECS_MAP_INITIAL_NODE_COUNT (64)
#define ECS_TABLE_INITIAL_ROW_COUNT (8)
#define ECS_SYSTEM_INITIAL_TABLE_COUNT (4)
#define ECS_MAX_JOBS_PER_WORKER (16)

/** A family identifies a set of components */
typedef uint32_t EcsFamily;

/* -- Builtin component types -- */

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef struct EcsId {
    const char *id;
} EcsId;

typedef enum EcsSystemExprElemKind {
    EcsFromEntity,
    EcsFromComponent
} EcsSystemExprElemKind;

typedef enum EcsSystemExprOperKind {
    EcsOperAnd = 0,
    EcsOperOr = 1,
    EcsOperNot = 2,
    EcsOperLast = 3
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
        EcsHandle component;          /* Used for AND operator */
    } is;
} EcsSystemColumn;

typedef struct EcsSystemRef {
    EcsHandle entity;
    EcsHandle component;
} EcsSystemRef;

typedef struct EcsSystem {
    EcsSystemAction action;    /* Callback to be invoked for matching rows */
    EcsArray *columns;         /* Column components (AND) and families (OR) */
    EcsArray *components;      /* Computed component list per matched table */
    EcsArray *inactive_tables; /* Inactive tables */
    EcsArray *jobs;            /* Jobs for this system */
    EcsArray *tables;          /* Table index + refs index + column offsets */
    EcsArray *refs;            /* Columns that point to other entities */
    EcsArrayParams table_params; /* Parameters for tables array */
    EcsArrayParams component_params; /* Parameters for components array */
    EcsArrayParams ref_params; /* Parameters for tables array */
    EcsSystemKind kind;        /* Kind that determines when system is invoked */
    EcsFamily not_from_entity;    /* Exclude components from entity */
    EcsFamily not_from_component; /* Exclude components from components */
    EcsFamily and_from_entity; /* Used to match init / deinit notifications */
    bool enabled;              /* Is system enabled or not */
} EcsSystem;

/* -- Private types -- */

typedef struct EcsTable {
    EcsArray *family;             /* Reference to family_index entry */
    EcsArray *rows;               /* Rows of the table */
    EcsArray *periodic_systems;   /* Periodic systems matched with table */
    EcsArray *init_systems;       /* Init systems, indexed by family */
    EcsArray *deinit_systems;     /* Deinit systems, indexed by family */
    EcsArrayParams row_params;    /* Parameters for rows array */
    EcsFamily family_id;          /* Identifies a family in family_index */
    uint16_t *columns;            /* Column (component) sizes */
} EcsTable;

typedef struct EcsRow {
    uint32_t family_id;           /* Identifies a family (and table) in world */
    uint32_t index;               /* Index of the entity in its table */
} EcsRow;

typedef struct EcsStage {
    EcsMap *add_stage;            /* Entities with components to add */
    EcsMap *remove_stage;         /* Entities with components to remove */
    EcsMap *remove_merge;         /* All removed components before merge */
    EcsArray *delete_stage;       /* Deleted entities while in progress */
    EcsMap *entity_stage;         /* Committed entities in stage */
    EcsMap *data_stage;           /* Arrays with staged component values */
} EcsStage;

typedef struct EcsJob {
    EcsHandle system;             /* System handle */
    EcsSystem *system_data;       /* System to run */
    uint32_t table_index;         /* Current SystemTable */
    uint32_t start_index;         /* Start index in row chunk */
    uint32_t row_count;           /* Total number of rows to process */
} EcsJob;

typedef struct EcsThread {
    EcsWorld *world;
    EcsJob *jobs[ECS_MAX_JOBS_PER_WORKER];
    EcsStage *stage;
    uint32_t job_count;
    pthread_t thread;
} EcsThread;

struct EcsWorld {
    EcsArray *table_db;           /* Table storage */
    EcsArray *periodic_systems;   /* Periodic systems */
    EcsArray *inactive_systems;   /* Periodic systems with empty tables */
    EcsArray *other_systems;      /* Non-periodic systems */

    EcsMap *entity_index;         /* Maps entity handle to EcsRow  */
    EcsMap *table_index;          /* Identifies a table by family_id */
    EcsMap *family_index;         /* References to component families */
    EcsMap *family_handles;       /* Index to explicitly created families */
    EcsMap *prefab_index;         /* Index for finding prefabs in families */

    EcsStage stage;              /* Stage of main thread */

    EcsArray *worker_threads;     /* Worker threads */
    pthread_cond_t thread_cond;   /* Signal that worker threads can start */
    pthread_mutex_t thread_mutex; /* Mutex for thread condition */
    pthread_cond_t job_cond;      /* Signal that worker thread job is done */
    pthread_mutex_t job_mutex;    /* Mutex for protecting job counter */
    uint32_t jobs_finished;       /* Number of jobs finished */
    uint32_t threads_running;     /* Number of threads running */

    void *context;                /* Application context */

    EcsHandle last_handle;        /* Last issued handle */
    EcsHandle deinit_system;      /* Handle to internal deinit system */

    EcsFamily component_family;   /* EcsComponent, EcsId */
    EcsFamily system_family;      /* EcsSystem, EcsId */
    EcsFamily family_family;      /* EcsFamily, EcsId */
    EcsFamily prefab_family;      /* EcsPrefab, EcsId */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
    bool in_progress;             /* Is world being progressed */
    bool auto_merge;              /* Are stages auto-merged by ecs_progress */
};

extern const EcsArrayParams handle_arr_params;
extern const EcsArrayParams table_arr_params;

#endif
