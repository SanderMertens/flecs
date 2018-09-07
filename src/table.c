#include <ecs/components.h>
#include <ecs/hash.h>
#include <ecs/entity.h>
#include "table.h"
#include "entity.h"

static const EcsVectorParams components_vec_params = {
    .element_size = sizeof(EcsEntity*),
    .chunk_count = ECS_INITIAL_CHUNK_COUNT
};

static
void ecs_table_row_move(
    void *to,
    void *from,
    void *ctx)
{
    EcsEntity *e = *(EcsEntity**)from;
    *(EcsEntity**)to = e;
    ecs_entity_move(e, to, from);
}

EcsTable* ecs_table_new(
    EcsWorld *world)
{
    EcsTable *result = ecs_world_alloc_table(world);
    result->world = world;
    result->components = ecs_vector_new(&components_vec_params);
    result->components_hash = 0;
    result->rows = NULL;
    ecs_world_add_table(world, result);
    return result;
}

void ecs_table_add_component(
    EcsTable *table,
    EcsEntity *component_type)
{
    if (!table->rows) {
        EcsEntity **el = ecs_vector_add(
            table->components, &components_vec_params);
        *el = component_type;
    }
}

EcsResult ecs_table_finalize(
    EcsTable *table)
{
    EcsIter it = ecs_vector_iter(table->components, &components_vec_params);
    size_t table_size = sizeof(EcsEntity*);
    uint64_t components_hash = 0;
    uint32_t component_count = ecs_vector_count(table->components);
    uint32_t column = 0;

    if (!component_count) {
        goto error;
    }

    table->columns = malloc((1 + component_count) * sizeof(uint32_t));

    ecs_vector_sort(
        table->components, &components_vec_params, ecs_vector_compare_ptr);

    while (ecs_iter_hasnext(&it)) {
        EcsEntity **e = ecs_iter_next(&it);
        EcsType *t = ecs_get(*e, EcsType_e);
        if (!t) {
            goto error;
        }

        table->columns[column] = table_size;
        table_size += t->size;
        column ++;
        ecs_hash(*e, sizeof(EcsEntity*), &components_hash);
    }

    table->columns[column] = table_size;
    table->rows_params.element_size = table_size;
    table->rows_params.chunk_count = ECS_INITIAL_CHUNK_COUNT;
    table->rows_params.move_action = ecs_table_row_move;
    table->components_hash = components_hash;
    table->rows = ecs_vector_new(&table->rows_params);

    ecs_world_add_table(table->world, table);
    return EcsOk;
error:
    ecs_world_remove_table(table->world, table);
    return EcsError;
}

void* ecs_table_insert(
    EcsTable *table,
    EcsEntity *entity)
{
    void* row = ecs_vector_add(table->rows, &table->rows_params);
    *(EcsEntity**)row = entity;
    return row;
}

void ecs_table_remove(
    EcsTable *table,
    void *row)
{
    ecs_vector_remove(table->rows, &table->rows_params, row);
}

void* ecs_table_column(
    EcsTable *table,
    void *row,
    uint32_t column)
{
    return ECS_OFFSET(row, table->columns[column]);
}

size_t ecs_table_column_size(
    EcsTable *table,
    uint32_t column)
{
    uint32_t *columns = table->columns;
    return columns[column + 1] - columns[column];
}
