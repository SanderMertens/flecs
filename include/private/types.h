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

/* -- Builtin component types -- */

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef struct EcsId {
    const char *id;
} EcsId;

typedef struct EcsSystem {
    EcsSystemAction action; /* Callback to be invoked for matching rows */
    EcsArray *components;   /* System components in specified order */
    EcsArray *tables;       /* Table index + column offsets for components */
    EcsArray *inactive_tables; /* Inactive tables */
    /* EcsArray *jobs; */
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
    EcsArray *init_systems;       /* Systems to run on init */
    EcsArray *deinit_systems;     /* Systems to run on deinit */
    EcsArrayParams row_params;    /* Parameters for rows array */
    EcsFamily family_id;          /* Identifies a family in family_index */
    uint16_t *columns;            /* Column (component) sizes */
} EcsTable;

typedef struct EcsRow {
    uint32_t family_id;           /* Identifies a family in family_index */
    uint32_t index;               /* Index of the entity in its table */
} EcsRow;

struct EcsWorld {
    EcsArray *table_db;           /* Table storage */
    EcsArray *periodic_systems;   /* Periodic systems */
    EcsArray *inactive_systems;   /* Periodic systems with empty tables */
    EcsArray *other_systems;      /* Array with non-periodic systems */
    EcsMap *entity_index;         /* Maps entity handle to EcsRow  */
    EcsMap *table_index;          /* Identifies a table by family_id */
    EcsMap *family_index;         /* References to component families */
    EcsMap *staging_index;        /* Staged family_id of uncommitted entities */

    void *context;                /* Application context */

    EcsHandle component;          /* Component type entity */
    EcsHandle system;             /* System type entity */
    EcsHandle id;                 /* Id type entity */
    EcsHandle last_handle;        /* Last issued handle */
};

extern const EcsArrayParams handle_arr_params;
extern const EcsArrayParams table_arr_params;

#endif
