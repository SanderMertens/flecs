#include <assert.h>
#include "include/private/reflecs.h"

/** Notify systems that a table has changed its active state */
static
void activate_table(
    EcsWorld *world,
    EcsTable *table,
    bool activate)
{
    EcsArray *systems = table->frame_systems;
    if (systems) {
        EcsIter it = ecs_array_iter(systems, &handle_arr_params);
        while (ecs_iter_hasnext(&it)) {
            EcsEntity system = *(EcsEntity*)ecs_iter_next(&it);
            ecs_system_activate_table(world, system, table, activate);
        }
    }
}

/* -- Private functions -- */

EcsTableColumn *ecs_table_get_columns(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *type)
{
    EcsTableColumn *result = calloc(sizeof(EcsTableColumn), ecs_array_count(type) + 1);
    EcsEntity *buf = ecs_array_buffer(type);
    uint32_t i, count = ecs_array_count(type);

    /* First column is reserved for storing entity id's */
    result[0].size = sizeof(EcsEntity);
    result[0].data = NULL;

    for (i = 0; i < count; i ++) {
        EcsComponent *component = ecs_get_ptr(world, buf[i], EcsComponent);

        if (component) {
            if (component->size) {
                /* Regular column data */
                result[i + 1].size = component->size;
            }
        }
    }

    return result;
}

EcsResult ecs_table_init(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table)
{
    EcsArray *type = ecs_type_get(world, stage, table->type_id);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, "invalid type id of table");
    bool prefab_set = false;

    table->frame_systems = NULL;
    table->type = type;
    table->columns = ecs_table_get_columns(world, stage, type);

    if (stage == &world->main_stage) {
        EcsEntity *buf = ecs_array_buffer(type);
        uint32_t i, count = ecs_array_count(type);

        for (i = 0; i < count; i ++) {
            /* Only if creating columns in the main stage, register prefab */
            if (!ecs_has(world, buf[i], EcsComponent)) {
                if (ecs_has(world, buf[i], EcsPrefab)) {
                    /* Tables can contain at most one prefab */
                    ecs_assert(prefab_set == false, ECS_MORE_THAN_ONE_PREFAB, ecs_id(world, buf[i]));
                    prefab_set = true;

                    /* Register type with prefab index for quick lookups */
                    ecs_map_set(world->prefab_index, table->type_id, buf[i]);

                } else if (!ecs_has(world, buf[i], EcsContainer)) {
                    ecs_assert(0, ECS_INVALID_HANDLE, NULL);
                }
            }
        }
    }

    return EcsOk;
}

void ecs_table_deinit(
    EcsWorld *world,
    EcsTable *table)
{
    /*ecs_notify(world, NULL,
        world->remove_systems, table->type_id, table, table->rows, -1);*/
}

void ecs_table_free(
    EcsWorld *world,
    EcsTable *table)
{
    uint32_t i, column_count = ecs_array_count(table->type);
    
    for (i = 0; i < column_count + 1; i ++) {
        ecs_array_free(table->columns[i].data);
    }

    free(table->columns);

    ecs_array_free(table->frame_systems);
}

uint32_t ecs_table_insert(
    EcsWorld *world,
    EcsTable *table,
    EcsTableColumn *columns,
    EcsEntity entity)
{
    uint32_t column_count = ecs_array_count(table->type);

    /* Fist add entity to column with entity ids */
    EcsEntity *e = ecs_array_add(&columns[0].data, &handle_arr_params);
    if (!e) {
        return -1;
    }

    *e = entity;

    /* Add elements to each column array */
    uint32_t i;
    for (i = 1; i < column_count + 1; i ++) {
        uint32_t size = columns[i].size;
        if (size) {
            EcsArrayParams params = {.element_size = size};
            if (!ecs_array_add(&columns[i].data, &params)) {
                return -1;
            }
        }
    }

    uint32_t index = ecs_array_count(columns[0].data) - 1;

    if (!world->in_progress && !index) {
        activate_table(world, table, true);
    }

    /* Return index of last added entity */
    return index;
}

void ecs_table_delete(
    EcsWorld *world,
    EcsTable *table,
    uint32_t index)
{
    EcsTableColumn *columns = table->columns;
    EcsArray *entity_column = columns[0].data;
    uint32_t count = ecs_array_count(entity_column);

    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    count --;
    
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    if (index != count) {
        /* Move last entity in array to index */
        EcsEntity *entities = ecs_array_buffer(entity_column);
        EcsEntity to_move = entities[count];
        entities[index] = to_move;

        uint32_t column_last = ecs_array_count(table->type) + 1;
        uint32_t i;
        for (i = 1; i < column_last; i ++) {
            EcsTableColumn *column = &columns[i];
            EcsArrayParams params = {.element_size = column->size};
            ecs_array_remove_index(column->data, &params, index);
        }

        /* Last entity in table is now moved to index of removed entity */
        EcsRow row;
        row.type_id = table->type_id;
        row.index = index;
        ecs_map_set64(world->main_stage.entity_index, to_move, ecs_from_row(row));
    }

    ecs_array_remove_last(entity_column);

    if (!world->in_progress && !count) {
        activate_table(world, table, false);
    }
}

uint32_t ecs_table_grow(
    EcsWorld *world,
    EcsTable *table,
    EcsTableColumn *columns,
    uint32_t count,
    EcsEntity first_entity)
{
    uint32_t column_count = ecs_array_count(table->type);

    /* Fist add entity to column with entity ids */
    EcsEntity *e = ecs_array_addn(&columns[0].data, &handle_arr_params, count);
    if (!e) {
        return -1;
    }

    uint32_t i;
    for (i = 0; i < count; i ++) {
        e[i] = first_entity + i;
    }

    /* Add elements to each column array */
    for (i = 1; i < column_count + 1; i ++) {
        EcsArrayParams params = {.element_size = columns[i].size};
        if (!ecs_array_addn(&columns[i].data, &params, count)) {
            return -1;
        }
    }

    uint32_t row_count = ecs_array_count(columns[0].data);
    if (!world->in_progress && row_count == count) {
        activate_table(world, table, true);
    }

    /* Return index of first added entity */
    return row_count - count;
}

int16_t ecs_table_dim(
    EcsTable *table,
    uint32_t count)
{
    EcsTableColumn *columns = table->columns;
    uint32_t column_count = ecs_array_count(table->type);

    if (!ecs_array_set_size(&columns[0].data, &handle_arr_params, count)) {
        return -1;
    }

    uint32_t i;
    for (i = 1; i < column_count + 1; i ++) {
        EcsArrayParams params = {.element_size = columns[i].size};
        if (!ecs_array_set_size(&columns[i].data, &params, count)) {
            return -1;
        }
    }

    return 0;
}

uint64_t ecs_table_count(
    EcsTable *table)
{
    return ecs_array_count(table->columns[0].data);
}

uint32_t ecs_table_row_size(
    EcsTable *table)
{
    uint32_t i, count = ecs_array_count(table->type);
    uint32_t size = 0;

    for (i = 0; i < count; i ++) {
        size += table->columns[i].size;
    }

    return size;
}

uint32_t ecs_table_rows_dimensioned(
    EcsTable *table)
{
    return ecs_array_size(table->columns[0].data);
}