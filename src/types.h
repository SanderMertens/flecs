#ifndef REFLECS_TYPES_PRIVATE_H
#define REFLECS_TYPES_PRIVATE_H

#include <reflecs/map.h>
#include <reflecs/vector.h>
#include <reflecs/array.h>

#define REFLECS_INITIAL_TABLE_COUNT (8)
#define REFLECS_INITIAL_ENTITY_COUNT (8)
#define REFLECS_INITIAL_CHUNK_COUNT (64)
#define REFLECS_INITIAL_COMPONENT_SET_COUNT (8)


/* -- Private types -- */

struct EcsEntity {
    char *id;
    uint64_t stage_hash;
    uint64_t table_hash;
    void *row;
    EcsWorld *world;
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

struct EcsWorld {
    EcsVector *entities;          /* vector<EcsEntity> */
    EcsVector *tables;            /* vector<EcsTable> */
    EcsVector *systems;           /* vector<EcsEntity*> Periodic systems */
    EcsMap *entities_map;         /* Map for quick entity lookups */
    EcsMap *tables_map;           /* Map for quick table lookups */
    EcsMap *components_map;       /* Map that stores component sets */
    EcsEntity *component;         /* Component type entity */
    EcsEntity *system;            /* System type entity */
    void *context;                /* Application context */
};

extern const EcsVectorParams entities_vec_params;
extern const EcsVectorParams tables_vec_params;
extern const EcsVectorParams entityptr_vec_params;
extern const EcsArrayParams entityptr_arr_params;

#endif
