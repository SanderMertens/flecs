#ifndef REFLECS_TYPES_PRIVATE_H
#define REFLECS_TYPES_PRIVATE_H

#include <stdlib.h>
#include <pthread.h>

#include "array.h"
#include "vector.h"
#include "map.h"

#define REFLECS_INITIAL_TABLE_COUNT (8)
#define REFLECS_INITIAL_ENTITY_COUNT (8)
#define REFLECS_ENTITIES_CHUNK_COUNT (256)
#define REFLECS_TABLES_CHUNK_COUNT (4)
#define REFLECS_SYSTEMS_CHUNK_COUNT (4)
#define REFLECS_ROW_CHUNK_COUNT (64)
#define REFLECS_THREAD_CHUNK_COUNT (8)
#define REFLECS_INITIAL_COMPONENT_SET_COUNT (8)

#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))

typedef struct EcsEntity EcsEntity;


/* -- Builtin component types -- */

typedef struct EcsSystem {
    EcsSystemAction action;
    EcsArray *components;
    EcsVector *tables;
    EcsVector *jobs;
    EcsVectorParams tables_params;
    EcsSystemKind kind;
    bool enabled;
} EcsSystem;

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef struct EcsId {
    const char *id;
} EcsId;


/* -- Private types -- */

struct EcsEntity {
    uint64_t stage_hash;
    uint64_t table_hash;
    void *row;
};

typedef struct EcsTable {
    uint64_t components_hash;     /* Hash of the components */
    EcsWorld *world;              /* World associated with the table */
    EcsVectorParams rows_params;  /* Parameters for the rows vector */
    EcsVector *rows;              /* vector<T> */
    uint32_t *columns;            /* Offsets to columns in row */
    EcsVector *init_systems;      /* OnInit systems active on this table */
    EcsVector *deinit_systems;    /* OnDeinit systems active on this table */
} EcsTable;

typedef struct EcsSystemTable {
    EcsTable *table;
} EcsSystemTable;

typedef struct EcsJob {
    EcsHandle system;               /* System handle */
    EcsSystem *system_data;         /* System to run */
    EcsVectorChunk *chunk;          /* Chunk of row vector */
    uint32_t table_index;           /* Current SystemTable */
    uint32_t chunk_index;           /* Start index in row chunk */
    uint32_t total_rows;            /* Total number of rows to process */
} EcsJob;

typedef struct EcsThread {
    EcsWorld *world;
    EcsJob *job;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool quit;
    bool running;
} EcsThread;

struct EcsWorld {
    EcsVector *entities;          /* vector<EcsEntity> */
    EcsVector *tables;            /* vector<EcsTable> */
    EcsVector *periodic_systems;  /* vector<EcsHandle> Periodic systems */
    EcsVector *other_systems;     /* vector<EcsHandle> Non-periodic systems */
    EcsVector *worker_threads;    /* vector<EcsThread> Worker threads */
    pthread_cond_t job_cond;      /* Signal that worker thread job is done */
    pthread_mutex_t job_mutex;    /* Mutex for protecting job counter */
    uint32_t jobs_finished;       /* Number of jobs finished */
    EcsMap *entities_map;         /* Map for quick entity lookups */
    EcsMap *tables_map;           /* Map for quick table lookups */
    EcsMap *components_map;       /* Map that stores component sets */
    EcsHandle component;          /* Component type entity */
    EcsHandle system;             /* System type entity */
    EcsHandle id;                 /* Id type entity */
    void *context;                /* Application context */
    bool valid_schedule;          /* Is the current job schedule valid */
};

extern const EcsVectorParams entities_vec_params;
extern const EcsVectorParams tables_vec_params;
extern const EcsVectorParams handle_vec_params;
extern const EcsArrayParams handle_arr_params;

#endif
