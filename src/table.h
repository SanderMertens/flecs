#ifndef REFLECS_TABLE_PRIVATE_H
#define REFLECS_TABLE_PRIVATE_H

#include <reflecs/reflecs.h>
#include <reflecs/vector.h>


/* -- Private types -- */

typedef struct EcsTable {
    uint64_t components_hash;     /* Hash of the components */
    EcsWorld *world;              /* World associated with the table */
    EcsVectorParams rows_params;  /* Parameters for the rows vector */
    EcsVector *rows;              /* vector<T> */
    uint32_t *columns;            /* Offsets to columns in row */
} EcsTable;


/* -- Private functions -- */

EcsTable* ecs_table_new(
    EcsWorld *world,
    uint64_t components_hash);

EcsTable* ecs_table_new_w_size(
    EcsWorld *world,
    uint64_t components_hash,
    size_t size);

void ecs_table_add_component(
    EcsTable *table,
    EcsEntity *component_type);

void* ecs_table_insert(
    EcsTable *table,
    EcsEntity *entity);

void ecs_table_remove(
    EcsTable *table,
    void *row);

int32_t ecs_table_find_column(
    EcsTable *table,
    EcsEntity *component);

void* ecs_table_column(
    EcsTable *table,
    void *row,
    uint32_t column);

size_t ecs_table_column_size(
    EcsTable *table,
    uint32_t column);

#endif
