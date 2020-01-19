#include "flecs_private.h"

static
void* get_owned_column_ptr(
    const ecs_rows_t *rows,
    size_t size,
    int32_t table_column,
    int32_t row)
{
    ecs_assert(rows->table_columns != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    ecs_column_t *column = &((ecs_column_t*)rows->table_columns)[table_column];
    ecs_assert(column->size != 0, ECS_COLUMN_HAS_NO_DATA, NULL);
    ecs_assert(!size || column->size == size, ECS_COLUMN_TYPE_MISMATCH, NULL);
    void *buffer = ecs_vector_first(column->data);
    return ECS_OFFSET(buffer, column->size * (rows->offset + row));
}

static
void* get_shared_column(
    const ecs_rows_t *rows,
    size_t size,
    int32_t table_column)
{
    ecs_assert(rows->references != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

#ifndef NDEBUG
    if (size) {
        EcsComponent *cdata = ecs_get_ptr(
            rows->world, rows->references[-table_column - 1].component, 
            EcsComponent);
        ecs_assert(cdata != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cdata->size == size, ECS_COLUMN_TYPE_MISMATCH, 
            ecs_get_id(rows->world, rows->system));
    }
#endif

    return rows->references[-table_column - 1].cached_ptr;    
}

static
bool get_table_column(
    const ecs_rows_t *rows,
    int32_t column,
    int32_t *table_column_out)
{
    ecs_assert(column <= rows->column_count, ECS_INTERNAL_ERROR, NULL);

    int32_t table_column = 0;

    if (column != 0) {
        ecs_assert(rows->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        table_column = rows->columns[column - 1];
        if (!table_column) {
            /* column is not set */
            return false;
        }
    }

    *table_column_out = table_column;

    return true;
}

static
void* get_column(
    const ecs_rows_t *rows,
    size_t size,
    int32_t column,
    int32_t row)
{
    int32_t table_column;

    if (!get_table_column(rows, column, &table_column)) {
        return NULL;
    }

    if (table_column < 0) {
        return get_shared_column(rows, size, table_column);
    } else {
        return get_owned_column_ptr(rows, size, table_column, row);
    }
}


/* --- Public API --- */

void* _ecs_column(
    const ecs_rows_t *rows,
    size_t size,
    int32_t column)
{
    return get_column(rows, size, column, 0);
}

void* _ecs_field(
    const ecs_rows_t *rows, 
    size_t size,
    int32_t column,
    int32_t row)
{
    return get_column(rows, size, column, row);
}

bool ecs_is_shared(
    const ecs_rows_t *rows,
    int32_t column)
{
    int32_t table_column;

    if (!get_table_column(rows, column, &table_column)) {
        return false;
    }

    return table_column < 0;
}

bool ecs_is_readonly(
    const ecs_rows_t *rows,
    int32_t column)
{
    if (ecs_is_shared(rows, column)) {
        return true;
    }

    ecs_sig_column_t *column_data = ecs_vector_get(
        rows->query->sig.columns, ecs_sig_column_t, column - 1);

    return column_data->inout_kind == EcsIn;
}

ecs_entity_t ecs_column_source(
    const ecs_rows_t *rows,
    int32_t index)
{
    ecs_assert(index <= rows->column_count, ECS_INVALID_PARAMETER, NULL);

    /* Index 0 (entity array) does not have a source */
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_assert(rows->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t table_column = rows->columns[index - 1];
    if (table_column >= 0) {
        return 0;
    }

    ecs_assert(rows->references != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_reference_t *ref = &rows->references[-table_column - 1];

    return ref->entity;
}

ecs_type_t ecs_column_type(
    const ecs_rows_t *rows,
    int32_t index)
{
    ecs_assert(index <= rows->column_count, ECS_INVALID_PARAMETER, NULL);

    /* Index 0 (entity array) does not have a type */
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_assert(rows->components != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t component = rows->components[index - 1];
    return ecs_type_from_entity(rows->world, component);
}

ecs_entity_t ecs_column_entity(
    const ecs_rows_t *rows,
    int32_t index)
{
    ecs_assert(index <= rows->column_count, ECS_INVALID_PARAMETER, NULL);

    /* Index 0 (entity array) does not have a type */
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_assert(rows->components != NULL, ECS_INTERNAL_ERROR, NULL);
    return rows->components[index - 1];
}

ecs_type_t ecs_table_type(
    const ecs_rows_t *rows)
{
    ecs_table_t *table = rows->table;
    return table->type;
}

void* ecs_table_column(
    const ecs_rows_t *rows,
    int32_t column)
{
    ecs_world_t *world = rows->world;
    ecs_table_t *table = rows->table;
    ecs_data_t *data = ecs_table_get_data(world, table);
    return ecs_vector_first(data->columns[column].data);
}
