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
#define ECS_MAP_INITIAL_NODE_COUNT (64)
#define ECS_TABLE_INITIAL_ROW_COUNT (8)
#define ECS_SYSTEM_INITIAL_TABLE_COUNT (4)
#define ECS_MAX_JOBS_PER_WORKER (16)

/* -- Builtin component types -- */

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef struct EcsId {
    const char *id;
} EcsId;

typedef struct EcsSystem {
    EcsFamily family_id;    /* Family with all system components */
    EcsSystemAction action; /* Callback to be invoked for matching rows */
    EcsArray *components;   /* System components in specified order */
    EcsArray *tables;       /* Table index + column offsets for components */
    EcsArray *inactive_tables; /* Inactive tables */
    EcsArray *jobs;
    EcsArrayParams table_params; /* Parameters for table array */
    EcsSystemKind kind;     /* Kind that determines when system is invoked */
    bool enabled;           /* Is system enabled or not */
} EcsSystem;

/* -- Private types -- */

typedef struct EcsTable {
    EcsWorld *world;              /* Reference to the world */
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
    uint32_t family_id;           /* Identifies a family in family_index */
    uint32_t index;               /* Index of the entity in its table */
} EcsRow;

typedef struct EcsJob {
    EcsHandle system;               /* System handle */
    EcsSystem *system_data;         /* System to run */
    uint32_t table_index;           /* Current SystemTable */
    uint32_t start_index;           /* Start index in row chunk */
    uint32_t row_count;             /* Total number of rows to process */
} EcsJob;

typedef struct EcsThread {
    EcsWorld *world;
    EcsJob *jobs[ECS_MAX_JOBS_PER_WORKER];
    uint32_t job_count;
    pthread_t thread;
} EcsThread;

struct EcsWorld {
    EcsArray *table_db;           /* Table storage */
    EcsArray *periodic_systems;   /* Periodic systems */
    EcsArray *inactive_systems;   /* Periodic systems with empty tables */
    EcsArray *other_systems;      /* Array with non-periodic systems */

    EcsMap *entity_index;         /* Maps entity handle to EcsRow  */
    EcsMap *table_index;          /* Identifies a table by family_id */
    EcsMap *add_stage;            /* Entities with components to add */
    EcsMap *remove_stage;         /* Entities with components to remove */
    EcsMap *family_index;         /* References to component families */

    EcsArray *worker_threads;     /* Worker threads */
    pthread_cond_t thread_cond;   /* Signal that worker threads can start */
    pthread_mutex_t thread_mutex; /* Mutex for thread condition */
    pthread_cond_t job_cond;      /* Signal that worker thread job is done */
    pthread_mutex_t job_mutex;    /* Mutex for protecting job counter */
    uint32_t jobs_finished;       /* Number of jobs finished */
    uint32_t threads_running;     /* Number of threads running */

    void *context;                /* Application context */

    EcsHandle component;          /* Component type entity */
    EcsHandle system;             /* System type entity */
    EcsHandle id;                 /* Id type entity */
    EcsHandle last_handle;        /* Last issued handle */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
};

extern const EcsArrayParams handle_arr_params;
extern const EcsArrayParams table_arr_params;

#endif
