#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "include/private/reflecs.h"

static
void copy_column(
    EcsTableColumn *new_column,
    uint32_t new_index,
    EcsTableColumn *old_column,
    uint32_t old_index)
{
    EcsArrayParams param = {.element_size = new_column->size};
    void *dst = ecs_array_get(new_column->data, &param, new_index);
    void *src = ecs_array_get(old_column->data, &param, old_index);
    memcpy(dst, src, param.element_size);
}

static
void copy_row(
    EcsArray *new_family,
    EcsTableColumn *new_columns,
    uint32_t new_index,
    EcsArray *old_family,
    EcsTableColumn *old_columns,
    uint32_t old_index)
{
    uint16_t i_new, new_component_count = ecs_array_count(new_family);
    uint16_t i_old = 0, old_component_count = ecs_array_count(old_family);
    EcsEntity *new_components = ecs_array_buffer(new_family);
    EcsEntity *old_components = ecs_array_buffer(old_family);

    for (i_new = 0; i_new < new_component_count; ) {
        if (i_old == old_component_count) {
            break;
        }

        EcsEntity new_component = new_components[i_new];
        EcsEntity old_component = old_components[i_old];

        if (new_component == old_component) {
            copy_column(&new_columns[i_new + 1], new_index, &old_columns[i_old + 1], old_index);
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            i_new ++;
        } else if (new_component > old_component) {
            i_old ++;
        }
    }
}

static
bool has_type(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity type,
    bool match_all)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    EcsFamily family_id = row.family_id;
    bool match_prefab = type != EcsPrefab_h && type != EcsId_h;

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
        world, stage, family_id, type_family, match_all, match_prefab);
}

static
void* get_row_ptr(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *family,
    EcsTableColumn *columns,
    uint32_t index,
    EcsEntity component)
{
    uint32_t column_index = ecs_family_index_of(family, component);
    if (column_index == -1) {
        return NULL;
    }

    EcsTableColumn *column = &columns[column_index + 1];
    EcsArrayParams param = {.element_size = column->size};

    return ecs_array_get(column->data, &param, index);
}

static
void* get_ptr(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity component,
    bool staged_only,
    bool search_prefab,
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
            EcsTableColumn *columns = ecs_map_get(stage->data_stage, staged_id);
            EcsTable *table = ecs_world_get_table(world, stage, staged_id);
            info->entity = entity;
            info->family_id = row.family_id;
            info->index = row.index;
            info->table = table;
            info->columns = columns;
            ptr = get_row_ptr(
                world, stage, table->family, columns, row.index, component);
        }
    }

    if (ptr) return ptr;

    EcsEntity prefab = 0;

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
            info->columns = table->columns;
            ptr = get_row_ptr(
                world, stage, table->family, table->columns, row.index, component);
        }

        if (ptr) return ptr;

        if (family_id && search_prefab) {
            prefab = ecs_map_get64(world->prefab_index, family_id);
        }
    }

    if (!prefab && staged_id && search_prefab) {
        prefab = ecs_map_get64(world->prefab_index, staged_id);
    }

    if (prefab) {
        return get_ptr(world, prefab, component, staged_only, true, info);
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
    EcsEntity entity,
    uint32_t index,
    EcsFamily family_id,
    EcsFamily to_add)
{
    EcsEntity prefab;
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
        EcsEntity *add_handles = ecs_array_buffer(add_family);
        uint32_t i, add_count = ecs_array_count(add_family);

        for (i = 0; i < add_count; i ++) {
            EcsEntity component = add_handles[i];
            void *prefab_ptr = get_row_ptr(
                world, stage, prefab_table->family, prefab_table->columns,
                row.index, component);

            if (prefab_ptr) {
                EcsTableColumn *columns;
                if (world->in_progress) {
                    columns = ecs_map_get(stage->data_stage, family_id);
                } else {
                    columns = table->columns;
                }

                void *ptr = get_row_ptr(
                    world, stage, table->family, columns, index, component);
                if (ptr) {
                    EcsComponent *component_data = ecs_get_ptr(
                        world, component, EcsComponent_h);
                    assert(component_data != NULL);
                    memcpy(ptr, prefab_ptr, component_data->size);
                }
            }
        }

        /* Recursively search through prefabs */
        entity_family = row.family_id;
    }
}

static
bool notify_pre_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table,
    EcsTableColumn *table_columns,
    uint32_t offset,
    uint32_t limit,
    EcsFamily to_init,
    EcsMap *systems)
{
    if (world->is_merging) {
        return false;
    }

    bool in_progress = world->in_progress;
    world->in_progress = true;

    bool result = ecs_notify(
        world, stage, systems, to_init, table, table_columns, offset, limit);

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
    EcsTableColumn *table_columns,
    uint32_t offset,
    uint32_t limit,
    EcsFamily to_deinit)
{
    if (world->in_progress) {
        return false;
    }

    return ecs_notify(
        world, stage, world->family_sys_remove_index, to_deinit, table, 
        table_columns, offset, limit);
}

/** Commit an entity with a specified family to memory */
static
uint32_t commit_w_family(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity entity,
    uint64_t old_row_64,
    EcsFamily family_id,
    EcsFamily to_add,
    EcsFamily to_remove,
    bool notify_on_add)
{
    EcsTable *new_table, *old_table;
    EcsTableColumn *new_columns, *old_columns;
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
            old_columns = ecs_map_get(stage->data_stage, old_family_id);
        } else {
            old_columns = old_table->columns;
        }
    }

    if (family_id) {
        if (in_progress) {
            EcsTableColumn *columns = ecs_map_get(stage->data_stage, family_id);
            new_columns = columns;
            if (!new_columns) {
                new_columns = calloc(sizeof(EcsTableColumn), 1);
            }

            new_index = ecs_table_insert(world, new_table, new_columns, entity);
            assert(new_index != -1);

            if (new_columns != columns) {
                ecs_map_set(stage->data_stage, family_id, new_columns);
            }
        } else {
            new_index = ecs_table_insert(
                world, new_table, new_table->columns, entity);

            new_columns = new_table->columns;
        }
    }

    if (old_family_id) {
        if (family_id) {
            copy_row(new_table->family, new_columns, new_index, 
                old_table->family, old_columns, old_index);
        }
        if (to_remove) {
            notify_post_merge(
                world, stage, old_table, old_columns, old_index, 1, to_remove);
        }
        ecs_table_delete(world, old_table, old_index);
    }

    if (family_id) {
        new_row = (EcsRow){.family_id = family_id, .index = new_index};
        uint64_t row_64 = ecs_from_row(new_row);
        ecs_map_set64(entity_index, entity, row_64);

        if (to_add && notify_on_add) {
            notify_pre_merge (
              world, stage, new_table, new_columns, new_index, 1, to_add, 
              world->family_sys_add_index);

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

bool ecs_notify_system(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity system,
    EcsFamily family_id,
    EcsTable *table,
    EcsTableColumn *table_columns,
    int32_t offset,
    int32_t limit)  
{
    EcsRowSystem *system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return false;
    }

    EcsArray *family = ecs_family_get(world, stage, system_data->base.and_from_entity);
    uint32_t i, column_count = ecs_array_count(family);
    EcsEntity *buffer = ecs_array_buffer(system_data->components);
    int32_t columns[column_count];

    for (i = 0; i < column_count; i ++) {
        columns[i] = ecs_family_index_of(table->family, buffer[i]) + 1;
    }

    ecs_row_notify(
        world,
        system,
        system_data,
        columns,
        table_columns,
        offset,
        limit);

    return true;
}

bool ecs_notify(
    EcsWorld *world,
    EcsStage *stage,
    EcsMap *index,
    EcsFamily family_id,
    EcsTable *table,
    EcsTableColumn *table_columns,
    int32_t offset,
    int32_t limit)
{
    EcsArray *systems = ecs_map_get(index, family_id);
    bool notified = false;

    if (systems) {
        EcsEntity *buffer = ecs_array_buffer(systems);
        uint32_t i, count = ecs_array_count(systems);

        for (i = 0; i < count; i ++) {
            notified |= ecs_notify_system(
                world, stage, buffer[i], family_id, table, table_columns, 
                offset, limit);
        }
    } 

    return notified;
}

EcsEntity ecs_new_w_family(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    EcsEntity entity = ++ world->last_handle;
    commit_w_family(world, stage, entity, 0, family_id, family_id, 0, true);
    return entity;
}

void ecs_merge_entity(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity entity,
    EcsRow *staged_row)
{
    uint64_t old_row_64 = ecs_map_get64(world->entity_index, entity);
    EcsRow old_row = ecs_to_row(old_row_64);
    EcsFamily to_remove = ecs_map_get64(stage->remove_merge, entity);

    EcsFamily staged_id = staged_row->family_id;
    EcsFamily family_id = ecs_family_merge(
        world, stage, old_row.family_id, staged_row->family_id, to_remove);

    uint32_t new_index = commit_w_family(
        world, stage, entity, old_row_64, family_id, 0, to_remove, true);

    if (family_id && staged_id) {
        EcsTable *new_table = ecs_world_get_table(world, stage, family_id);
        assert(new_table != NULL);

        EcsTable *staged_table = ecs_world_get_table(world, stage, staged_id);
        EcsTableColumn *staged_columns = ecs_map_get(
            stage->data_stage, staged_row->family_id);

        copy_row( new_table->family, new_table->columns, new_index,
                  staged_table->family, staged_columns, staged_row->index); 
    }
}

/* -- Public functions -- */

EcsResult ecs_commit(
    EcsWorld *world,
    EcsEntity entity)
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
        world, stage, entity, row_64, family_id, to_add, to_remove, true);
}

EcsEntity ecs_new(
    EcsWorld *world,
    EcsEntity type)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsEntity entity = ++ world->last_handle;
    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, stage, type, NULL);
        commit_w_family(world, stage, entity, 0, family_id, family_id, 0, true);
    }

    return entity;
}

EcsEntity ecs_clone(
    EcsWorld *world,
    EcsEntity entity,
    bool copy_value)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsEntity result = ++ world->last_handle;
    if (entity) {
        int64_t row64 = ecs_map_get64(world->entity_index, entity);
        if (row64) {
            EcsRow row = ecs_to_row(row64);
            EcsFamily family_id = row.family_id;
            commit_w_family(world, stage, result, 0, family_id, family_id, 0, true);

            if (copy_value) {
                EcsTable *from_table = ecs_world_get_table(world, stage, family_id);
                EcsTableColumn *to_columns = NULL, *from_columns = from_table->columns;
                EcsTable *to_table = ecs_world_get_table(world, stage, family_id);
                EcsRow to_row = {0};

                if (world->in_progress) {
                    to_columns = ecs_map_get(stage->data_stage, family_id);
                    to_row = ecs_to_row(ecs_map_get64(stage->entity_stage, result));
                } else {
                    to_columns = to_table->columns;
                    to_row = ecs_to_row(ecs_map_get64(world->entity_index, result));
                }

                if (!to_table)
                    to_table = from_table;

                if (!to_columns)
                    to_columns = from_columns;

                if (!to_row.index)
                    to_row = ecs_to_row(ecs_map_get64(
                            world->entity_index, result));

                copy_row(to_table->family, to_columns, to_row.index,
                    from_table->family, from_columns, row.index);

                /* A clone with value is equivalent to a set */
                ecs_notify(
                    world, stage, world->family_sys_set_index, from_table->family_id, 
                    to_table, to_columns, to_row.index, 1);
            }
        }
    }

    return result;
}

EcsEntity ecs_new_w_count(
    EcsWorld *world,
    EcsEntity type,
    uint32_t count,
    EcsEntity *handles_out)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsEntity result = world->last_handle + 1;
    world->last_handle += count;

    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, stage, type, NULL);
        EcsTable *table = ecs_world_get_table(world, stage, family_id);
        ecs_table_grow(world, table, table->columns, count, result);

        int i;
        for (i = result; i < (result + count); i ++) {
            /* We need to commit each entity individually in order to populate
             * the entity index. Unfortunately this cannot be done in bulk, but
             * fortunately this is a reasonably fast operation. 
             * 
             * Note how notify_on_add is set to false, which will cause any
             * OnAdd row systems not to be executed as a result of the commit.
             * That is something we *can* do more efficiently in bulk.
             */
            commit_w_family(world, stage, i, 0, family_id, family_id, 0, false);

            if (handles_out) {
                handles_out[i - result] = i;
            }
        }

        /* Now we can notify matching OnAdd row systems in bulk */
        notify_pre_merge(
            world, stage, table, table->columns, result, count, 
            family_id, world->family_sys_add_index);
    }

    return result;
}

void ecs_delete(
    EcsWorld *world,
    EcsEntity entity)
{
    EcsStage *stage = ecs_get_stage(&world);
    bool in_progress = world->in_progress;

    if (!in_progress) {
        uint64_t row64;
        if (ecs_map_has(world->entity_index, entity, &row64)) {
            EcsRow row = ecs_to_row(row64);
            commit_w_family(world, stage, entity, row64, 0, 0, row.family_id, true);
            ecs_map_remove(world->entity_index, entity);
        }
    } else {
        EcsEntity *h = ecs_array_add(&stage->delete_stage, &handle_arr_params);
        *h = entity;
    }
}

void* ecs_get_ptr(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity component)
{
    EcsEntityInfo info;
    return get_ptr(world, entity, component, false, true, &info);
}

EcsEntity ecs_set_ptr(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity component,
    void *src)
{
    EcsEntityInfo info = {0}, cinfo = {0};
    assert(src != NULL);
    assert(world != NULL);
    assert(component != 0);

    if (!entity) {
        entity = ecs_new(world, component);
    }

    int *dst = get_ptr(world, entity, component, true, false, &info);
    if (!dst) {
        ecs_stage_add(world, entity, component);
        ecs_commit(world, entity);

        dst = get_ptr(world, entity, component, true, false, &info);
        assert(dst != NULL);
    }

    EcsComponent *c = get_ptr(world, component, EcsComponent_h, false, false, &cinfo);
    assert(c != NULL);
    memcpy(dst, src, c->size);

    EcsStage *stage = ecs_get_stage(&world);
    EcsFamily to_set = ecs_family_from_handle(world, stage, component, &cinfo);

    notify_pre_merge(
        world,
        stage,
        info.table,
        info.columns,
        info.index,
        1,
        to_set,
        world->family_sys_set_index);

    return entity;
}

bool ecs_has(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity type)
{
    return has_type(world, entity, type, true);
}

bool ecs_has_any(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity type)
{
    return has_type(world, entity, type, false);
}

EcsEntity ecs_new_component(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsEntity result = ecs_lookup(world, id);
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
    EcsEntity entity)
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
    EcsEntity entity)
{
    uint64_t row64 = ecs_map_get64(world->entity_index, entity);
    return row64 != 0;
}

EcsEntity ecs_get_component(
    EcsWorld *world,
    EcsEntity entity,
    uint32_t index)
{
    EcsStage *stage = ecs_get_stage(&world);
    int64_t row64;

    if (world->in_progress) {
        row64 = ecs_map_get64(stage->entity_stage, entity);
    } else {
        row64 = ecs_map_get64(world->entity_index, entity);
    }

    if (!row64) {
        return 0;
    }

    EcsArray *components;
    EcsRow row = ecs_to_row(row64);
    if (world->in_progress) {
        components = ecs_map_get(world->family_index, row.family_id);
    } else {
        components = ecs_map_get(stage->family_stage, row.family_id);
    }

    EcsEntity *buffer = ecs_array_buffer(components);

    if (ecs_array_count(components) > index) {
        return buffer[index];
    } else {
        return 0;
    }
}
