#include "reflecs.h"

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

static
EcsResult ecs_table_init(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *components)
{
    size_t column_offset = sizeof(EcsEntity*);
    uint32_t column = 0;

    table->columns = malloc(sizeof(uint32_t) * (ecs_array_count(components) + 1));

    EcsIter it = ecs_array_iter(components, &entityptr_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);

        EcsComponent *type = ecs_get(e, world->component);
        if (!type) {
            return EcsError;
        }

        table->columns[column] = column_offset;
        column_offset += type->size;
        column ++;
    }

    table->columns[column] = column_offset;

    table->rows_params.element_size = column_offset;
    table->rows_params.chunk_count = REFLECS_INITIAL_CHUNK_COUNT;
    table->rows_params.compare_action = NULL;
    table->rows_params.move_action = ecs_table_row_move;
    table->rows_params.ctx = NULL;
    table->rows = ecs_vector_new(&table->rows_params);

    return EcsOk;
}

static
EcsTable* ecs_table_alloc(
    EcsWorld *world,
    uint64_t components_hash)
{
    EcsTable *result = ecs_vector_add(world->tables, &tables_vec_params);
    result->world = world;
    result->components_hash = components_hash;
    result->init_systems = NULL;
    result->deinit_systems = NULL;
    return result;
}

EcsTable* ecs_table_new(
    EcsWorld *world,
    uint64_t components_hash)
{
    EcsArray *components = ecs_world_get_components(world, components_hash);
    if (!components) {
        return NULL;
    }

    EcsTable *result = ecs_table_alloc(world, components_hash);
    if (!result) {
        return NULL;
    }

    if (ecs_table_init(world, result, components) != EcsOk) {
        ecs_vector_remove(world->tables, &tables_vec_params, result);
        return NULL;
    }

    return result;
}

EcsTable* ecs_table_new_w_size(
    EcsWorld *world,
    uint64_t components_hash,
    size_t size)
{
    EcsTable *result = ecs_table_alloc(world, components_hash);
    if (!result) {
        return NULL;
    }

    result->rows_params.chunk_count = REFLECS_INITIAL_CHUNK_COUNT;
    result->rows_params.element_size = size + sizeof(EcsEntity*);
    result->rows_params.compare_action = NULL;
    result->rows_params.move_action = ecs_table_row_move;
    result->rows_params.ctx = NULL;
    result->columns = malloc(sizeof(uint32_t) * 2);
    result->columns[0] = sizeof(EcsEntity*);
    result->columns[1] = sizeof(EcsEntity*) + size;
    result->rows = ecs_vector_new(&result->rows_params);

    return result;
}

void* ecs_table_insert(
    EcsTable *table,
    EcsEntity *entity)
{
    void* row = ecs_vector_add(table->rows, &table->rows_params);
    memset(row, 0, table->rows_params.element_size);
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

int32_t ecs_table_find_column(
    EcsTable *table,
    EcsEntity *component)
{
    EcsWorld *world = table->world;
    EcsArray *set = ecs_world_get_components(world, table->components_hash);
    if (!set) {
        return -1;
    }

    uint32_t column = 0;
    EcsIter it = ecs_array_iter(set, &entityptr_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
        if (e == component) {
            return column;
        }
        column ++;
    }

    return -1;
}

bool ecs_table_has_components(
    EcsTable *table,
    EcsArray *components)
{
    EcsIter it = ecs_array_iter(components, &entityptr_arr_params);

    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
        if (ecs_table_find_column(table, e) == -1) {
            return false;
        }
    }

    return true;
}

void ecs_table_add_on_init(
    EcsTable *table,
    EcsEntity *system)
{
    if (!table->init_systems) {
        table->init_systems = ecs_vector_new(&entityptr_vec_params);
    }

    EcsEntity **e = ecs_vector_add(table->init_systems, &entityptr_vec_params);
    *e = system;
}

void ecs_table_add_on_deinit(
    EcsTable *table,
    EcsEntity *system)
{
    if (!table->deinit_systems) {
        table->deinit_systems = ecs_vector_new(&entityptr_vec_params);
    }

    EcsEntity **e = ecs_vector_add(table->deinit_systems, &entityptr_vec_params);
    *e = system;
}
