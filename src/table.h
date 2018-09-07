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
    uint32_t *columns;            /* Offsets to columns in row */
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
    EcsTable *table,
    EcsEntity *entity);

void ecs_table_remove(
    EcsTable *table,
    void *row);

void* ecs_table_column(
    EcsTable *table,
    void *row,
    uint32_t column);

size_t ecs_table_column_size(
    EcsTable *table,
    uint32_t column);

#endif
