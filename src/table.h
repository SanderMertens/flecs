#ifndef ECS_TABLE_PRIVATE_H
#define ECS_TABLE_PRIVATE_H

#include <ecs/entity.h>
#include <ecs/vector.h>


/* -- Private types -- */

typedef struct EcsTable {
    EcsVector *components;        /* Components stored in the table */
    uint64_t components_hash;     /* Hash of the components */
    EcsWorld *world;              /* World associated with the table */
    EcsVectorParams rows_params;  /* Parameters for the rows vector */
    EcsVector *rows;              /* vector<T> */
} EcsTable;


/* -- Private functions -- */

EcsTable* ecs_table_new(
    EcsWorld *world);

void ecs_table_add_component(
    EcsTable *table,
    EcsEntity *component_type);

EcsResult ecs_table_finalize(
    EcsTable *table);

void* ecs_table_insert(
    EcsTable *table);

#endif
