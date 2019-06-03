
#include "flecs_private.h"

static
void copy_column(
    ecs_table_column_t *new_column,
    int32_t new_index,
    ecs_table_column_t *old_column,
    int32_t old_index)
{
    ecs_assert(new_index > 0, ECS_INTERNAL_ERROR, NULL);

    uint32_t size = new_column->size;

    if (size) {
        ecs_vector_params_t param = {.element_size = new_column->size};

        if (old_index < 0) old_index *= -1;
        
        void *dst = ecs_vector_get(new_column->data, &param, new_index - 1);
        void *src = ecs_vector_get(old_column->data, &param, old_index - 1);
            
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

        memcpy(dst, src, param.element_size);
    }
}

static
void copy_row(
    ecs_vector_t *new_type,
    ecs_table_column_t *new_columns,
    int32_t new_index,
    ecs_vector_t *old_type,
    ecs_table_column_t *old_columns,
    int32_t old_index)
{
    uint16_t i_new, new_component_count = ecs_vector_count(new_type);
    uint16_t i_old = 0, old_component_count = ecs_vector_count(old_type);
    ecs_entity_t *new_components = ecs_vector_first(new_type);
    ecs_entity_t *old_components = ecs_vector_first(old_type);

    ecs_assert(old_columns->data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_columns->data != NULL, ECS_INTERNAL_ERROR, NULL);

    for (i_new = 0; i_new < new_component_count; ) {
        if (i_old == old_component_count) {
            break;
        }

        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

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
    ecs_vector_t *type,
    ecs_table_column_t *columns,
    int32_t index,
    ecs_entity_t component)
{
    int16_t column_index = ecs_type_index_of(type, component);
    if (column_index == -1) {
        return NULL;
    }

    if (index < 0) {
        index *= -1;
    }

    ecs_table_column_t *column = &columns[column_index + 1];
    ecs_vector_params_t param = {.element_size = column->size};

    if (param.element_size) {
        ecs_assert(column->data != NULL, ECS_INTERNAL_ERROR, NULL);

        void *ptr = ecs_vector_get(column->data, &param, index - 1);
        return ptr;
    } else {
        return NULL;
    }
}

static
ecs_row_t row_from_stage(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    uint64_t row64 = ecs_map_get64(stage->entity_index, entity);
    if (row64) {
        ecs_row_t row = ecs_to_row(row64);
        return row;
    } else {
        return (ecs_row_t){0, 0};
    }
}

static
bool stage_has_entity(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_row_t *row_out)
{
    uint64_t row64;
    if (ecs_map_has(stage->entity_index, entity, &row64)) {
        if (row64) {
            *row_out = ecs_to_row(row64);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

static
bool update_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info)
{
    ecs_entity_t entity = info->entity;

    info->commit_count = stage->commit_count;
    info->stage = stage;

    ecs_row_t row;
    if (stage_has_entity(stage, entity, &row)) {
        ecs_table_t *table = ecs_world_get_table(world, stage, row.type_id);
        info->table = table;

        if (world->in_progress && stage != &world->main_stage) {
            info->columns = ecs_map_get(stage->data_stage, row.type_id);
        } else {
            info->columns = table->columns;
        }

        ecs_assert(info->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        info->index = row.index;
        info->type_id = row.type_id;

        return true;
    } else {
        return false;
    }
}

static
bool populate_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info)
{
    bool needs_update = false;

    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    if (info->stage != stage) {
        needs_update = true;
    } else {
        int diff = stage->commit_count - info->commit_count;
        ecs_type_t type = info->type_id;

        if (diff == 1) {
            if (stage->to_type == type || stage->from_type == type) {
                needs_update = true;
            }
        } else if (diff) {
            needs_update = true;
        }
    }

    if (needs_update) {
        return update_info(world, stage, info);
    } else {
        return info->table != NULL;
    }
}

static
ecs_type_t notify_pre_merge(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_table_column_t *table_columns,
    uint32_t offset,
    uint32_t limit,
    ecs_type_t to_init,
    ecs_map_t *systems)
{
    ecs_world_t *real_world = world;
    ecs_get_stage(&real_world);
    
    ecs_assert(!real_world->is_merging, ECS_INTERNAL_ERROR, 0);

    bool in_progress = real_world->in_progress;
    real_world->in_progress = true;

    ecs_type_t result = ecs_notify(
        world, stage, systems, to_init, table, table_columns, offset, limit);

    real_world->in_progress = in_progress;
    if (result && !in_progress) {
        ecs_merge(world);
    }

    return result;
}

static
bool notify_post_merge(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_table_column_t *table_columns,
    uint32_t offset,
    uint32_t limit,
    ecs_type_t to_deinit)
{
    if (world->magic != ECS_WORLD_MAGIC || world->in_progress) {
        return false;
    }

    /* This will trigger asserts when operations are invoked during a merge that
     * are not valid while merging. */
    bool is_merging = world->is_merging;
    world->is_merging = true;

    bool result = ecs_notify(
        world, stage, world->type_sys_remove_index, to_deinit, table, 
        table_columns, offset, limit);

    world->is_merging = is_merging;

    return result;
}

static
ecs_type_t instantiate_prefab(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    bool is_prefab,
    ecs_table_t *prefab_table,
    ecs_row_t prefab_row,
    uint32_t limit)
{
    ecs_type_t modified = 0;
    ecs_vector_t *prefab_type = prefab_table->type;
    ecs_table_column_t *prefab_columns = prefab_table->columns;

    EcsPrefabBuilder *builder = get_row_ptr(prefab_type, 
        prefab_columns, prefab_row.index, EEcsPrefabBuilder);

    /* If the current entity is not a prefab itself, and the prefab
        * has children, add the children to the entity. */
    if (!is_prefab) {
        if (builder && builder->ops) {
            int32_t i, count = ecs_vector_count(builder->ops);
            ecs_builder_op_t *ops = ecs_vector_first(builder->ops);

            for (i = 0; i < count; i ++) {
                ecs_builder_op_t *op = &ops[i];
                
                ecs_entity_t child = _ecs_new_w_count(
                    world, op->type, limit);

                uint32_t j;
                for (j = 0; j < limit; j ++) {
                    ecs_adopt(world, child + j, entity + j);
                    ecs_set(world, child + j, EcsId, {op->id});
                }
            }
        }

        /* Keep track of components shared from new prefabs */
        modified = ecs_type_merge(world, stage, modified, prefab_row.type_id, 0);

    /* If the current entity is also prefab, do not add children to
        * it. Instead, add children (if any) of its base to its ops */
    } else if (builder) {
        ecs_entity_info_t info = {.entity = entity};
        EcsPrefabBuilder *entity_builder = ecs_get_ptr_intern(world, 
            stage, &info, EEcsPrefabBuilder, false, false);

        if (!entity_builder) {
            ecs_add(world, entity, EcsPrefabBuilder);
            entity_builder = ecs_get_ptr(world, entity, EcsPrefabBuilder);
            entity_builder->ops = NULL;
        }

        uint32_t count = ecs_vector_count(builder->ops);
        void *new_ops = ecs_vector_addn(
            &entity_builder->ops, &builder_params, count);
        
        memcpy(new_ops, ecs_vector_first(builder->ops), 
            sizeof(ecs_builder_op_t) * count);
    }

    return modified;
}

ecs_entity_t ecs_get_prefab_from_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    bool is_new_table,
    ecs_entity_t entity,
    ecs_type_t type_id)
{
    if (is_new_table) {
        ecs_vector_t *type = ecs_type_get(world, stage, type_id);
        int32_t i, count = ecs_vector_count(type);
        ecs_entity_t *buffer = ecs_vector_first(type);

        ecs_entity_t exclude_prefab = 0;

        for (i = 0; i < count; i ++) {
            ecs_entity_t component = buffer[i];
            if (entity && component == ecs_entity(EcsPrefab)) {
                EcsPrefab *prefab = ecs_get_ptr(world, entity, EcsPrefab);
                if (prefab) {
                    exclude_prefab = prefab->parent;
                }
            } else if (buffer[i] != exclude_prefab) {
                if (ecs_has(world, buffer[i], EcsPrefab)) {
                    return buffer[i];
                }                
            }
        }

        return 0;
    } else {
        return ecs_map_get64(world->prefab_index, type_id);
    }
}

/** Copy default values from base (and base of base) prefabs */
static
ecs_type_t copy_from_prefab(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info,
    uint32_t offset,
    uint32_t limit,
    ecs_type_t to_add)
{
    ecs_entity_t prefab, entity = info->entity;
    ecs_type_t type_id = info->type_id;
    ecs_type_t entity_type = type_id;
    ecs_type_t modified = 0;
    ecs_table_column_t *columns = info->columns;
    bool is_new_table = info->table->flags & EcsTableIsStaged;

    if (world->in_progress) {
        entity_type = info->type_id;
        if (is_new_table) {
            entity_type = ecs_type_merge( world, stage, row_from_stage(
                &world->main_stage, entity).type_id, entity_type, 0);
        }
    }

    ecs_vector_t *add_type = ecs_type_get(world, stage, to_add);
    ecs_entity_t *add_handles = ecs_vector_first(add_type);
    uint32_t i, add_count = ecs_vector_count(add_type);

    bool is_prefab = false;

    while ((prefab = ecs_get_prefab_from_type(world, stage, is_new_table, entity, entity_type))) {
        /* Prefabs are only resolved from the main stage. Prefabs created while
         * iterating cannot be resolved in the same iteration. */
        ecs_row_t prefab_row = row_from_stage(&world->main_stage, prefab);

        ecs_table_t *prefab_table = ecs_world_get_table(
            world, stage, prefab_row.type_id);
        ecs_vector_t *prefab_type = prefab_table->type;
        ecs_table_column_t *prefab_columns = prefab_table->columns;

        for (i = 0; i < add_count; i ++) {
            ecs_entity_t component = add_handles[i];

            /* Keep track of whether this entity is a prefab */
            if (component == EEcsPrefab) {
                is_prefab = true;

            /* Only copy if this component is not the prefab id */                
            } else if (component != prefab) {
                void *prefab_ptr = get_row_ptr(prefab_type, prefab_columns, 
                    prefab_row.index, component);

                if (prefab_ptr) {
                    ecs_vector_t *type_arr = ecs_type_get(world, stage, type_id);
                    uint32_t column_index = ecs_type_index_of(type_arr, component);
                    uint32_t size = columns[column_index + 1].size;

                    if (size) {
                        void *buffer = ecs_vector_first(columns[column_index + 1].data);
                        void *ptr = ECS_OFFSET(buffer, offset * size);
                        uint32_t i;

                        for (i = 0; i < limit; i ++) {
                            memcpy(ptr, prefab_ptr, size);
                            ptr = ECS_OFFSET(ptr, size);
                        }
                    }
                }

            /* If component is the prefab, the prefab has just been added */
            } else {
                modified = instantiate_prefab(world, stage, entity, is_prefab, 
                    prefab_table, prefab_row, limit);
            }
        }

        /* Recursively search through prefabs */
        entity_type = prefab_row.type_id;
    }

    if (modified) {
        /* Always strip EcsPrefab, as an entity will never inherit the EcsPrefab
         * component from a prefab. Same for EcsId. */
        modified = ecs_type_merge(world, stage, modified, 0, ecs_type(EcsPrefab));
        modified = ecs_type_merge(world, stage, modified, 0, ecs_type(EcsId));
    }

    return modified;
}

static
bool notify_after_commit(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info,
    uint32_t offset,
    uint32_t limit,
    ecs_type_t to_add,
    bool do_set)
{
    ecs_type_t modified = 0;

    ecs_type_t initialized = notify_pre_merge (
        world, stage, info->table, info->columns, offset, limit, to_add, 
        world->type_sys_add_index);

    populate_info(world, stage, info);

    ecs_type_t overridden = copy_from_prefab(
        world, stage, info, offset, limit, to_add);

    populate_info(world, stage, info);

    /* Invoke OnSet handlers if components received their first value */
    if (do_set) {
        if (initialized || overridden) {
            modified = ecs_type_merge(
                world, stage, initialized, overridden, 0);

            notify_pre_merge (
                world, stage, info->table, info->columns, offset, limit, 
                modified, world->type_sys_set_index);
        }
    }

    return initialized || overridden || modified;
}

/** Commit an entity with a specified type to a table (probably the most 
 * important function in flecs). */
static
uint32_t commit_w_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info,
    ecs_type_t type_id,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    bool do_set)
{
    ecs_table_t *new_table = NULL, *old_table;
    ecs_table_column_t *new_columns = NULL, *old_columns;
    ecs_map_t *entity_index = stage->entity_index;
    ecs_type_t old_type_id = 0;
    int32_t new_index = 0, old_index = 0;
    bool in_progress = world->in_progress;
    ecs_entity_t entity = info->entity;
    ecs_vector_t *old_type = NULL;

    /* Always update remove_merge stage when in progress. It is possible (and
     * likely) that when a component is removed, it hasn't been added in the
     * same iteration. As a result, the staged entity index does not know about
     * the component to remove, and thus the staged type_id of the entity when
     * merged with the to_remove type id results in the same type id. This would
     * ordinarily cause the commit action to be skipped, but when in progress,
     * we need to keep track of the remove so that it is removed from the main
     * stage once the merge takes place. */
    if (in_progress) {
        /* Update remove type. Add to_remove, and subtract to_add. */
        uint64_t *rm_type_ptr = ecs_map_get_ptr(stage->remove_merge, entity);
        ecs_type_t remove_type = rm_type_ptr ? *rm_type_ptr : 0;

        /* If remove_type and to_remove are 0, the result of the merge is going
         * to be 0 as well, so don't bother overwriting the remove_index */
        if (remove_type || to_remove) {
            remove_type = ecs_type_merge(
                world, stage, remove_type, to_remove, to_add);

            if (rm_type_ptr) {
                *rm_type_ptr = remove_type;
            } else if (remove_type) {
                ecs_map_set64(stage->remove_merge, entity, remove_type);
            } else {
                /* If the result is 0, remove entity from remove stage which
                 * reduces the work required during the merge */
                ecs_map_remove(stage->remove_merge, entity);
            }
        }
    }

    if ((old_table = info->table)) {
        old_type_id = info->type_id;

        /* If the type id is the same as that of the table where the entity is
         * currently stored, nothing needs to be committed. */
        if (old_type_id == type_id) {
            return info->index;
        }

        /* If committing while iterating, obtain component columns from the
         * stage. Otherwise, obtain columns from the table directly. */
        old_index = info->index;
        
        if (in_progress) {
            old_columns = ecs_map_get(stage->data_stage, old_type_id);
        } else {
            old_columns = old_table->columns;
        }

        old_type = old_table->type;
    } else if (!in_progress) {
        /* Entity ranges are only checked when not iterating. It is allowed to
         * modify entities that existed before setting the range, and thus the
         * range checks are only applied if the old_table is NULL, meaning the
         * entity did not yet exist/was empty. When iterating, old_table refers
         * to a table in the data stage, not to the table in the main stage.
         * Therefore it is not possible to check while in progress if the entity
         * already existed. Instead, the check will be applied when the entity
         * is merged, which will invoke commit_w_type again. */
        if (stage->range_check_enabled) {
            ecs_assert(!world->max_handle || entity <= world->max_handle, ECS_OUT_OF_RANGE, 0);
            ecs_assert(entity >= world->min_handle, ECS_OUT_OF_RANGE, 0);
        }
    }

    /* If the entity has a negative index, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (old_index < 0) {
        world->should_match = true;
    }

    /* If the new type contains components (that is, it is not 0) obtain the new
     * table and new columns. */
    if (type_id) {
        ecs_vector_t *old_table_db = stage->tables;
        new_table = ecs_world_get_table(world, stage, type_id);

        if (old_table && old_table_db != stage->tables) {
            old_table = ecs_world_get_table(world, stage, info->type_id);
        }

        /* This operation will automatically obtain components from the stage if
         * the application is iterating. */
        new_columns = ecs_table_get_columns(world, stage, new_table);
        ecs_assert(new_columns != NULL, ECS_INTERNAL_ERROR, 0);

        new_index = ecs_table_insert(world, new_table, new_columns, entity);
        ecs_assert(new_index != 0, ECS_INTERNAL_ERROR, 0);
    }

    /* Copy components from old table to new table, only if the entity was not
     * empty, and will not be empty */
    if (old_type_id && type_id) {
        copy_row(new_table->type, new_columns, new_index, 
            old_type, old_columns, old_index);
    }

    /* Update the entity index so that it points to the new table */
    if (type_id) {
        ecs_row_t new_row = (ecs_row_t){.type_id = type_id, .index = new_index};

        /* If old row was being watched, make sure new row is as well */
        if (old_index < 0) {
            new_row.index *= -1;
        }

        ecs_map_set64(entity_index, entity, ecs_from_row(new_row));
    } else {
        if (in_progress) {
            /* The entity must be kept in the stage index because otherwise the
             * merge doesn't know that it needs to merge data for the entity */
            ecs_map_set64(entity_index, entity, 0);
        } else {
            ecs_map_remove(entity_index, entity);
        }
    }

    if (!in_progress) {
        bool merged = false;

        /* Invoke the OnRemove callbacks when there are components to remove,
         * but only when not in progress. If we are currently in progress, the
         * OnRemove handlers will be invoked during the merge at the end of the
         * frame, to ensure that no data is cleaned up while we are still
         * iterating. 
         * 
         * Note that this action is performanced *after* the entity index has
         * been updated. An OnRemove action can itself contain operations that
         * update the entity index, so we need to make sure the index is up to
         * date before invoking the callback.
         */
        if (to_remove && old_index) {
            merged = notify_post_merge(
                world, stage, old_table, old_columns, old_index - 1, 1, to_remove);
        }

        /* After the cleanup code has been invoked we can finally remove the
         * entity from the old table. If an OnRemove handler was invoked, it did
         * already trigger a merge. Since the entity index was already updated
         * to reflect the removed component, the merge will already have removed
         * the entity from the table. In that case, we won't have to do it
         * again. Additionally, if the entity was not stored in any table before
         * this commit, we also don't need to perform the delete. */
        if (!merged && old_type_id) {
            ecs_table_delete(world, old_table, old_index);
        }
    }

    uint32_t last_count = stage->commit_count ++;
    stage->from_type = old_type_id;
    stage->to_type = type_id;

    info->type_id = type_id;
    info->index = new_index;
    info->table = new_table;
    info->columns = new_columns;
    info->commit_count = last_count;
    info->stage = stage;

    /* After the entity has been created in the new table and the stage is
     * consistent again with the entity index, execute the OnAdd actions. */
    if (type_id) {
        if (to_add) {
            /* If in progress, check that overrides, OnAdd systems and prefab 
             * instantiations don't take place if the component is already added 
             * in the main stage. */
            if (in_progress) {
                ecs_row_t main_row = row_from_stage(&world->main_stage, entity);
                to_add = _ecs_merge_type(world, to_add, 0, main_row.type_id);
            }

            if (to_add) {
                notify_after_commit(
                    world, stage, info, new_index - 1, 1, to_add, do_set);
            }
        }
    }

    /* This is a crude way to signal to the world that we need to redetermine
     * the schedule for the worker threads */
    world->valid_schedule = false;

    return new_index;
}


/* -- Private functions -- */

void* ecs_get_ptr_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info,
    ecs_entity_t component,
    bool staged_only,
    bool search_prefab)
{
    ecs_type_t type_id = 0, staged_id = 0;
    ecs_entity_t entity = info->entity;
    void *ptr = NULL;

    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    if (world->in_progress && stage != &world->main_stage) {
        if (populate_info(world, stage, info)) {
            ptr = get_row_ptr(info->table->type, info->columns, info->index, component);
        }

        if (!ptr) {
            /* If component is in to remove type, it has been removed while in
             * progress. Return NULL if so. */
            ecs_type_t to_remove = ecs_map_get64(stage->remove_merge, entity);
            if (to_remove) {
                if (ecs_type_contains_component(
                    world, stage, to_remove, component, false)) 
                {
                    return NULL;
                }
            }
        } 
    }

    if (ptr) return ptr;

    ecs_entity_t prefab = 0;

    if (!world->in_progress || !staged_only) {
        if (populate_info(world, &world->main_stage, info)) {
            ptr = get_row_ptr(info->table->type, info->columns, info->index, component);
        }

        if (ptr) return ptr;

        type_id = info->type_id;

        if (type_id && search_prefab) {
            bool new_table = false;
            if (info->table) {
                new_table = info->table->flags & EcsTableIsStaged;
            }            

            prefab = ecs_get_prefab_from_type(world, stage, new_table, 0, type_id);
        }
    }

    if (!prefab && staged_id && search_prefab) {
        prefab = ecs_map_get64(world->prefab_index, staged_id);
    }

    if (prefab) {
        if (component != EEcsId && component != EEcsPrefab) {
            *info = (ecs_entity_info_t){.entity = prefab};
            return ecs_get_ptr_intern(
                world, stage, info, component, staged_only, true);
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

ecs_type_t ecs_notify(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_map_t *index,
    ecs_type_t type_id,
    ecs_table_t *table,
    ecs_table_column_t *table_columns,
    int32_t offset,
    int32_t limit)
{
    ecs_vector_t *systems = ecs_map_get(index, type_id);
    ecs_type_t modified = 0;

    if (systems) {
        ecs_entity_t *buffer = ecs_vector_first(systems);
        uint32_t i, count = ecs_vector_count(systems);

        for (i = 0; i < count; i ++) {
            ecs_type_t m = ecs_notify_row_system(
                world, buffer[i], table->type, table, table_columns, offset, 
                limit);
            
            if (i) {
                modified = ecs_type_merge(world, stage, modified, m, 0);
            } else {
                modified = m;
            }
        }
    } 

    return modified;
}

void ecs_merge_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_row_t *staged_row)
{
    ecs_row_t old_row = {0};
    ecs_table_t *old_table = NULL;
    
    if (stage_has_entity(&world->main_stage, entity, &old_row)) {
        old_table = ecs_world_get_table(world, stage, old_row.type_id);
    }

    ecs_type_t to_remove = ecs_map_get64(stage->remove_merge, entity);
    ecs_type_t staged_id = staged_row->type_id;    
    ecs_type_t type_id = ecs_type_merge(
        world, stage, old_row.type_id, staged_row->type_id, to_remove);

    ecs_entity_info_t info = {
        .entity = entity,
        .table = old_table,
        .type_id = old_row.type_id,
        .index = old_row.index
    };

    int32_t new_index = commit_w_type(
        world, &world->main_stage, &info, type_id, 0, to_remove, false);

    if (type_id && staged_id) {
        ecs_table_t *new_table = ecs_world_get_table(world, stage, type_id);
        assert(new_table != NULL);

        ecs_table_t *staged_table = ecs_world_get_table(world, stage, staged_id);
        ecs_table_column_t *staged_columns = ecs_map_get(
            stage->data_stage, staged_row->type_id);

        ecs_assert(staged_columns != NULL, ECS_INTERNAL_ERROR, NULL);

        if (new_index < 0) {
            new_index *= -1;
        }

        copy_row( new_table->type, new_table->columns, new_index,
                  staged_table->type, staged_columns, staged_row->index); 
    }
}

void ecs_set_watching(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool watching)
{    
    ecs_row_t row = row_from_stage(&world->main_stage, entity);

    if (watching) {
        if (row.index > 0) {
            row.index *= -1;
            ecs_map_set64(
                world->main_stage.entity_index, entity, ecs_from_row(row));
        }
    } else {
        if (row.index < 0) {
            row.index *= -1;
            ecs_map_set64(
                world->main_stage.entity_index, entity, ecs_from_row(row));
        }
    }
}

bool ecs_components_contains_component(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_entity_t component,
    ecs_entity_t *entity_out)
{
    ecs_vector_t *components = ecs_type_get(world, &world->main_stage, table_type);
    assert(components != NULL);

    uint32_t i, count = ecs_vector_count(components);
    for (i = 0; i < count; i ++) {
        ecs_entity_t h = *(ecs_entity_t*)ecs_vector_get(
            components, &handle_arr_params, i);

        ecs_row_t row = row_from_stage(&world->main_stage, h);
        bool result = ecs_type_contains_component(
            world, &world->main_stage, row.type_id, component, true);
        if (result) {
            if (entity_out) *entity_out = h;
            return true;
        }
    }

    return false;
}

void ecs_add_remove_intern(
    ecs_world_t *world,
    ecs_entity_info_t *info,
    ecs_type_t to_add_type,
    ecs_type_t to_remove_type,
    bool do_set)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    ecs_type_t dst_type = 0;

    if (populate_info(world, stage, info)) {
        ecs_vector_t *to_add = ecs_type_get(world, stage, to_add_type);
        ecs_vector_t *to_remove = ecs_type_get(world, stage, to_remove_type);
        dst_type = ecs_type_merge_arr(
            world, stage, info->table->type, to_add, to_remove);
    } else {
        dst_type = to_add_type;
    }

    commit_w_type(
        world, stage, info, dst_type, to_add_type, to_remove_type, do_set);
}

/* -- Public functions -- */

ecs_entity_t _ecs_new(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    ecs_entity_t entity = ++ world->last_handle;

    ecs_assert(!world->max_handle || entity <= world->max_handle, 
        ECS_OUT_OF_RANGE, NULL);

    if (type) {
        ecs_entity_info_t info = {
            .entity = entity
        };

        commit_w_type(world, stage, &info, type, type, 0, true);
    }

    return entity;
}

ecs_entity_t _ecs_new_w_count(
    ecs_world_t *world,
    ecs_type_t type,
    uint32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_t result = world->last_handle + 1;
    
    world->last_handle += count;

    ecs_assert(!world->max_handle || world->last_handle <= world->max_handle, 
        ECS_OUT_OF_RANGE, NULL);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    if (type) {
        ecs_table_t *table = ecs_world_get_table(world, stage, type);

        ecs_table_column_t *columns = ecs_table_get_columns(
                world, stage, table);
        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, 0);

        uint32_t row = ecs_table_grow(world, table, columns, count, result);

        ecs_map_t *entity_index = stage->entity_index;

        uint32_t cur_index_count = ecs_map_count(entity_index);
        ecs_map_set_size(entity_index, cur_index_count + count);

        uint64_t i, cur_row = row;
        for (i = result; i < (result + count); i ++) {
            /* We need to commit each entity individually in order to populate
             * the entity index */

            ecs_row_t new_row = (ecs_row_t){.type_id = type, .index = cur_row};
            ecs_map_set64(entity_index, i, ecs_from_row(new_row));

            cur_row ++;
        }

        ecs_entity_info_t info = {
            .entity = result, 
            .table = table, 
            .columns = columns, 
            .type_id = type
        };

        notify_after_commit(
            world_arg, stage, &info, row - 1, count, type, true);
    }

    return result;
}

ecs_entity_t _ecs_new_child_w_count(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_type_t type,
    uint32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    ecs_type_t TFullType = type;
    
    if (parent) {
        if (!ecs_has(world, parent, EcsContainer)) {
            ecs_add(world, parent, EcsContainer);
            ecs_set_watching(world, parent, true);
            world->should_match = true;
        }
        ecs_type_t TParentType = ecs_type_from_entity(world, parent);
        TFullType = ecs_merge_type(world, FullType, ParentType, 0);
    }

    return ecs_new_w_count(world, FullType, count);
}

ecs_entity_t _ecs_new_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    ecs_type_t TFullType = type;
    
    if (parent) {
        if (!ecs_has(world, parent, EcsContainer)) {
            ecs_add(world, parent, EcsContainer);
            ecs_set_watching(world, parent, true);
            world->should_match = true;
        }
        ecs_type_t TParentType = ecs_type_from_entity(world, parent);
        TFullType = ecs_merge_type(world, FullType, ParentType, 0);
    }

    return ecs_new(world, FullType);
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETERS, NULL);

    ecs_row_t row;
    ecs_stage_t *stage = ecs_get_stage(&world);
    bool in_progress = world->in_progress;

    if (!in_progress) {
        if (stage_has_entity(&world->main_stage, entity, &row)) {
            ecs_entity_info_t info = {
                .entity = entity,
                .type_id = row.type_id,
                .index = row.index,
                .table = ecs_world_get_table(world, stage, row.type_id)
            };

            commit_w_type(world, stage, &info, 0, 0, row.type_id, false);

            ecs_map_remove(world->main_stage.entity_index, entity);
        }
    } else {
        /* Mark components of the entity in the main stage as removed. This will
         * ensure that subsequent calls to ecs_has, ecs_get and ecs_is_empty will
         * behave consistently with the delete. */
        if (stage_has_entity(&world->main_stage, entity, &row)) {
            ecs_map_set64(stage->remove_merge, entity, row.type_id);
        }

        /* Remove the entity from the staged index. Any added components while
         * in progress will be discarded as a result. */
        ecs_map_set64(stage->entity_index, entity, 0);
    }
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    ecs_entity_t result = ++ world->last_handle;
    if (entity) {
        ecs_row_t row;
        if (stage_has_entity(&world->main_stage, entity, &row)) {
            ecs_type_t type_id = row.type_id;
            ecs_entity_info_t info = {
                .entity = result,
                .type_id = 0,
                .index = row.index
            };

            commit_w_type(world, stage, &info, type_id, type_id, 0, false);

            if (copy_value) {
                ecs_table_t *from_table = ecs_world_get_table(world, stage, type_id);
                ecs_table_column_t *to_columns = NULL, *from_columns = from_table->columns;
                ecs_table_t *to_table = ecs_world_get_table(world, stage, type_id);

                if (world->in_progress) {
                    to_columns = ecs_map_get(stage->data_stage, type_id);
                } else {
                    to_columns = to_table->columns;
                }

                ecs_row_t to_row = row_from_stage(stage, result);

                if (!to_table)
                    to_table = from_table;

                if (!to_columns)
                    to_columns = from_columns;

                if (!stage_has_entity(stage, result, &to_row))
                    to_row = row_from_stage(&world->main_stage, result);

                copy_row(to_table->type, to_columns, to_row.index,
                    from_table->type, from_columns, row.index);

                /* A clone with value is equivalent to a set */
                ecs_notify(
                    world_arg, stage, world->type_sys_set_index, 
                    from_table->type_id, to_table, to_columns, to_row.index - 1, 
                    1);
            }
        }
    }

    return result;
}

void _ecs_add(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entity_info_t info = {.entity = entity};
    ecs_add_remove_intern(world, &info, type, 0, true);
}

void _ecs_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entity_info_t info = {.entity = entity};
    ecs_add_remove_intern(world, &info, 0, type, false);
}

void _ecs_add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t add_type,
    ecs_type_t remove_type)
{
    ecs_entity_info_t info = {.entity = entity};
    ecs_add_remove_intern(world, &info, add_type, remove_type, false);
}

void ecs_adopt(
    ecs_world_t *world,
    ecs_entity_t child,
    ecs_entity_t parent)
{    
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    if (!ecs_has(world, parent, EcsContainer)) {
        ecs_add(world, parent, EcsContainer);
        ecs_set_watching(world, parent, true);
        world->should_match = true;
    }

    ecs_type_t TParentType = ecs_type_from_entity(world, parent);

    ecs_add(world, child, ParentType);
}

void ecs_orphan(
    ecs_world_t *world,
    ecs_entity_t child,
    ecs_entity_t parent)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    ecs_type_t TParentType = ecs_type_from_entity(world, parent);

    ecs_remove(world, child, ParentType);    
}

void* _ecs_get_ptr(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);

    /* Get only accepts types that hold a single component */
    ecs_entity_t component = ecs_entity_from_type(world_arg, type);

    ecs_entity_info_t info = {.entity = entity};
    return ecs_get_ptr_intern(world, stage, &info, component, false, true);
}

static
ecs_entity_t _ecs_set_ptr_intern(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    size_t size,
    void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info = {.entity = entity};

    /* Set only accepts types that hold a single component */
    ecs_entity_t component = ecs_entity_from_type(world_arg, type);

    /* If component hasn't been added to entity yet, add it */
    int *dst = ecs_get_ptr_intern(world, stage, &info, component, true, false);
    if (!dst) {
        ecs_add_remove_intern(world_arg, &info, type, 0, false);
        dst = ecs_get_ptr_intern(world, stage, &info, component, true, false);
        if (!dst) {
            /* It is possible that an OnAdd system removed the component before
             * it could have been set */
            return entity;
        }
    }

#ifndef NDEBUG
    ecs_entity_info_t cinfo = {.entity = component};
    EcsComponent *cdata = ecs_get_ptr_intern(
        world, stage, &cinfo, EEcsComponent, false, false);
    ecs_assert(cdata->size == size, ECS_INVALID_COMPONENT_SIZE, NULL);
#endif

    memcpy(dst, ptr, size);

    notify_pre_merge(
        world_arg, stage, info.table, info.columns, info.index - 1, 1, type,
        world->type_sys_set_index);

    return entity;
}

ecs_entity_t _ecs_set_ptr(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    size_t size,
    void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETERS, NULL);

    /* If no entity is specified, create one */
    if (!entity) {
        entity = _ecs_new(world, type);
    }

    return _ecs_set_ptr_intern(world, entity, type, size, ptr);
}

ecs_entity_t _ecs_set_singleton_ptr(
    ecs_world_t *world,
    ecs_type_t type,
    size_t size,
    void *ptr)
{
    return _ecs_set_ptr_intern(world, 0, type, size, ptr);
}

bool _ecs_has(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    if (!type) {
        return true;
    }

    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_type_t entity_type = ecs_get_type(world_arg, entity);

    return ecs_type_contains(world, stage, entity_type, type, true, true) != 0;
}

bool _ecs_has_any(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    if (!type) {
        return true;
    }

    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_type_t entity_type = ecs_get_type(world_arg, entity);
    return ecs_type_contains(world, stage, entity_type, type, false, true);
}

bool ecs_contains(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    if (!parent || !child) {
        return false;
    }

    ecs_type_t TParent = ecs_type_from_entity(world, parent);
    return ecs_has(world, child, Parent);
}

ecs_entity_t ecs_get_parent(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_entity_t parent = 0;
    ecs_type_t type = ecs_get_type(world, entity);
    
    ecs_components_contains_component(
        world, type, component, &parent);

    return parent;
}

const char* ecs_get_id(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    EcsId *id = ecs_get_ptr(world, entity, EcsId);
    if (id) {
        return *id;
    } else {
        return NULL;
    }
}

bool ecs_is_empty(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_row_t cur = row_from_stage(&world->main_stage, entity);

    if (world->in_progress) {
        ecs_row_t to_add = row_from_stage(stage, entity);
        uint64_t to_remove64 = ecs_map_get64(stage->remove_merge, entity);

        ecs_row_t to_remove = ecs_to_row(to_remove64);
        ecs_type_t result = ecs_type_merge(world, stage, 
            cur.type_id, to_add.type_id, to_remove.type_id);

        return result == 0;   
    } else {
        return cur.type_id == 0;
    }
}

ecs_type_t ecs_type_from_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);   

    return ecs_type_from_handle(world, stage, entity, NULL);
}

ecs_entity_t ecs_entity_from_type(
    ecs_world_t *world, 
    ecs_type_t type_id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);

    if (!type_id) {
        return 0;
    }
    
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_vector_t *type = ecs_map_get(world->main_stage.type_index, type_id);
    if (!type) {
        if (world->in_progress) {
            type = ecs_map_get(stage->type_index, type_id);
        }
    }

    ecs_assert(type != NULL, ECS_UNKNOWN_TYPE_ID, NULL);

    /* If array contains n entities, it cannot be reduced to a single entity */
    if (ecs_vector_count(type) != 1) {
        ecs_abort(ECS_TYPE_NOT_AN_ENTITY, NULL);
    }

    return ((ecs_entity_t*)ecs_vector_first(type))[0];
}

ecs_type_t ecs_get_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_row_t row = row_from_stage(stage, entity);
    ecs_type_t result = row.type_id;

    if (world->in_progress) {
        ecs_type_t remove_type = ecs_map_get64(stage->remove_merge, entity);
        ecs_row_t main_row = row_from_stage(&world->main_stage, entity);
        result = ecs_type_merge(world, stage, main_row.type_id, result, remove_type);
    }
    
    return result;
}

uint32_t _ecs_count(
    ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return 0;
    }

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_vector_t *table_array = world->main_stage.tables;
    
    ecs_table_t *tables = ecs_vector_first(table_array);
    uint32_t i, count = ecs_vector_count(table_array);
    uint32_t result = 0;

    for (i = 0; i < count; i ++) {
        if (ecs_type_contains(world, stage, tables[i].type_id, type, true, true)) {
            result += ecs_vector_count(tables[i].columns[0].data);
        }
    }
    
    return result;
}

ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    const char *id,
    size_t size)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETERS, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    result = _ecs_new(world, world->t_component);
    ecs_set(world, result, EcsComponent, {.size = size});
    ecs_set(world, result, EcsId, {id});

    return result;
}
