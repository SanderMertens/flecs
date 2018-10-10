#include "include/private/reflecs.h"
#include <string.h>

/** Move row from one table to another table */
static
void move_row(
    EcsWorld *world,
    EcsTable *new_table,
    uint32_t new_index,
    EcsTable *old_table,
    uint32_t old_index)
{
    int i_new = 0, i_old = 0;
    EcsArray *old_family = old_table->family;
    void *old_row = ecs_table_get(old_table, old_index);
    EcsArray *new_family = new_table->family;
    void *new_row = ecs_table_get(new_table, new_index);

    uint32_t new_offset = sizeof(EcsHandle);
    uint32_t old_offset = sizeof(EcsHandle);
    uint32_t bytes_to_copy = 0;
    uint32_t count_new = ecs_array_count(new_family);

    EcsHandle *old_ptr = ecs_array_get(
       old_family, &handle_arr_params, i_old);

    for (; i_new < count_new; ) {
        EcsHandle new = *(EcsHandle*)ecs_array_get(
            new_family, &handle_arr_params, i_new);

        EcsHandle old = 0;

        if (old_ptr) {
            old = *old_ptr;
        }

        if (new == old) {
            bytes_to_copy += new_table->columns[i_new];
            i_new ++;
            i_old ++;
            old_ptr = ecs_array_get(
               old_family, &handle_arr_params, i_old);
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
                    old_ptr = ecs_array_get(
                       old_family, &handle_arr_params, i_old);

                    if (old_ptr) {
                        old_offset += old_table->columns[i_old];
                    }
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

    ecs_table_delete(old_table, old_index);
}

static
void notify(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *systems,
    uint32_t row,
    EcsFamily family)
{
    uint32_t i, count = ecs_array_count(systems);
    uint32_t table_index = ecs_array_get_index(
        world->table_db, &table_arr_params, table);

    for (i = 0; i < count; i ++) {
        EcsHandle h = *(EcsHandle*)
            ecs_array_get(systems, &handle_arr_params, i);
        EcsSystem *system_data = ecs_get(world, h, world->system);

        if (ecs_family_contains(world, system_data->family_id, family)) {
            ecs_system_notify(world, h, system_data, table, table_index, row);
        }
    }
}

static
void init_components(
    EcsWorld *world,
    EcsTable *table,
    uint32_t row,
    EcsFamily to_init)
{
    notify(world, table, table->init_systems, row, to_init);
}

static
void deinit_components(
    EcsWorld *world,
    EcsTable *table,
    uint32_t row,
    EcsFamily to_deinit)
{
    notify(world, table, table->deinit_systems, row, to_deinit);
}

/** Stage components for adding or removing from an entity */
static
EcsResult ecs_stage(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component,
    bool is_remove)
{
    EcsFamily family_id = 0;
    EcsArray *family = NULL;

    if (is_remove) {
        family_id = ecs_map_get64(world->remove_stage, entity);
    } else {
        family_id = ecs_map_get64(world->add_stage, entity);
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
        abort();
    }

    if (family_id != new_family_id) {
        if (is_remove) {
            ecs_map_set64(world->remove_stage, entity, new_family_id);
        } else {
            ecs_map_set64(world->add_stage, entity, new_family_id);
        }
    }

    return EcsOk;
}

/** Commit an entity with a specified family to memory */
static
EcsResult commit_w_family(
    EcsWorld *world,
    EcsHandle entity,
    EcsFamily family_id,
    EcsFamily to_add,
    EcsFamily to_remove)
{
    EcsTable *new_table = ecs_world_get_table(world, family_id);
    if (!new_table) {
        abort();
    }

    uint32_t new_index = ecs_table_insert(new_table, entity);
    uint64_t row_64 = ecs_map_get64(world->entity_index, entity);

    if (row_64) {
        EcsRow old_row = ecs_to_row(row_64);
        EcsTable *old_table = ecs_world_get_table(world, old_row.family_id);
        move_row(world, new_table, new_index, old_table, old_row.index);
        if (to_remove) {
            deinit_components(world, old_table, old_row.index, to_remove);
        }
    }

    if (to_add) {
        init_components(world, new_table, new_index, to_add);
    }

    EcsRow row = {.family_id = family_id, .index = new_index};
    row_64 = ecs_from_row(row);
    ecs_map_set64(world->entity_index, entity, row_64);

    world->valid_schedule = false;

    return EcsOk;
}


/* -- Public functions -- */

EcsResult ecs_commit(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsFamily to_add = ecs_map_get64(world->add_stage, entity);
    EcsFamily to_remove = ecs_map_get64(world->remove_stage, entity);
    EcsFamily family_id = 0;

    uint64_t row_64 = ecs_map_get64(world->entity_index, entity);
    if (row_64) {
        EcsRow row = ecs_to_row(row_64);
        family_id = row.family_id;
    }

    family_id = ecs_family_merge(world, family_id, to_add, to_remove);

    EcsResult result = commit_w_family(
        world, entity, family_id, to_add, to_remove);

    if (to_add) {
        ecs_map_remove(world->add_stage, entity);
    }

    if (to_remove) {
        ecs_map_remove(world->remove_stage, entity);
    }

    return result;
}

EcsHandle ecs_new(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsHandle entity = ++ world->last_handle;
    if (family_id) {
        commit_w_family(world, entity, family_id, family_id, 0);
    }

    return entity;
}

void ecs_delete(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    if (row.family_id) {
        EcsTable *table = ecs_world_get_table(world, row.family_id);
        ecs_table_delete(table, row.index);
        ecs_map_remove(world->entity_index, entity);
        world->valid_schedule = false;
    }
}

EcsResult ecs_add(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    return ecs_stage(world, entity, component, false);
}

EcsResult ecs_remove(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    return ecs_stage(world, entity, component, true);
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

EcsHandle ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsHandle result = ecs_new(world, 0);

    if (ecs_add(world, result, world->component) != EcsOk) {
        return 0;
    }

    if (ecs_add(world, result, world->id) != EcsOk) {
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
