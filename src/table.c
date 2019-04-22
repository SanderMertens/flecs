#include <assert.h>
#include "include/private/flecs.h"

/** Notify systems that a table has changed its active state */
static
void activate_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t system,
    bool activate)
{
    if (system) {
        ecs_system_activate_table(world, system, table, activate);
    } else {
        ecs_vector_t *systems = table->frame_systems;
        
        if (systems) {
            ecs_entity_t *buffer = ecs_vector_first(systems);
            uint32_t i, count = ecs_vector_count(systems);
            for (i = 0; i < count; i ++) {
                ecs_system_activate_table(world, buffer[i], table, activate);
            }
        }
    }
}

static
ecs_table_column_t* new_columns(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *type)
{
    ecs_table_column_t *result = ecs_os_calloc(
        sizeof(ecs_table_column_t), ecs_vector_count(type) + 1);

    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    ecs_entity_t *buf = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

    /* First column is reserved for storing entity id's */
    result[0].size = sizeof(ecs_entity_t);
    result[0].data = NULL;

    for (i = 0; i < count; i ++) {
        ecs_entity_info_t info = {0};
        EcsComponent *component = get_ptr(
            world, stage, buf[i], EEcsComponent, false, false, &info);

        if (component) {
            if (component->size) {
                /* Regular column data */
                result[i + 1].size = component->size;
            }
        }
    }
    
    return result;
}

/* -- Private functions -- */

ecs_table_column_t* ecs_table_get_columns(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    if (!world->in_progress) {
        return table->columns;
    } else {
        ecs_type_t type_id = table->type_id;
        ecs_table_column_t *columns = ecs_map_get(stage->data_stage, type_id);

        if (!columns) {
            ecs_vector_t *type = table->type;
            columns = new_columns(world, stage, type);
            ecs_map_set(stage->data_stage, type_id, columns);
        }

        return columns;
    }
}

int ecs_table_init(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    ecs_vector_t *type = ecs_type_get(world, stage, table->type_id);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, "invalid type id of table");
    bool prefab_set = false;

    table->frame_systems = NULL;
    table->type = type;
    table->columns = new_columns(world, stage, type);

    if (stage == &world->main_stage) {
        ecs_entity_t *buf = ecs_vector_first(type);
        uint32_t i, count = ecs_vector_count(type);

        for (i = 0; i < count; i ++) {
            ecs_assert(buf[i] <= world->last_handle, ECS_INVALID_HANDLE, NULL);
            
            /* Only if creating columns in the main stage, register prefab */
            if (!ecs_has(world, buf[i], EcsComponent)) {
                if (ecs_has(world, buf[i], EcsPrefab)) {
                    /* Tables can contain at most one prefab */
                    ecs_assert(prefab_set == false, ECS_MORE_THAN_ONE_PREFAB, ecs_get_id(world, buf[i]));
                    prefab_set = true;

                    /* Register type with prefab index for quick lookups */
                    ecs_map_set(world->prefab_index, table->type_id, buf[i]);
                }
            }
        }
    }

    return 0;
}

void ecs_table_deinit(
    ecs_world_t *world,
    ecs_table_t *table)
{
    uint32_t count = ecs_vector_count(table->columns[0].data);
    if (count) {
        ecs_notify(
            world, world->type_sys_remove_index, table->type_id, table,
            table->columns, 0, count);
    }
}

void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    uint32_t i, column_count = ecs_vector_count(table->type);
    (void)world;
    
    for (i = 0; i < column_count + 1; i ++) {
        ecs_vector_free(table->columns[i].data);
    }

    ecs_os_free(table->columns);

    ecs_vector_free(table->frame_systems);
}

void ecs_table_register_system(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t system)
{
    /* Register system with the table */
    ecs_entity_t *h = ecs_vector_add(&table->frame_systems, &handle_arr_params);
    if (h) *h = system;

    if (ecs_vector_count(table->columns[0].data)) {
        activate_table(world, table, system, true);
    }
}

uint32_t ecs_table_insert(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    ecs_entity_t entity)
{
    uint32_t column_count = ecs_vector_count(table->type);

    /* Fist add entity to column with entity ids */
    ecs_entity_t *e = ecs_vector_add(&columns[0].data, &handle_arr_params);
    if (!e) {
        return -1;
    }

    *e = entity;

    /* Add elements to each column array */
    uint32_t i;
    for (i = 1; i < column_count + 1; i ++) {
        uint32_t size = columns[i].size;
        if (size) {
            ecs_vector_params_t params = {.element_size = size};
            if (!ecs_vector_add(&columns[i].data, &params)) {
                return -1;
            }
        }
    }

    uint32_t index = ecs_vector_count(columns[0].data) - 1;

    if (!world->in_progress && !index) {
        activate_table(world, table, 0, true);
    }

    /* Return index of last added entity */
    return index + 1;
}

void ecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index)
{
    ecs_table_column_t *columns = table->columns;
    ecs_vector_t *entity_column = columns[0].data;
    uint32_t count = ecs_vector_count(entity_column);

    if (index < 0) {
        index *= -1;
    }

    index --;

    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    count --;
    
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    uint32_t column_last = ecs_vector_count(table->type) + 1;
    uint32_t i;

    if (index != count) {        
        /* Move last entity in array to index */
        ecs_entity_t *entities = ecs_vector_first(entity_column);
        ecs_entity_t to_move = entities[count];
        entities[index] = to_move;

        for (i = 1; i < column_last; i ++) {
            if (columns[i].size) {
                ecs_vector_params_t params = {.element_size = columns[i].size};
                ecs_vector_remove_index(columns[i].data, &params, index);
            }
        }

        /* Last entity in table is now moved to index of removed entity */
        ecs_row_t row;
        row.type_id = table->type_id;
        row.index = index + 1;
        ecs_map_set64(world->main_stage.entity_index, to_move, ecs_from_row(row));

        /* Decrease size of entity column */
        ecs_vector_remove_last(entity_column);

    /* This was the last entity, free all columns */
    } else if (!count) {
        ecs_vector_free(entity_column);
        columns[0].data = NULL;

        for (i = 1; i < column_last; i ++) {
            if (columns[i].size) {
                ecs_vector_free(columns[i].data);
                columns[i].data = NULL;
            }
        }

    /* This is the last entity in the table, just decrease column counts */
    } else {
        ecs_vector_remove_last(entity_column);

        for (i = 1; i < column_last; i ++) {
            if (columns[i].size) {
                ecs_vector_remove_last(columns[i].data);
            }
        }
    }
    
    if (!world->in_progress && !count) {
        activate_table(world, table, 0, false);
    }
}

uint32_t ecs_table_grow(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    uint32_t count,
    ecs_entity_t first_entity)
{
    uint32_t column_count = ecs_vector_count(table->type);

    /* Fist add entity to column with entity ids */
    ecs_entity_t *e = ecs_vector_addn(&columns[0].data, &handle_arr_params, count);
    if (!e) {
        return -1;
    }

    uint32_t i;
    for (i = 0; i < count; i ++) {
        e[i] = first_entity + i;
    }

    /* Add elements to each column array */
    for (i = 1; i < column_count + 1; i ++) {
        ecs_vector_params_t params = {.element_size = columns[i].size};
        if (!ecs_vector_addn(&columns[i].data, &params, count)) {
            return -1;
        }
    }

    uint32_t row_count = ecs_vector_count(columns[0].data);
    if (!world->in_progress && row_count == count) {
        activate_table(world, table, 0, true);
    }

    /* Return index of first added entity */
    return row_count - count + 1;
}

int16_t ecs_table_dim(
    ecs_table_t *table,
    uint32_t count)
{
    ecs_table_column_t *columns = table->columns;
    uint32_t column_count = ecs_vector_count(table->type);

    if (!ecs_vector_set_size(&columns[0].data, &handle_arr_params, count)) {
        return -1;
    }

    uint32_t i;
    for (i = 1; i < column_count + 1; i ++) {
        ecs_vector_params_t params = {.element_size = columns[i].size};
        if (!ecs_vector_set_size(&columns[i].data, &params, count)) {
            return -1;
        }
    }

    return 0;
}

uint64_t ecs_table_count(
    ecs_table_t *table)
{
    return ecs_vector_count(table->columns[0].data);
}

uint32_t ecs_table_row_size(
    ecs_table_t *table)
{
    uint32_t i, count = ecs_vector_count(table->type);
    uint32_t size = 0;

    for (i = 0; i < count; i ++) {
        size += table->columns[i].size;
    }

    return size;
}

uint32_t ecs_table_rows_dimensioned(
    ecs_table_t *table)
{
    return ecs_vector_size(table->columns[0].data);
}
