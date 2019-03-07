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
    EcsArray *new_type,
    EcsTableColumn *new_columns,
    uint32_t new_index,
    EcsArray *old_type,
    EcsTableColumn *old_columns,
    uint32_t old_index)
{
    uint16_t i_new, new_component_count = ecs_array_count(new_type);
    uint16_t i_old = 0, old_component_count = ecs_array_count(old_type);
    EcsEntity *new_components = ecs_array_buffer(new_type);
    EcsEntity *old_components = ecs_array_buffer(old_type);

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
void* get_row_ptr(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *type,
    EcsTableColumn *columns,
    uint32_t index,
    EcsEntity component)
{
    uint32_t column_index = ecs_type_index_of(type, component);
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
    EcsType type_id = 0, staged_id = 0;
    void *ptr = NULL;
    EcsStage *stage = ecs_get_stage(&world);

    if (world->in_progress) {
        row_64 = ecs_map_get64(stage->entity_index, entity);
        if (row_64) {
            EcsRow row = ecs_to_row(row_64);
            staged_id = row.type_id;
            EcsTableColumn *columns = ecs_map_get(stage->data_stage, staged_id);
            EcsTable *table = ecs_world_get_table(world, stage, staged_id);
            info->entity = entity;
            info->type_id = row.type_id;
            info->index = row.index;
            info->table = table;
            info->columns = columns;
            ptr = get_row_ptr(
                world, stage, table->type, columns, row.index, component);
        }
    }

    if (ptr) return ptr;

    EcsEntity prefab = 0;

    if (!world->in_progress || !staged_only) {
        row_64 = ecs_map_get64(world->main_stage.entity_index, entity);
        if (row_64) {
            EcsRow row = ecs_to_row(row_64);
            type_id = row.type_id;
            EcsTable *table = ecs_world_get_table(world, stage, type_id);
            info->entity = entity;
            info->type_id = row.type_id;
            info->index = row.index;
            info->table = table;
            info->columns = table->columns;
            ptr = get_row_ptr(
                world, stage, table->type, table->columns, row.index, component);
        }

        if (ptr) return ptr;

        if (type_id && search_prefab) {
            prefab = ecs_map_get64(world->prefab_index, type_id);
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
    EcsType type_id,
    EcsType to_add)
{
    EcsEntity prefab;
    EcsType entity_type = type_id;

    if (world->in_progress) {
        uint64_t row64 = ecs_map_get64(stage->entity_index, entity);
        if (row64) {
            EcsRow row = ecs_to_row(row64);
            entity_type = row.type_id;
        }
    }

    while ((prefab = ecs_map_get64(world->prefab_index, entity_type))) {
        /* Prefabs are only resolved from the main stage. Prefabs created while
         * iterating cannot be resolved in the same iteration. */
        EcsRow row = ecs_to_row(ecs_map_get64(world->main_stage.entity_index, prefab));

        EcsTable *prefab_table = ecs_world_get_table(
            world, stage, row.type_id);

        EcsArray *add_type = ecs_type_get(world, stage, to_add);
        EcsEntity *add_handles = ecs_array_buffer(add_type);
        uint32_t i, add_count = ecs_array_count(add_type);

        for (i = 0; i < add_count; i ++) {
            EcsEntity component = add_handles[i];
            void *prefab_ptr = get_row_ptr(
                world, stage, prefab_table->type, prefab_table->columns,
                row.index, component);

            if (prefab_ptr) {
                EcsTableColumn *columns;
                if (world->in_progress) {
                    columns = ecs_map_get(stage->data_stage, type_id);
                } else {
                    columns = table->columns;
                }

                void *ptr = get_row_ptr(
                    world, stage, table->type, columns, index, component);
                if (ptr) {
                    EcsComponent *component_data = _ecs_get_ptr(
                        world, component, tEcsComponent);
                    assert(component_data != NULL);
                    memcpy(ptr, prefab_ptr, component_data->size);
                }
            }
        }

        /* Recursively search through prefabs */
        entity_type = row.type_id;
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
    EcsType to_init,
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
    EcsType to_deinit)
{
    if (world->in_progress) {
        return false;
    }

    return ecs_notify(
        world, stage, world->type_sys_remove_index, to_deinit, table, 
        table_columns, offset, limit);
}

/** Commit an entity with a specified type to memory */
static
uint32_t commit_w_type(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntityInfo *info,
    EcsType type_id,
    EcsType to_add,
    EcsType to_remove)
{
    EcsTable *old_table, *new_table;
    EcsTableColumn *new_columns, *old_columns;
    EcsMap *entity_index;
    EcsType old_type_id = 0;
    uint32_t new_index = -1, old_index = -1;
    bool in_progress = world->in_progress;
    EcsEntity entity = info->entity;
    EcsArray *old_type = NULL;

    entity_index = stage->entity_index;

    if ((old_table = info->table)) {
        old_type_id = info->type_id;
        if (old_type_id == type_id) {
            return info->index;
        }

        old_index = info->index;
        if (in_progress) {
            old_columns = ecs_map_get(stage->data_stage, old_type_id);
        } else {
            old_columns = old_table->columns;
        }

        old_type = old_table->type;
    }

    if (type_id) {
        EcsArray *old_table_db = stage->tables;
        new_table = ecs_world_get_table(world, stage, type_id);
        if (old_table && old_table_db != stage->tables) {
            old_table = ecs_world_get_table(world, stage, info->type_id);
        }

        if (in_progress) {
            EcsTableColumn *columns = ecs_map_get(stage->data_stage, type_id);
            new_columns = columns;
            if (!new_columns) {
                new_columns = calloc(sizeof(EcsTableColumn), 1);
            }

            new_index = ecs_table_insert(world, new_table, new_columns, entity);
            assert(new_index != -1);

            if (new_columns != columns) {
                ecs_map_set(stage->data_stage, type_id, new_columns);
            }
        } else {
            new_index = ecs_table_insert(
                world, new_table, new_table->columns, entity);

            new_columns = new_table->columns;
        }
    }

    if (old_type_id) {
        if (type_id) {
            copy_row(new_table->type, new_columns, new_index, 
                old_type, old_columns, old_index);
        }
    }

    if (to_remove) {
        ecs_assert(old_index != -1, ECS_INTERNAL_ERROR, NULL);
        notify_post_merge(
            world, stage, old_table, old_columns, old_index, 1, to_remove);
    }

    if (old_type_id) {
        ecs_table_delete(world, old_table, old_index);
    }

    if (type_id) {
        EcsRow new_row = (EcsRow){.type_id = type_id, .index = new_index};
        ecs_map_set64(entity_index, entity, ecs_from_row(new_row));

        if (to_add) {
            notify_pre_merge (
                world, stage, new_table, new_columns, new_index, 1, to_add, 
                world->type_sys_add_index);

            copy_from_prefab(
                world, stage, new_table, entity, new_index, type_id, to_add);
        }     
    } else {
        if (world->in_progress) {
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
    EcsType type_id,
    EcsTable *table,
    EcsTableColumn *table_columns,
    int32_t offset,
    int32_t limit)  
{
    EcsRowSystem *system_data = _ecs_get_ptr(world, system, tEcsRowSystem);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return false;
    }

    EcsArray *type = ecs_type_get(world, stage, system_data->base.and_from_entity);
    uint32_t i, column_count = ecs_array_count(type);
    EcsEntity *buffer = ecs_array_buffer(system_data->components);
    int32_t columns[column_count];

    for (i = 0; i < column_count; i ++) {
        columns[i] = ecs_type_index_of(table->type, buffer[i]) + 1;
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
    EcsType type_id,
    EcsTable *table,
    EcsTableColumn *table_columns,
    int32_t offset,
    int32_t limit)
{
    EcsArray *systems = ecs_map_get(index, type_id);
    bool notified = false;

    if (systems) {
        EcsEntity *buffer = ecs_array_buffer(systems);
        uint32_t i, count = ecs_array_count(systems);

        for (i = 0; i < count; i ++) {
            notified |= ecs_notify_system(
                world, stage, buffer[i], type_id, table, table_columns, 
                offset, limit);
        }
    } 

    return notified;
}

void ecs_merge_entity(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity entity,
    EcsRow *staged_row)
{
    uint64_t old_row_64 = ecs_map_get64(world->main_stage.entity_index, entity);

    EcsRow old_row = ecs_to_row(old_row_64);
    EcsType to_remove = ecs_map_get64(stage->remove_merge, entity);
    EcsType staged_id = staged_row->type_id;
    EcsType type_id = ecs_type_merge(
        world, stage, old_row.type_id, staged_row->type_id, to_remove);

    EcsEntityInfo info = {
        .entity = entity,
        .type_id = old_row.type_id,
        .index = old_row.index
    };

    uint32_t new_index = commit_w_type(
        world, stage, &info, type_id, 0, to_remove);

    if (type_id && staged_id) {
        EcsTable *new_table = ecs_world_get_table(world, stage, type_id);
        assert(new_table != NULL);

        EcsTable *staged_table = ecs_world_get_table(world, stage, staged_id);
        EcsTableColumn *staged_columns = ecs_map_get(
            stage->data_stage, staged_row->type_id);

        copy_row( new_table->type, new_table->columns, new_index,
                  staged_table->type, staged_columns, staged_row->index); 
    }
}

/* -- Public functions -- */

EcsEntity ecs_clone(
    EcsWorld *world,
    EcsEntity entity,
    bool copy_value)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsEntity result = ++ world->last_handle;
    if (entity) {
        int64_t row64 = ecs_map_get64(world->main_stage.entity_index, entity);
        if (row64) {
            EcsRow row = ecs_to_row(row64);
            EcsType type_id = row.type_id;
            EcsEntityInfo info = {
                .entity = result,
                .type_id = 0,
                .index = row.index
            };

            commit_w_type(world, stage, &info, type_id, type_id, 0);

            if (copy_value) {
                EcsTable *from_table = ecs_world_get_table(world, stage, type_id);
                EcsTableColumn *to_columns = NULL, *from_columns = from_table->columns;
                EcsTable *to_table = ecs_world_get_table(world, stage, type_id);
                EcsRow to_row = {0};

                if (world->in_progress) {
                    to_columns = ecs_map_get(stage->data_stage, type_id);
                } else {
                    to_columns = to_table->columns;
                }

                to_row = ecs_to_row(ecs_map_get64(stage->entity_index, result));

                if (!to_table)
                    to_table = from_table;

                if (!to_columns)
                    to_columns = from_columns;

                if (!to_row.index)
                    to_row = ecs_to_row(ecs_map_get64(
                            world->main_stage.entity_index, result));

                copy_row(to_table->type, to_columns, to_row.index,
                    from_table->type, from_columns, row.index);

                /* A clone with value is equivalent to a set */
                ecs_notify(
                    world, stage, world->type_sys_set_index, from_table->type_id, 
                    to_table, to_columns, to_row.index, 1);
            }
        }
    }

    return result;
}

EcsEntity _ecs_new(
    EcsWorld *world,
    EcsType type)
{
    EcsEntity entity = ++ world->last_handle;
    if (type) {
        EcsEntityInfo info = {
            .entity = entity
        };

        EcsStage *stage = ecs_get_stage(&world);
        commit_w_type(world, stage, &info, type, type, 0);
    }

    return entity;
}

EcsEntity _ecs_new_w_count(
    EcsWorld *world,
    EcsType type,
    uint32_t count,
    EcsEntity *handles_out)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsEntity result = world->last_handle + 1;
    world->last_handle += count;

    if (type) {
        EcsTable *table = ecs_world_get_table(world, stage, type);
        uint32_t row = ecs_table_grow(world, table, table->columns, count, result);

        EcsMap *entity_index = stage->entity_index;

        uint32_t cur_index_count = ecs_map_count(entity_index);
        ecs_map_set_size(entity_index, cur_index_count + count);

        int i;
        for (i = result; i < (result + count); i ++) {
            /* We need to commit each entity individually in order to populate
             * the entity index */

            EcsRow new_row = (EcsRow){.type_id = type, .index = row};
            ecs_map_set64(entity_index, i, ecs_from_row(new_row));

            row ++;
        }

        /* Now we can notify matching OnAdd row systems in bulk */
        notify_pre_merge(
            world, stage, table, table->columns, row - count, count, 
            type, world->type_sys_add_index);
    } 
    
    if (handles_out) {
        int i;
        for (i = 0; i < count; i ++) {
            handles_out[i] = result + i;
        }
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
        if (ecs_map_has(world->main_stage.entity_index, entity, &row64)) {
            EcsRow row = ecs_to_row(row64);
            EcsEntityInfo info = {
                .entity = entity,
                .type_id = row.type_id,
                .index = row.index,
                .table = ecs_world_get_table(world, stage, row.type_id)
            };

            commit_w_type(world, stage, &info, 0, 0, row.type_id);

            ecs_map_remove(world->main_stage.entity_index, entity);
        }
    } else {
        EcsEntity *h = ecs_array_add(&stage->delete_stage, &handle_arr_params);
        *h = entity;
    }
}

EcsResult ecs_commit(
    EcsWorld *world,
    EcsEntity entity)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsMap *entity_index = stage->entity_index;

    EcsType to_add = ecs_map_get64(stage->add_stage, entity);
    EcsType to_remove = ecs_map_get64(stage->remove_stage, entity);
    uint64_t row_64 = ecs_map_get64(entity_index, entity);
    EcsRow row = ecs_to_row(row_64);

    EcsType type_id = ecs_type_merge(
        world, stage, row.type_id, to_add, to_remove);

    if (to_add) {
        ecs_map_remove(stage->add_stage, entity);
    }

    if (to_remove) {
        ecs_map_remove(stage->remove_stage, entity);
        if (world->in_progress) {
            EcsType remove_merge = ecs_map_get64(
                stage->remove_merge, entity);
            remove_merge = ecs_type_merge(
                world, stage, remove_merge, to_remove, 0);
            ecs_map_set64(stage->remove_merge, entity, remove_merge);
        }
    }

    EcsEntityInfo info = {
        .entity = entity,
        .type_id = row.type_id,
        .index = row.index
    };

    return commit_w_type(
        world, stage, &info, type_id, to_add, to_remove);
}

EcsResult _ecs_add(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsMap *entity_index = stage->entity_index;

    EcsType dst_type = 0;

    EcsEntityInfo info = {.entity = entity};

    uint64_t row_64 = ecs_map_get64(entity_index, entity);
    if (row_64) {
        EcsRow row = ecs_to_row(row_64);
        info.table = ecs_world_get_table(world, stage, row.type_id);
        info.columns = info.table->columns;
        info.index = row.index;
        info.type_id = row.type_id;
        EcsArray *to_add = ecs_type_get(world, stage, type);
        dst_type = ecs_type_merge_arr(world, stage, info.table->type, to_add, NULL);
    } else {
        dst_type = type;
    }

    return commit_w_type(world, stage, &info, dst_type, type, 0);
}

EcsResult _ecs_remove(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type)
{
    EcsStage *stage = ecs_get_stage(&world);
    EcsMap *entity_index = stage->entity_index;

    EcsType dst_type = 0;

    EcsEntityInfo info = {.entity = entity};

    uint64_t row_64 = ecs_map_get64(entity_index, entity);
    if (row_64) {
        EcsRow row = ecs_to_row(row_64);
        info.table = ecs_world_get_table(world, stage, row.type_id);
        info.columns = info.table->columns;
        info.index = row.index;
        info.type_id = row.type_id;
        EcsArray *to_remove = ecs_type_get(world, stage, type);
        dst_type = ecs_type_merge_arr(world, stage, info.table->type, NULL, to_remove);
    } else {
        /* Nothing to remove */
        return EcsOk;
    }

    return commit_w_type(world, stage, &info, dst_type, 0, type);
}

void* _ecs_get_ptr(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type)
{
    EcsEntityInfo info;

    /* Get only accepts types that hold a single component */
    EcsEntity component = ecs_type_to_entity(world, type);

    return get_ptr(world, entity, component, false, true, &info);
}

EcsEntity _ecs_set_ptr(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type,
    size_t size,
    void *src)
{
    EcsEntityInfo info = {0};

    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETERS, NULL);

    /* Set only accepts types that hold a single component */
    EcsEntity component = ecs_type_to_entity(world, type);

    /* If no entity is specified, create one */
    if (!entity) {
        entity = _ecs_new(world, type);
    }

    /* If component hasn't been added to entity yet, add it */
    int *dst = get_ptr(world, entity, component, true, false, &info);
    if (!dst) {
        _ecs_add(world, entity, type);
        dst = get_ptr(world, entity, component, true, false, &info);
        assert(dst != NULL);
    }

#ifndef NDEBUG
    EcsEntityInfo cinfo = {0};
    EcsComponent *cdata = get_ptr(
        world, component, eEcsComponent, false, false, &cinfo);
    ecs_assert(cdata->size == size, ECS_INVALID_COMPONENT_SIZE, NULL);
#endif

    memcpy(dst, src, size);

    EcsStage *stage = ecs_get_stage(&world);
    notify_pre_merge(
        world,
        stage,
        info.table,
        info.columns,
        info.index,
        1,
        type,
        world->type_sys_set_index);

    return entity;
}

bool _ecs_has(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type)
{
    if (!type) {
        return true;
    }

    EcsStage *stage = ecs_get_stage(&world);
    EcsType entity_type = ecs_typeid(world, entity);
    return ecs_type_contains(world, stage, entity_type, type, true, false) != 0;
}

bool _ecs_has_any(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type)
{
    if (!type) {
        return true;
    }

    EcsStage *stage = ecs_get_stage(&world);
    EcsType entity_type = ecs_typeid(world, entity);
    return ecs_type_contains(world, stage, entity_type, type, false, false);
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

    result = _ecs_new(world, world->t_component);
    ecs_set(world, result, EcsComponent, {.size = size});
    ecs_set(world, result, EcsId, {id});

    return result;
}

const char* ecs_id(
    EcsWorld *world,
    EcsEntity entity)
{
    EcsId *id = ecs_get_ptr(world, entity, EcsId);
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
    uint64_t row64 = ecs_map_get64(world->main_stage.entity_index, entity);
    return row64 == 0;
}

EcsEntity ecs_get_component(
    EcsWorld *world,
    EcsEntity entity,
    uint32_t index)
{
    EcsStage *stage = ecs_get_stage(&world);
    int64_t row64 = ecs_map_get64(stage->entity_index, entity);

    if (!row64) {
        return 0;
    }

    EcsRow row = ecs_to_row(row64);
    EcsArray *components = ecs_map_get(stage->type_index, row.type_id);
    EcsEntity *buffer = ecs_array_buffer(components);

    if (ecs_array_count(components) > index) {
        return buffer[index];
    } else {
        return 0;
    }
}

EcsType ecs_entity_to_type(
    EcsWorld *world,
    EcsEntity entity)
{
    EcsStage *stage = ecs_get_stage(&world);
    return ecs_type_from_handle(world, stage, entity, NULL);
}

EcsEntity ecs_type_to_entity(
    EcsWorld *world, 
    EcsType type_id)
{
    EcsArray *type = ecs_map_get(world->main_stage.type_index, type_id);
    if (!type) {
        ecs_abort(ECS_UNKNOWN_TYPE_ID, NULL);
    }

    /* If array contains n entities, it cannot be reduced to a single entity */
    if (ecs_array_count(type) != 1) {
        ecs_abort(ECS_TYPE_NOT_AN_ENTITY, NULL);
    }

    return ((EcsEntity*)ecs_array_buffer(type))[0];
}

EcsType ecs_typeid(
    EcsWorld *world,
    EcsEntity entity)
{
    EcsStage *stage = ecs_get_stage(&world);
    int64_t row64 = ecs_map_get64(stage->entity_index, entity);

    EcsRow row = ecs_to_row(row64);
    
    return row.type_id;
}