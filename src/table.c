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

EcsResult ecs_table_init(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table)
{
    bool prefab_set = false;
    EcsArray *family = ecs_family_get(world, stage, table->family_id);
    ecs_assert(family != NULL, ECS_INTERNAL_ERROR, "invalid family id in table");
    
    table->frame_systems = NULL;
    table->family = family;
    table->columns = calloc(sizeof(EcsTableColumn), ecs_array_count(family) + 1);
    EcsEntity *buf = ecs_array_buffer(family);
    uint32_t i, count = ecs_array_count(family);

    /* First column is reserved for storing entity id's */
    table->columns[0].size = sizeof(EcsEntity);
    table->columns[0].data = ecs_array_new(&handle_arr_params, ECS_TABLE_INITIAL_ROW_COUNT);

    for (i = 0; i < count; i ++) {
        EcsComponent *component = ecs_get_ptr(world, buf[i], EcsComponent_h);

        if (component) {
            if (component->size) {
                /* Regular column data */
                EcsArrayParams param = {.element_size = component->size};
                table->columns[i + 1].size = component->size;
                table->columns[i + 1].data = ecs_array_new(&param, ECS_TABLE_INITIAL_ROW_COUNT);
            }
        } else {
            if (ecs_has(world, buf[i], EcsPrefab_h)) {
                /* Tables can contain at most one prefab */
                ecs_assert(prefab_set == false, ECS_MORE_THAN_ONE_PREFAB, ecs_id(world, buf[i]));
                prefab_set = true;

                /* Register family with prefab index for quick lookups */
                ecs_map_set(world->prefab_index, table->family_id, buf[i]);

            } else if (!ecs_has(world, buf[i], EcsContainer_h)) {
                ecs_assert(0, ECS_INVALID_HANDLE, NULL);
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
        world->remove_systems, table->family_id, table, table->rows, -1);*/
}

void ecs_table_free(
    EcsWorld *world,
    EcsTable *table)
{
    /*ecs_array_free(table->rows);
    if (table->frame_systems) ecs_array_free(table->frame_systems);
    free(table->columns);*/
}

uint32_t ecs_table_insert(
    EcsWorld *world,
    EcsTable *table,
    EcsTableColumn *columns,
    EcsEntity entity)
{
    uint32_t column_count = ecs_array_count(table->family);

    /* Fist add entity to column with entity ids */
    EcsEntity *e = ecs_array_add(&columns[0].data, &handle_arr_params);
    if (!e) {
        return -1;
    }

    *e = entity;

    /* Add elements to each column array */
    uint32_t i;
    for (i = 1; i < column_count + 1; i ++) {
        EcsArrayParams params = {.element_size = columns[i].size};
        if (!ecs_array_add(&columns[i].data, &params)) {
            return -1;
        }
    }

    uint32_t count = ecs_array_count(columns[0].data);
    if (!world->in_progress && count == 1) {
        activate_table(world, table, true);
    }

    /* Return index of last added entity */
    return ecs_array_count(columns[0].data) - 1;
}

void ecs_table_delete(
    EcsWorld *world,
    EcsTable *table,
    uint32_t index)
{
    EcsTableColumn *columns = table->columns;
    uint32_t column_count = ecs_array_count(table->family);
    uint32_t count = ecs_array_count(columns[0].data);
    if (index >= count) {
        return;
    }

    /* Last entity in array is about to be moved, need handle to update entity_index */
    EcsEntity e = *(EcsEntity*)ecs_array_get(
        columns[0].data, &handle_arr_params, count - 1);

    ecs_array_remove_index(columns[0].data, &handle_arr_params, index);        

    int32_t i;
    for (i = 1; i < column_count + 1; i ++) {
        EcsArrayParams params = {.element_size = columns[i].size};
        ecs_array_remove_index(columns[i].data, &params, index);
    }

    if (index != (count - 1)) {
        /* Last entity in table is now moved to index of removed entity */
        EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, e));
        row.index = index;
        ecs_map_set64(world->entity_index, e, ecs_from_row(row));
    }

    if (!world->in_progress && !(count - 1)) {
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
    uint32_t column_count = ecs_array_count(table->family);

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
    if (!world->in_progress && row_count == 1) {
        activate_table(world, table, true);
    }

    /* Return index of last added entity */
    return ecs_array_count(columns[0].data) - 1;
}

int16_t ecs_table_dim(
    EcsTable *table,
    uint32_t count)
{
    EcsTableColumn *columns = table->columns;
    uint32_t column_count = ecs_array_count(table->family);

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
    uint32_t i, count = ecs_array_count(table->family);
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