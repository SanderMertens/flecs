#include "include/private/reflecs.h"
#include <string.h>

static
void ecs_move_row(
    EcsTable *new_table,
    EcsArray *new_family,
    void *new_row,
    EcsTable *old_table,
    EcsArray *old_family,
    void *old_row)
{
    int i_new = 0, i_old = 0;

    uint32_t new_offset = sizeof(EcsHandle);
    uint32_t old_offset = sizeof(EcsHandle);
    uint32_t bytes_to_copy = 0;
    uint32_t count_new = ecs_array_count(new_family);

    for (; i_new < count_new; ) {
        EcsHandle new = *(EcsHandle*)ecs_array_get(
            new_family, &handle_arr_params, i_new);

        EcsHandle old = 0;
        EcsHandle *old_ptr = ecs_array_get(
            old_family, &handle_arr_params, i_old);
        if (old_ptr) {
            old = *old_ptr;
        }

        if (new == old) {
            bytes_to_copy += new_table->columns[i_new];
            i_new ++;
            i_old ++;
        } else {
            if (bytes_to_copy) {
                void *dst = ECS_OFFSET(new_row, new_offset);
                void *src = ECS_OFFSET(old_row, old_offset);
                memcpy(dst, src, bytes_to_copy);
                new_offset += bytes_to_copy;
                old_offset += bytes_to_copy;
                bytes_to_copy = 0;
            }

            if (old) {
                if (new < old) {
                    i_new ++;
                    new_offset += new_table->columns[i_new];
                } else if (old < new) {
                    i_old ++;
                    old_offset += old_table->columns[i_old];
                }
            }
        }

        if (!old || !old_ptr) {
            break;
        }
    }

    if (bytes_to_copy) {
        void *dst = ECS_OFFSET(new_row, new_offset);
        void *src = ECS_OFFSET(old_row, old_offset);
        memcpy(dst, src, bytes_to_copy);
    }
}

static
EcsResult ecs_commit_w_family(
    EcsWorld *world,
    EcsHandle entity,
    EcsFamily family_id)
{
    EcsTable *new_table = ecs_world_get_table(world, family_id);
    if (!new_table) {
        return EcsError;
    }

    uint32_t new_index = ecs_table_insert(new_table, entity);
    EcsRow old_row = ecs_to_row(ecs_map_get64(world->entity_index, entity));

    if (old_row.family_id) {
        EcsTable *old_table = ecs_world_get_table(world, old_row.family_id);
        EcsArray *old_family = old_table->family;
        void *old_row_ptr = ecs_table_get(old_table, old_row.index);
        EcsArray *new_family = new_table->family;
        void *new_row_ptr = ecs_table_get(new_table, new_index);

        ecs_move_row(
            new_table,
            new_family,
            new_row_ptr,
            old_table,
            old_family,
            old_row_ptr);

        ecs_array_remove(old_table->rows, &old_table->row_params, old_row_ptr);
    }

    EcsRow row = {.family_id = family_id, .index = new_index};
    ecs_map_set64(world->entity_index, entity, ecs_from_row(row));

    return EcsOk;
}

EcsResult ecs_commit(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsFamily family_id = (uintptr_t)ecs_map_get64(world->staging_index, entity);
    return ecs_commit_w_family(world, entity, family_id);
}

EcsHandle ecs_new(
    EcsWorld *world)
{
    return ecs_world_new_handle(world);
}

EcsHandle ecs_new_w_family(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsHandle entity = ecs_world_new_handle(world);
    ecs_commit_w_family(world, entity, family_id);
    return entity;
}

void ecs_delete(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    if (row.family_id) {
        EcsTable *table = ecs_world_get_table(world, row.family_id);
        ecs_array_remove_index(table->rows, &table->row_params, row.index);
        ecs_map_remove(world->entity_index, entity);
    }
}

EcsResult ecs_stage(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    EcsFamily family_id = (uintptr_t)ecs_map_get64(world->staging_index, entity);
    EcsArray *family = NULL;
    if (!family_id) {
        EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
        family_id = row.family_id;
    }

    if (family_id) {
        family = ecs_map_get(world->family_index, family_id);
        if (!family) {
            return EcsError;
        }
    }

    EcsFamily new_family_id =
        ecs_world_register_family(world, component, family);

    if (!new_family_id) {
        return EcsError;
    }

    if (family_id != new_family_id) {
        ecs_map_set64(world->staging_index, entity, new_family_id);
    }

    return EcsOk;
}

void* ecs_get(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    if (!row.family_id) {
        return NULL;
    }

    EcsTable *table = ecs_world_get_table(world, row.family_id);
    if (!table) {
        return NULL;
    }

    void *row_ptr;
    uint32_t offset = 0;
    uint16_t column = 0;
    bool found = false;

    EcsIter it = ecs_array_iter(table->family, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle handle = *(EcsHandle*)ecs_iter_next(&it);
        if (component == handle) {
            found = true;
            break;
        }
        offset += table->columns[column];
        column ++;
    }

    if (!found) {
        return NULL;
    }

    row_ptr = ecs_table_get(table, row.index);

    return ECS_OFFSET(row_ptr, offset + sizeof(EcsHandle));
}

void* ecs_add(
    EcsWorld *world,
    EcsHandle h_entity,
    EcsHandle h_component)
{
    ecs_stage(world, h_entity, h_component);

    if (ecs_commit(world, h_entity) != EcsOk) {
        return NULL;
    }

    return ecs_get(world, h_entity, h_component);
}

EcsHandle ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsHandle result = ecs_new(world);

    if (ecs_stage(world, result, world->component) != EcsOk) {
        return 0;
    }

    if (ecs_stage(world, result, world->id) != EcsOk) {
        return 0;
    }

    if (ecs_commit(world, result) != EcsOk) {
        return 0;
    }

    EcsComponent *component_data = ecs_get(world, result, world->component);
    if (!component_data) {
        return 0;
    }

    EcsId *id_data = ecs_get(world, result, world->id);
    if (!id_data) {
        return 0;
    }

    id_data->id = id;
    component_data->size = size;

    return result;
}
