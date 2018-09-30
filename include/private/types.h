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
#define ECS_MAP_INITIAL_NODE_COUNT (64)
#define ECS_TABLE_INITIAL_ROW_COUNT (2)
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))

/* -- Builtin component types -- */

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef struct EcsId {
    const char *id;
} EcsId;


/* -- Private types -- */

typedef uint32_t EcsFamily;

typedef struct EcsTable {
    EcsArray *rows;
    EcsArray *family;             /* Reference to family_index entry */
    EcsArrayParams row_params;
    EcsFamily family_id;
    uint16_t *columns;
} EcsTable;

typedef struct EcsRow {
    uint32_t family_id;
    uint32_t index;
} EcsRow;

struct EcsWorld {
    EcsArray *table_db;
    EcsMap *entity_index;
    EcsMap *table_index;
    EcsMap *family_index;
    EcsMap *staging_index;

    EcsHandle component;          /* Component type entity */
    EcsHandle system;             /* System type entity */
    EcsHandle id;                 /* Id type entity */
    EcsHandle last_handle;        /* Last issued handle */
    void *context;                /* Application context */
};

extern const EcsArrayParams handle_arr_params;

#endif
