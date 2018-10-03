#include "include/private/reflecs.h"

/** Callback that is invoked when a row is moved in the table->rows array */
static
void move_row(
    EcsArray *array,
    const EcsArrayParams *params,
    void *to,
    void *from,
    void *ctx)
{
    EcsTable *table = ctx;
    EcsWorld *world = table->world;
    uint32_t new_index = ecs_array_get_index(array, params, to);
    EcsHandle handle = *(EcsHandle*)to;
    EcsRow row = {.family_id = table->family_id, .index = new_index};
    ecs_map_set64(world->entity_index, handle, ecs_from_row(row));
}

/* -- Private functions -- */

EcsResult ecs_table_init_w_size(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *family,
    uint32_t size)
{
    table->world = world;
    table->family = family;

    table->row_params.element_size = size + sizeof(EcsHandle);
    table->row_params.compare_action = NULL;
    table->row_params.move_action = move_row;
    table->row_params.move_ctx = table;

    table->rows = ecs_array_new(
        &table->row_params, ECS_TABLE_INITIAL_ROW_COUNT);

    return EcsOk;
}

EcsResult ecs_table_init(
    EcsWorld *world,
    EcsTable *table)
{
    EcsArray *family = ecs_map_get(world->family_index, table->family_id);
    if (!family) {
        return EcsError;
    }

    EcsIter it = ecs_array_iter(family, &handle_arr_params);
    uint32_t column = 0;
    uint32_t total_size = 0;

    table->columns = malloc(sizeof(uint16_t) * ecs_array_count(family));

    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        EcsComponent *type = ecs_get(world, h, world->component);
        if (!type) {
            return EcsError;
        }

        table->columns[column] = type->size;
        total_size += type->size;
        column ++;
    }

    ecs_table_init_w_size(world, table, family, total_size);

    return EcsOk;
}

uint32_t ecs_table_insert(
    EcsTable *table,
    EcsHandle handle)
{
    void *row = ecs_array_add(&table->rows, &table->row_params);
    *(EcsHandle*)row = handle;
    return ecs_array_count(table->rows) - 1;
}

void* ecs_table_get(
    EcsTable *table,
    uint32_t index)
{
    return ecs_array_get(table->rows, &table->row_params, index);
}

uint32_t ecs_table_column_offset(
    EcsTable *table,
    EcsHandle component)
{
    EcsIter it = ecs_array_iter(table->family, &handle_arr_params);
    uint32_t i = 0, offset = 0;

    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        if (h == component) {
            return offset;
        }
        offset += table->columns[i];
        i ++;
    }

    return -1;
}

bool ecs_table_has_components(
    EcsTable *table,
    EcsArray *components)
{
    EcsIter it = ecs_array_iter(components, &handle_arr_params);

    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        if (ecs_table_column_offset(table, h) == -1) {
            return false;
        }
    }

    return true;
}
