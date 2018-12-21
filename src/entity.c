#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "include/private/reflecs.h"

static
void copy_row(
    EcsTable *new_table,
    EcsArray *new_rows,
    uint32_t new_index,
    EcsTable *old_table,
    EcsArray *old_rows,
    uint32_t old_index)
{
    EcsArray *old_family = old_table->family;
    void *old_row = ecs_table_get(old_table, old_rows, old_index);
    EcsArray *new_family = new_table->family;
    void *new_row = ecs_table_get(new_table, new_rows, new_index);
    uint16_t *new_offsets = new_table->columns;
    uint16_t *old_offsets = old_table->columns;

    assert(old_row != NULL);
    assert(new_row != NULL);

    uint32_t new_offset = sizeof(EcsHandle);
    uint32_t old_offset = sizeof(EcsHandle);
    uint32_t bytes_to_copy = 0;
    uint32_t count_new = ecs_array_count(new_family);
    int i_new = 0, i_old = 0;

    EcsHandle *old_ptr = ecs_array_get(old_family, &handle_arr_params, i_old);

    for (; i_new < count_new; ) {
        EcsHandle new = *(EcsHandle*)ecs_array_get(
            new_family, &handle_arr_params, i_new);

        EcsHandle old = 0;

        if (old_ptr) {
            old = *old_ptr;
        }

        if (new == old) {
            bytes_to_copy += new_offsets[i_new];
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
                    new_offset += new_offsets[i_new];
                    i_new ++;
                } else if (old < new) {
                    old_offset += old_offsets[i_old];
                    i_old ++;
                    old_ptr = ecs_array_get(
                       old_family, &handle_arr_params, i_old);
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
bool has_type(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle type,
    bool match_all)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    EcsFamily family_id = row.family_id;

    if (world->in_progress) {
        row = ecs_to_row(ecs_map_get64(stage->entity_stage, entity));
        EcsFamily to_remove = ecs_map_get64(stage->remove_merge, entity);
        family_id = ecs_family_merge(
            world, stage, family_id, row.family_id, to_remove);
    }

    if (!family_id) {
        return false;
    }

    EcsFamily type_family = ecs_family_from_handle(world, stage, type, NULL);
    return ecs_family_contains(
        world, stage, family_id, type_family, match_all, true);
}

static
int32_t get_offset_for_component(
    EcsTable *table,
    EcsHandle component)
{
    uint32_t offset = 0;
    uint16_t column = 0;
    EcsArray *family = table->family;
    EcsHandle *buffer = ecs_array_buffer(family);
    uint32_t count = ecs_array_count(family);

    for (column = 0; column < count; column ++) {
        if (component == buffer[column]) {
            break;
        }
        offset += table->columns[column];
    }

    if (column == count) {
        return -1;
    } else {
        return offset;
    }
}

static
void* get_row_ptr(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table,
    EcsArray *rows,
    uint32_t index,
    EcsHandle component,
    EcsFamily family_id)
{
    void *row = ecs_table_get(table, rows, index);
    assert(row != NULL);
    uint32_t offset = get_offset_for_component(table, component);

    if (offset != -1) {
        return ECS_OFFSET(row, offset + sizeof(EcsHandle));
    } else {
        return NULL;
    }
}

static
void* get_ptr(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component,
    bool staged_only,
    EcsEntityInfo *info)
{
    uint64_t row_64;
    EcsFamily family_id = 0, staged_id = 0;
    void *ptr = NULL;
    EcsStage *stage = ecs_get_stage(&world);

    if (world->in_progress) {
        row_64 = ecs_map_get64(stage->entity_stage, entity);
        if (row_64) {
            EcsRow row = ecs_to_row(row_64);
            staged_id = row.family_id;
            EcsArray *rows = ecs_map_get(stage->data_stage, staged_id);
            EcsTable *table = ecs_world_get_table(world, stage, staged_id);
            info->entity = entity;
            info->family_id = row.family_id;
            info->index = row.index;
            info->table = table;
            info->rows = rows;
            ptr = get_row_ptr(
                world, stage, table, rows, row.index, component, staged_id);
        }
    }

    if (ptr) return ptr;

    EcsHandle prefab = 0;

    if (!world->in_progress || !staged_only) {
        row_64 = ecs_map_get64(world->entity_index, entity);
        if (row_64) {
            EcsRow row = ecs_to_row(row_64);
            family_id = row.family_id;
            EcsTable *table = ecs_world_get_table(world, stage, family_id);
            info->entity = entity;
            info->family_id = row.family_id;
            info->index = row.index;
            info->table = table;
            info->rows = table->rows;

            ptr = get_row_ptr(
                world, stage, table, table->rows, row.index, component, family_id);
        }

        if (ptr) return ptr;

        if (family_id) {
            prefab = ecs_map_get64(world->prefab_index, family_id);
        }
    }

    if (!prefab && staged_id) {
        prefab = ecs_map_get64(world->prefab_index, staged_id);
    }

    if (prefab) {
        return get_ptr(world, prefab, component, staged_only, info);
    } else {
        return NULL;
    }
}

/** Copy default values from base (and base of base) prefabs */
static
void copy_from_prefab(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table,
    EcsHandle entity,
    uint32_t index,
    EcsFamily family_id,
    EcsFamily to_add)
{
    EcsHandle prefab;
    EcsFamily entity_family = family_id;

    if (world->in_progress) {
        uint64_t row64 = ecs_map_get64(world->entity_index, entity);
        if (row64) {
            EcsRow row = ecs_to_row(row64);
            entity_family = row.family_id;
        }
    }

    while ((prefab = ecs_map_get64(world->prefab_index, entity_family))) {
        EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, prefab));
        EcsTable *prefab_table = ecs_world_get_table(
            world, stage, row.family_id);

        EcsArray *add_family = ecs_family_get(world, stage, to_add);
        EcsHandle *add_handles = ecs_array_buffer(add_family);
        uint32_t i, add_count = ecs_array_count(add_family);

        for (i = 0; i < add_count; i ++) {
            EcsHandle component = add_handles[i];
            void *prefab_ptr = get_row_ptr(
                world, stage, prefab_table, prefab_table->rows,
                row.index, component, row.family_id);

            if (prefab_ptr) {
                EcsArray *rows;
                if (world->in_progress) {
                    rows = ecs_map_get(stage->data_stage, family_id);
                } else {
                    rows = table->rows;
                }

                void *ptr = get_row_ptr(
                    world, stage, table, rows, index, component, family_id);
                if (ptr) {
                    EcsComponent *cdata = ecs_get_ptr(
                        world, component, EcsComponent_h);
                    assert(cdata != NULL);
                    memcpy(ptr, prefab_ptr, cdata->size);
                }
            }
        }

        entity_family = row.family_id;
    }
}

static
bool notify_pre_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table,
    EcsArray *rows,
    uint32_t row,
    EcsFamily to_init,
    EcsMap *systems)
{
    if (world->is_merging) {
        return false;
    }

    bool in_progress = world->in_progress;
    world->in_progress = true;

    bool result = ecs_notify(
        world, stage, systems, to_init, table, rows, row);

    world->in_progress = in_progress;
    if (result && !in_progress) {
        ecs_merge(world);
    }

    return result;
}

static
bool notify_post_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table,
    EcsArray *rows,
    uint32_t row,
    EcsFamily to_deinit)
{
    if (world->in_progress) {
        return false;
    }

    return ecs_notify(
      world, stage, world->remove_systems, to_deinit, table, rows, row);
}

/** Commit an entity with a specified family to memory */
static
uint32_t commit_w_family(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle entity,
    uint64_t old_row_64,
    EcsFamily family_id,
    EcsFamily to_add,
    EcsFamily to_remove)
{
    EcsTable *new_table, *old_table;
    EcsArray *new_rows, *old_rows;
    EcsMap *entity_index;
    EcsRow new_row, old_row;
    EcsFamily old_family_id = 0;
    uint32_t new_index = -1, old_index;
    bool in_progress = world->in_progress;

    if (in_progress) {
        entity_index = stage->entity_stage;
    } else {
        entity_index = world->entity_index;
    }

    if (family_id) {
        new_table = ecs_world_get_table(world, stage, family_id);
    }

    if (old_row_64) {
        old_row = ecs_to_row(old_row_64);
        old_family_id = old_row.family_id;
        if (old_family_id == family_id) {
            return old_row.index;
        }

        old_table = ecs_world_get_table(world, stage, old_row.family_id);
        old_index = old_row.index;
        if (in_progress) {
            old_rows = ecs_map_get(stage->data_stage, old_family_id);
        } else {
            old_rows = old_table->rows;
        }
    }

    if (family_id) {
        if (in_progress) {
            EcsArray *rows = ecs_map_get(stage->data_stage, family_id);
            new_rows = rows;
            if (!new_rows) {
                new_rows = ecs_array_new(&new_table->row_params, 1);
            }

            new_index = ecs_table_insert(world, new_table, &new_rows, entity);
            assert(new_index != -1);

            if (new_rows != rows) {
                ecs_map_set(stage->data_stage, family_id, new_rows);
            }
        } else {
            new_index = ecs_table_insert(
                world, new_table, &new_table->rows, entity);
            new_rows = new_table->rows;
        }
    }

    if (old_family_id) {
        if (family_id) {
            copy_row(
                new_table, new_rows, new_index, old_table, old_rows, old_index);
        }
        if (to_remove) {
            notify_post_merge(world, stage, old_table, old_rows, old_index, to_remove);
        }
        ecs_table_delete(world, old_table, old_index);
    }

    if (family_id) {
        new_row = (EcsRow){.family_id = family_id, .index = new_index};
        uint64_t row_64 = ecs_from_row(new_row);
        ecs_map_set64(entity_index, entity, row_64);
        if (to_add) {
            notify_pre_merge
              (world, stage, new_table, new_rows, new_index, to_add, world->add_systems);
            copy_from_prefab(
                world, stage, new_table, entity, new_index, family_id, to_add);
        }
    } else {
        if (in_progress) {
            ecs_map_set64(entity_index, entity, 0);
        } else {
            ecs_map_remove(entity_index, entity);
        }
    }

    world->valid_schedule = false;

    return new_index;
}

/* -- Private functions -- */

bool ecs_notify(
    EcsWorld *world,
    EcsStage *stage,
    EcsMap *systems,
    EcsFamily family_id,
    EcsTable *table,
    EcsArray *rows,
    int32_t row_index)
{
    EcsHandle system = ecs_map_get64(systems, family_id);
    bool notified = false;

    if (system) {
        EcsRowSystem *system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
        assert(system_data != NULL);

        if (!system_data->base.enabled) {
            return false;
        }

        EcsArray *family = ecs_family_get(world, stage, family_id);
        uint32_t i, column_count = ecs_array_count(family);
        EcsHandle *buffer = ecs_array_buffer(system_data->components);
        int32_t columns[column_count];

        for (i = 0; i < column_count; i ++) {
            columns[i] = ecs_table_column_offset(table, buffer[i]);
        }

        int32_t row = row_index;
        if (row_index == -1) {
            row = ecs_array_count(rows) - 1;
            row_index = 0;
        }

        for (; row >= row_index; row --) {
            ecs_row_notify(
                world,
                stage,
                system,
                system_data,
                rows,
                &table->row_params,
                row,
                columns);
        }

        notified = true;
    } else {
        EcsArray *family = ecs_family_get(world, stage, family_id);
        uint32_t i, count = ecs_array_count(family);
        EcsHandle *buffer = ecs_array_buffer(family);
        int32_t offset = sizeof(EcsHandle);

        for (i = 0; i < count; i ++) {
            EcsFamily fid = ecs_family_from_handle(
                world, stage, buffer[i], NULL);

            system = ecs_map_get64(systems, fid);
            if (system) {
                EcsRowSystem *system_data = ecs_get_ptr(
                    world, system, EcsRowSystem_h);

                assert(system_data != NULL);

                int32_t row = row_index;
                if (row_index == -1) {
                    row = ecs_array_count(rows) - 1;
                    row_index = 0;
                }

                for (; row >= row_index; row --) {
                    ecs_row_notify(
                        world,
                        stage,
                        system,
                        system_data,
                        rows,
                        &table->row_params,
                        row,
                        &offset);
                }

                notified = true;
            }
            offset += table->columns[i];
        }
    }

    return notified;
}

EcsHandle ecs_new_w_family(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    EcsHandle entity = ++ world->last_handle;
    commit_w_family(world, stage, entity, 0, family_id, family_id, 0);
    return entity;
}

void ecs_merge_entity(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle entity,
    EcsRow *staged_row)
{
    uint64_t old_row_64 = ecs_map_get64(world->entity_index, entity);
    EcsRow old_row = ecs_to_row(old_row_64);
    EcsFamily to_remove = ecs_map_get64(stage->remove_merge, entity);

    EcsFamily staged_id = staged_row->family_id;
    EcsFamily family_id = ecs_family_merge(
        world, stage, old_row.family_id, staged_row->family_id, to_remove);

    uint32_t new_index = commit_w_family(
        world, stage, entity, old_row_64, family_id, 0, to_remove);

    if (family_id && staged_id) {
        EcsTable *new_table = ecs_world_get_table(world, stage, family_id);
        assert(new_table != NULL);

        EcsTable *staged_table = ecs_world_get_table(world, stage, staged_id);
        EcsArray *staged_rows = ecs_map_get(
            stage->data_stage, staged_row->family_id);

        copy_row(
            new_table,
            new_table->rows,
            new_index,
            staged_table,
            staged_rows,
            staged_row->index);
    }
}

/* -- Public functions -- */

EcsResult ecs_commit(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsStage *stage = ecs_get_stage(&world);

    EcsMap *entity_index;
    if (world->in_progress) {
        entity_index = stage->entity_stage;
    } else {
        entity_index = world->entity_index;
    }

    EcsFamily to_add = ecs_map_get64(stage->add_stage, entity);
    EcsFamily to_remove = ecs_map_get64(stage->remove_stage, entity);
    uint64_t row_64 = ecs_map_get64(entity_index, entity);
    EcsRow row = ecs_to_row(row_64);

    EcsFamily family_id = ecs_family_merge(
        world, stage, row.family_id, to_add, to_remove);

    if (to_add) {
        ecs_map_remove(stage->add_stage, entity);
    }

    if (to_remove) {
        ecs_map_remove(stage->remove_stage, entity);
        if (world->in_progress) {
            EcsFamily remove_merge = ecs_map_get64(
                stage->remove_merge, entity);
            remove_merge = ecs_family_merge(
                world, stage, remove_merge, to_remove, 0);
            ecs_map_set64(stage->remove_merge, entity, remove_merge);
        }
    }

    return commit_w_family(
        world, stage, entity, row_64, family_id, to_add, to_remove);
}

EcsHandle ecs_new(
    EcsWorld *world,
    EcsHandle type)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsHandle entity = ++ world->last_handle;
    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, stage, type, NULL);
        commit_w_family(world, stage, entity, 0, family_id, family_id, 0);
    }

    return entity;
}

EcsHandle ecs_clone(
    EcsWorld *world,
    EcsHandle entity,
    bool copy_value)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsHandle result = ++ world->last_handle;
    if (entity) {
        int64_t row64 = ecs_map_get64(world->entity_index, entity);
        if (row64) {
            EcsRow row = ecs_to_row(row64);
            EcsFamily family_id = row.family_id;
            commit_w_family(world, stage, result, 0, family_id, family_id, 0);

            if (copy_value) {
                EcsTable *from_table = ecs_world_get_table(world, stage, family_id);
                EcsArray *from_rows = from_table->rows;
                EcsTable *to_table;
                EcsArray *to_rows;
                EcsRow to_row;

                if (world->in_progress) {
                    to_table = ecs_world_get_table(world, stage, family_id);
                    to_rows = ecs_map_get(stage->data_stage, family_id);
                    to_row = ecs_to_row(ecs_map_get64(stage->entity_stage, result));
                } else {
                    to_table = from_table;
                    to_rows = from_rows;
                    to_row = ecs_to_row(ecs_map_get64(world->entity_index, result));
                }

                copy_row(to_table, to_rows, to_row.index,
                    from_table, from_rows, row.index);
            }
        }
    }

    return result;
}

EcsHandle ecs_new_w_count(
    EcsWorld *world,
    EcsHandle type,
    uint32_t count,
    EcsHandle *handles_out)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsHandle result = world->last_handle + 1;
    world->last_handle += count;

    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, stage, type, NULL);
        EcsTable *table = ecs_world_get_table(world, stage, family_id);
        uint32_t row_count = ecs_array_count(table->rows);
        row_count += count;
        ecs_array_set_size(&table->rows, &table->row_params, row_count);

        int i;
        for (i = result; i < (result + count); i ++) {
            commit_w_family(world, stage, i, 0, family_id, family_id, 0);
            if (handles_out) {
                handles_out[i - result] = i;
            }
        }
    }

    return result;
}

void ecs_delete(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsStage *stage = ecs_get_stage(&world);
    bool in_progress = world->in_progress;

    if (!in_progress) {
        uint64_t row64;
        if (ecs_map_has(world->entity_index, entity, &row64)) {
            EcsRow row = ecs_to_row(row64);
            commit_w_family(world, stage, entity, row64, 0, 0, row.family_id);
            ecs_map_remove(world->entity_index, entity);
        }
    } else {
        EcsHandle *h = ecs_array_add(&stage->delete_stage, &handle_arr_params);
        *h = entity;
    }
}

void* ecs_get_ptr(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    EcsEntityInfo info;
    return get_ptr(world, entity, component, false, &info);
}

EcsHandle ecs_set_ptr(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component,
    void *src)
{
    EcsEntityInfo info = {0}, cinfo = {0};
    assert(src != NULL);
    assert(world != NULL);
    assert(component != 0);

    if (!entity) {
        entity = ecs_new(world, component);
    }

    int *dst = get_ptr(world, entity, component, true, &info);
    if (!dst) {
        ecs_add(world, entity, component);
        ecs_commit(world, entity);

        dst = get_ptr(world, entity, component, true, &info);
        assert(dst != NULL);
    }

    EcsComponent *c = get_ptr(world, component, EcsComponent_h, false, &cinfo);
    assert(c != NULL);
    memcpy(dst, src, c->size);

    EcsStage *stage = ecs_get_stage(&world);
    EcsFamily to_set = ecs_family_from_handle(world, stage, component, &cinfo);
    notify_pre_merge(
        world,
        stage,
        info.table,
        info.rows,
        info.index,
        to_set,
        world->set_systems);

    return entity;
}

bool ecs_has(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle type)
{
    return has_type(world, entity, type, true);
}

bool ecs_has_any(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle type)
{
    return has_type(world, entity, type, false);
}

EcsHandle ecs_new_component(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsHandle result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    result = ecs_new_w_family(world, NULL, world->component_family);

    EcsComponent *component_data = ecs_get_ptr(world, result, EcsComponent_h);
    if (!component_data) {
        return 0;
    }

    EcsId *id_data = ecs_get_ptr(world, result, EcsId_h);
    if (!id_data) {
        return 0;
    }

    *id_data = id;
    component_data->size = size;

    return result;
}

const char* ecs_id(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsId *id = ecs_get_ptr(world, entity, EcsId_h);
    if (id) {
        return *id;
    } else {
        return NULL;
    }
}

bool ecs_empty(
    EcsWorld *world,
    EcsHandle entity)
{
    uint64_t row64 = ecs_map_get64(world->entity_index, entity);
    return row64 != 0;
}
