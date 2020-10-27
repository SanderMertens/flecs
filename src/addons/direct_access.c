#include "flecs.h"

#ifdef FLECS_DIRECT_ACCESS

#include "../private_api.h"

/* Prefix with "da" so that they don't conflict with other get_column's */

static
ecs_column_t *da_get_column(
    ecs_table_t *table,
    int32_t column)
{
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(column <= table->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_data_t *data = table->data;
    if (data && data->columns) {
        return &table->data->columns[column];    
    } else {
        return NULL;
    }    
}

static
ecs_column_t *da_get_or_create_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column)
{
    ecs_column_t *c = da_get_column(table, column);
    if (!c && (!table->data || !table->data->columns)) {
        ecs_data_t *data = ecs_table_get_or_create_data(table);
        ecs_init_data(world, table, data);
        c = da_get_column(table, column);
    }
    ecs_assert(c != NULL, ECS_INTERNAL_ERROR, NULL);
    return c;
}

static
ecs_entity_t* get_entity_array(
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->data->entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t *array = ecs_vector_first(table->data->entities, ecs_entity_t);
    return &array[row];
}

/* -- Public API -- */

ecs_type_t ecs_table_get_type(
    ecs_table_t *table)
{
    return table->type;
}

ecs_record_t* ecs_record_find(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_record_t *r = ecs_eis_get(world, entity);
    if (r) {
        return r;
    } else {
        return NULL;
    }
}

ecs_record_t ecs_table_insert(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_data_t *data = ecs_table_get_or_create_data(table);
    int32_t index = ecs_table_append(world, table, data, entity, record, true);
    if (record) {
        record->table = table;
        record->row = index + 1;
    }
    return (ecs_record_t){table, index + 1};
}

int32_t ecs_table_find_column(
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    return ecs_type_index_of(table->type, component);
}

ecs_vector_t* ecs_table_get_column(
    ecs_table_t *table,
    int32_t column)
{
    ecs_column_t *c = da_get_column(table, column);
    return c ? c->data : NULL;
}

void ecs_table_set_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column,
    ecs_vector_t* vector)
{
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    if (vector) {
        ecs_vector_assert_size(vector, c->size);
    }
    c->data = vector;
}

ecs_vector_t* ecs_table_get_entities(
    ecs_table_t *table)
{
    ecs_data_t *data = table->data;
    if (!data) {
        return NULL;
    }

    return data->entities;
}

ecs_vector_t* ecs_table_get_records(
    ecs_table_t *table)
{
    ecs_data_t *data = table->data;
    if (!data) {
        return NULL;
    }

    return data->record_ptrs;
}

void ecs_table_set_entities(
    ecs_table_t *table,
    ecs_vector_t *entities,
    ecs_vector_t *records)
{
    ecs_vector_assert_size(entities, sizeof(ecs_entity_t));
    ecs_vector_assert_size(records, sizeof(ecs_record_t*));
    ecs_assert(ecs_vector_count(entities) == ecs_vector_count(records), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_data_t *data = table->data;
    if (!data) {
        data = ecs_table_get_or_create_data(table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    data->entities = entities;
    data->record_ptrs = records;
}

void ecs_table_delete_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column,
    ecs_vector_t *vector)
{
    if (!vector) {
        vector = ecs_table_get_column(table, column);
        if (!vector) {
            return;
        }

        ecs_assert(table->data != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(table->data->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        table->data->columns[column].data = NULL;
    }

    ecs_column_t *c = da_get_or_create_column(world, table, column);
    ecs_vector_assert_size(vector, c->size);

    ecs_c_info_t *c_info = table->c_info[column];
    ecs_xtor_t dtor;
    if (c_info && (dtor = c_info->lifecycle.dtor)) {
        ecs_entity_t *entities = get_entity_array(table, 0);
        int16_t alignment = c->alignment;
        int32_t count = ecs_vector_count(vector);
        void *ptr = ecs_vector_first_t(vector, c->size, alignment);
        dtor(world, c_info->component, entities, ptr, ecs_to_size_t(c->size), 
            count, c_info->lifecycle.ctx);
    }

    ecs_vector_free(vector);
}

void* ecs_record_get_column(
    ecs_record_t *r,
    int32_t column,
    size_t c_size)
{
    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_column(table, column);
    if (!c) {
        return NULL;
    }

    int16_t size = c->size;
    ecs_assert(!ecs_from_size_t(c_size) || ecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    void *array = ecs_vector_first_t(c->data, c->size, c->alignment);
    bool is_watched;
    int32_t row = ecs_record_to_row(r->row, &is_watched);
    return ECS_OFFSET(array, size * row);
}

void ecs_record_copy_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t c_size,
    const void *value,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(c_size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    int16_t size = c->size;
    ecs_assert(!ecs_from_size_t(c_size) || ecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    int16_t alignment = c->alignment;
    bool is_monitored;
    int32_t row = ecs_record_to_row(r->row, &is_monitored);
    void *ptr = ecs_vector_get_t(c->data, size, alignment, row);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_c_info_t *c_info = table->c_info[column];
    ecs_copy_t copy;
    if (c_info && (copy = c_info->lifecycle.copy)) {
        ecs_entity_t *entities = get_entity_array(table, row);
        copy(world, c_info->component, entities, entities, ptr, value, c_size, 
            count, c_info->lifecycle.ctx);
    } else {
        ecs_os_memcpy(ptr, value, size * count);
    }
}

void ecs_record_copy_pod_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t c_size,
    const void *value,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(c_size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    int16_t size = c->size;
    ecs_assert(!ecs_from_size_t(c_size) || ecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    int16_t alignment = c->alignment;
    bool is_monitored;
    int32_t row = ecs_record_to_row(r->row, &is_monitored);
    void *ptr = ecs_vector_get_t(c->data, size, alignment, row);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_os_memcpy(ptr, value, size * count);
}

void ecs_record_move_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t c_size,
    void *value,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(c_size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    int16_t size = c->size;
    ecs_assert(!ecs_from_size_t(c_size) || ecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    int16_t alignment = c->alignment;
    bool is_monitored;
    int32_t row = ecs_record_to_row(r->row, &is_monitored);
    void *ptr = ecs_vector_get_t(c->data, size, alignment, row);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_c_info_t *c_info = table->c_info[column];
    ecs_move_t move;
    if (c_info && (move = c_info->lifecycle.move)) {
        ecs_entity_t *entities = get_entity_array(table, row);
        move(world, c_info->component, entities, entities, ptr, value, c_size, 
            count, c_info->lifecycle.ctx);
    } else {
        ecs_os_memcpy(ptr, value, size * count);
    }
}

#endif
