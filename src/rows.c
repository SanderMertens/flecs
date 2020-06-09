#include "flecs_private.h"

static
void* get_owned_column_ptr(
    const ecs_iter_t *it,
    size_t size,
    int32_t table_column,
    int32_t row)
{
    ecs_assert(it->table_columns != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    ecs_column_t *column = &((ecs_column_t*)it->table_columns)[table_column - 1];
    ecs_assert(column->size != 0, ECS_COLUMN_HAS_NO_DATA, NULL);
    ecs_assert(!size || column->size == size, ECS_COLUMN_TYPE_MISMATCH, NULL);
    void *buffer = ecs_vector_first_t(column->data, column->size, column->alignment);
    return ECS_OFFSET(buffer, column->size * (it->offset + row));
}

static
const void* get_shared_column(
    const ecs_iter_t *it,
    size_t size,
    int32_t table_column)
{
    ecs_assert(it->references != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

#ifndef NDEBUG
    if (size) {
        const EcsComponent *cdata = ecs_get(
            it->world, it->references[-table_column - 1].component, 
            EcsComponent);

        ecs_assert(cdata != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cdata->size == size, ECS_COLUMN_TYPE_MISMATCH, 
            ecs_get_name(it->world, it->system));
    }
#endif

    ecs_reference_t *ref = &it->references[-table_column - 1];

    return (void*)ecs_get_ref_w_entity(
        it->world,
        &ref->ref,
        ref->entity,
        ref->component);
}

static
bool get_table_column(
    const ecs_iter_t *it,
    int32_t column,
    int32_t *table_column_out)
{
    ecs_assert(column <= it->column_count, ECS_INTERNAL_ERROR, NULL);

    int32_t table_column = 0;

    if (column != 0) {
        ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        table_column = it->columns[column - 1];
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
    const ecs_iter_t *it,
    size_t size,
    int32_t column,
    int32_t row)
{
    int32_t table_column;

    if (!column) {
        return it->entities;
    }

    if (!get_table_column(it, column, &table_column)) {
        return NULL;
    }

    if (table_column < 0) {
        return (void*)get_shared_column(it, size, table_column);
    } else {
        return get_owned_column_ptr(it, size, table_column, row);
    }
}


/* --- Public API --- */

void* _ecs_column(
    const ecs_iter_t *it,
    size_t size,
    int32_t column)
{
    // ecs_assert(!ecs_is_readonly(it, column), ECS_COLUMN_ACCESS_VIOLATION, NULL);
    return get_column(it, size, column, 0);
}

const void* _ecs_const_column(
    const ecs_iter_t *it,
    size_t size,
    int32_t column)
{
    return get_column(it, size, column, 0);
}

void* _ecs_field(
    const ecs_iter_t *it, 
    size_t size,
    int32_t column,
    int32_t row)
{
    return get_column(it, size, column, row);
}

bool ecs_is_shared(
    const ecs_iter_t *it,
    int32_t column)
{
    int32_t table_column;

    if (!get_table_column(it, column, &table_column)) {
        return false;
    }

    return table_column < 0;
}

bool ecs_is_readonly(
    const ecs_iter_t *it,
    int32_t column)
{
    if (ecs_is_shared(it, column)) {
        return true;
    }

    ecs_query_t *query = it->query;
    if (query) {
        ecs_sig_column_t *column_data = ecs_vector_get(
            it->query->sig.columns, ecs_sig_column_t, column - 1);
        return column_data->inout_kind == EcsIn;
    } else {
        return true;
    }
}

ecs_entity_t ecs_column_source(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t table_column = it->columns[index - 1];
    if (table_column >= 0) {
        return 0;
    }

    ecs_assert(it->references != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_reference_t *ref = &it->references[-table_column - 1];

    return ref->entity;
}

ecs_type_t ecs_column_type(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->components != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t component = it->components[index - 1];
    return ecs_type_from_entity(it->world, component);
}

ecs_entity_t ecs_column_entity(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->components != NULL, ECS_INTERNAL_ERROR, NULL);

    return it->components[index - 1];
}

ecs_type_t ecs_table_type(
    const ecs_iter_t *it)
{
    ecs_table_t *table = it->table;
    return table->type;
}

void* ecs_table_column(
    const ecs_iter_t *it,
    int32_t column_index)
{
    ecs_world_t *world = it->world;
    ecs_table_t *table = it->table;
    ecs_data_t *data = ecs_table_get_data(world, table);
    ecs_column_t *column = &data->columns[column_index];
    return ecs_vector_first_t(column->data, column->size, column->alignment);
}
