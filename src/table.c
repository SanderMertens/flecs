#include "include/private/reflecs.h"

EcsResult ecs_table_init(
    EcsWorld *world,
    EcsTable *table)
{
    EcsArray *family = ecs_map_get_ptr(world->family_index, table->family_id);
    if (!family) {
        return EcsError;
    }

    EcsIter it = ecs_array_iter(family, &handle_arr_params);
    uint32_t column = 0;
    uint32_t total_size = 0;

    table->family = family;
    table->columns = malloc(sizeof(uint32_t) * (ecs_array_count(family) + 1));

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

    table->row_params.element_size = total_size + sizeof(EcsHandle);
    table->rows = ecs_array_new(&table->row_params, ECS_TABLE_INITIAL_ROW_COUNT);

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

void *ecs_table_get(
    EcsTable *table,
    uint32_t index)
{
    return ecs_array_get(table->rows, &table->row_params, index);
}
