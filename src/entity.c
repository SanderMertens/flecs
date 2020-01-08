
#include "flecs_private.h"

static
void copy_column(
    ecs_column_t *new_column,
    int32_t new_index,
    ecs_column_t *old_column,
    int32_t old_index)
{
    ecs_assert(new_index > 0, ECS_INTERNAL_ERROR, NULL);

    int32_t size = new_column->size;

    if (size) {
        size_t column_size = new_column->size;

        if (old_index < 0) old_index *= -1;
        
        void *dst = _ecs_vector_get(new_column->data, column_size, new_index - 1);
        void *src = _ecs_vector_get(old_column->data, column_size, old_index - 1);
            
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

        memcpy(dst, src, column_size);
    }
}

static
void copy_row(
    ecs_type_t new_type,
    ecs_column_t *new_columns,
    int32_t new_index,
    ecs_type_t old_type,
    ecs_column_t *old_columns,
    int32_t old_index)
{
    uint16_t i_new, new_component_count = ecs_vector_count(new_type);
    uint16_t i_old = 0, old_component_count = ecs_vector_count(old_type);
    ecs_entity_t *new_components = ecs_vector_first(new_type);
    ecs_entity_t *old_components = ecs_vector_first(old_type);

    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_columns->data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_columns->data != NULL, ECS_INTERNAL_ERROR, NULL);

    for (i_new = 0; i_new < new_component_count; ) {
        if (i_old == old_component_count) {
            break;
        }

        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if ((new_component & ECS_ENTITY_FLAGS_MASK) || 
            (old_component & ECS_ENTITY_FLAGS_MASK)) 
        {
            break;
        }

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

/* Get pointer to single component value */
static
void* get_component(
    ecs_entity_info_t *info,
    ecs_entity_t component)
{
    ecs_table_t *table = info->table;
    if (!table) {
        return NULL;
    }

    int32_t row = info->row;
    ecs_assert(row >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;
    ecs_assert(ecs_vector_count(type) < ECS_MAX_ENTITIES_IN_TYPE, ECS_TYPE_TOO_LARGE, NULL);

    ecs_column_t *columns = info->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *array = ecs_vector_first(type);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        if (array[i] == component) {
            ecs_column_t *component = &columns[i + 1];
            uint32_t size = component->size; 
            void *data = ecs_vector_first(component->data);
            ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);            
            return ECS_OFFSET(data, row * size);
        }
    }
    
    return NULL;
}

/* Get new entity handle */
static
ecs_entity_t new_entity_handle(
    ecs_world_t *world)
{
    ecs_entity_t entity = ++ world->last_handle;
    ecs_assert(!world->max_handle || entity <= world->max_handle, 
        ECS_OUT_OF_RANGE, NULL);
    return entity;
}

/* Get entity from main stage */
static
ecs_record_t* get_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    if (entity == ECS_SINGLETON) {
        return &world->singleton;
    } else {
        return ecs_map_get(world->main_stage.entity_index, ecs_record_t, entity);
    }
}

/* Get or create entity in main stage */
static
ecs_record_t* get_or_create_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool *is_new)
{
    if (entity == ECS_SINGLETON) {
        return &world->singleton;
    } else {
        ecs_record_t *record = ecs_map_get(
            world->main_stage.entity_index, ecs_record_t, entity);

        if (!record) {
            *is_new = true;
            ecs_record_t new_record = { 0 };
            ecs_map_set(world->main_stage.entity_index, entity, &new_record);
            record = ecs_map_get(world->main_stage.entity_index, ecs_record_t, entity);
        }

        return record;
    }
}

/* Get entity from stage that is not the main stage */
static
ecs_record_t* get_staged_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    ecs_assert(stage->entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_map_get(stage->entity_index, ecs_record_t, entity);
}

/* Utility to compute actual row from row in record */
static
int32_t set_row_info(
    ecs_entity_info_t *info,
    int32_t row)
{
    bool is_watched = row < 0;
    row = row * -(is_watched * 2 - 1) - 1 * (row != 0);
    info->is_watched = is_watched;
    return info->row = row;
}

/* Utility to set info from main stage record */
static
void set_info_from_record(
    ecs_world_t *world,
    ecs_entity_info_t *info,
    ecs_record_t *record)
{
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

    info->record = record;

    ecs_table_t *table = info->table;
    info->table = table;
    if (!table) {
        return;
    }

    set_row_info(info, record->row);

    ecs_column_t *columns = table->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    info->columns = columns;
}

/* Get info from main stage */
static
bool get_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_record_t *record = get_entity(world, entity);
    if (!record) {
        return false;
    }

    set_info_from_record(world, info, record);

    return true;
}

/* Get info from main stage, get or create record */
static
void get_or_create_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    bool is_new = false;
    ecs_record_t *record = get_or_create_entity(world, entity, &is_new);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (is_new) {
        record->type = NULL;
    }
    
    set_info_from_record(world, info, record);
}

/* Get info from any stage but main stage */
static
bool get_staged_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_record_t *record = get_staged_entity(world, stage, entity);
    if (!record) {
        return false;
    }

    ecs_table_t *table = ecs_world_get_table(world, stage, record->type);
    info->table = table;
    if (!table) {
        return true;
    }    
    
    ecs_column_t *columns = ecs_map_get_ptr(stage->data_stage, ecs_column_t*, (uintptr_t)record->type);
    info->columns = columns;

    // ecs_assert(columns->record_ptrs != NULL, ECS_INTERNAL_ERROR, NULL);
    // ecs_assert(ecs_vector_count(columns->record_ptrs) >= row, ECS_INTERNAL_ERROR, NULL);
    // ecs_record_t**record_ptrs = ecs_vector_first(columns->record_ptrs);
    info->record = get_entity(world, entity);

    return true;
}

/* Get info from either stage, and get or create record if in main stage. This
 * functionality is optimized for operations that are about to create a new
 * entity, and prevents an additional lookup in the main entity index. */
static
bool get_or_create_staged_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    if (stage != &world->main_stage) {
        if (get_staged_info(world, stage, entity, info)) {
            return true;
        }

        /* If the entity is not found in the stage but we're about to create the
         * entity, find the main stage table the entity is stored in (if any).
         * This ensures that when traversing the table graph, we use the correct
         * table as origin. Do not use get_or_create, as we can't change the
         * main stage at this point. */
        if (get_info(world, entity, info)) {
            return true;
        }

        return false;
    } else {
        get_or_create_info(world, entity, info);
        return true;
    }
}

/* Get entity */
ecs_record_t* ecs_get_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (stage == &world->main_stage) {
        return get_entity(world, entity);
    } else {
        return get_staged_entity(world, stage, entity);
    }
}

/* Get entity info */
bool ecs_get_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    if (stage == &world->main_stage) {
        return get_info(world, entity, info);
    } else {
        if (!get_staged_info(world, stage, entity, info)) {
            return get_info(world, entity, info);
        }

        return true;
    }    
}

/* Set entity */
void ecs_set_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    if (stage == &world->main_stage) {
        if (entity == ECS_SINGLETON) {
            world->singleton = *record;
        } else {
            ecs_record_t *dst_record = ecs_set_entity_in_main(world, entity);
            *dst_record = *record;
        }

        // ecs_assert(
        //     ecs_vector_count(record->table->columns[0]->entities) >= record->row, 
        //     ECS_INTERNAL_ERROR, 
        //     NULL);        
    } else {
        ecs_map_set(stage->entity_index, entity, record);
    }
}

/* Set entity in main stage */
ecs_record_t* ecs_set_entity_in_main(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    bool is_new = false;
    ecs_record_t *record = get_or_create_entity(world, entity, &is_new);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (is_new) {
        record->type = NULL;
    }

    return record;
}

/* Delete entity from stage */
void ecs_delete_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (stage == &world->main_stage) {
        if (entity == ECS_SINGLETON) {
            world->singleton = (ecs_record_t){0, 0};
        } else {
            //_ecs_sparse_remove(world->main_stage.entity_index, 0, entity);
        }
    } else {
        // ...
    }

    ecs_map_remove(stage->entity_index, entity);
}

void ecs_grow_entities(
    ecs_world_t *world,
    ecs_stage_t *stage,
    uint32_t count)
{
    if (!stage || stage == &world->main_stage) {
        // ecs_sparse_grow(world->main_stage.entity_index, count);
    } else {
        // ...
    }

    ecs_map_grow(stage->entity_index, count);
}

uint32_t ecs_count_entities(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    if (!stage || stage == &world->main_stage) {
       // return ecs_sparse_count(world->main_stage.entity_index);
    } else {
    }
    return ecs_map_count(stage->entity_index);
}

static
ecs_type_t notify_pre_merge(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_column_t *table_columns,
    int32_t offset,
    int32_t limit,
    ecs_type_t to_init,
    ecs_map_t *systems)
{
    ecs_world_t *real_world = world;
    ecs_get_stage(&real_world);
    
    ecs_assert(!real_world->is_merging, ECS_INTERNAL_ERROR, 0);

    bool in_progress = real_world->in_progress;
    real_world->in_progress = true;

    ecs_type_t result = ecs_notify(
        real_world, stage, systems, to_init, table, table_columns, offset, limit);

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
    ecs_column_t *table_columns,
    int32_t offset,
    int32_t limit,
    ecs_type_t to_deinit)
{
    /* This will trigger asserts when operations are invoked during a merge that
     * are not valid while merging. */
    bool is_merging = world->is_merging;
    world->is_merging = true;

    bool result = ecs_notify(
        world, stage, world->type_sys_remove_index, to_deinit, table, 
        table_columns, offset, limit) != NULL;

    world->is_merging = is_merging;

    return result;
}

static
ecs_type_t instantiate_prefab(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *entity_info,
    bool is_prefab,
    ecs_entity_info_t *prefab_info,
    int32_t limit,
    ecs_type_t modified)
{
    ecs_column_t *entity_columns = ecs_table_get_columns(world, stage, entity_info->table);
    ecs_entity_t *entity_ids = ecs_vector_first(entity_columns[0].data);

    EcsPrefabBuilder *builder = get_component(prefab_info, EEcsPrefabBuilder);

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

                int32_t j;
                for (j = 0; j < limit; j ++) {
                    int32_t row = entity_info->row + j - 1;
                    ecs_entity_t entity = entity_ids[row];
                    ecs_adopt(world, child + j, entity);
                    ecs_set(world, child + j, EcsId, {op->id});
                }
            }
        }

        /* Keep track of components shared from new prefabs */
        modified = ecs_type_merge_intern(
            world, stage, modified, prefab_info->table->type, 0);

    /* If the current entity is also prefab, do not add children to
     * it. Instead, add children (if any) of its base to its ops */
    } else if (builder) {
        int32_t row = entity_info->row - 1;
        ecs_entity_t entity = entity_ids[row];
        EcsPrefabBuilder *entity_builder = ecs_get_ptr_intern(world, 
            stage, entity, EEcsPrefabBuilder, false, false);

        if (!entity_builder) {
            ecs_add(world, entity, EcsPrefabBuilder);
            entity_builder = ecs_get_ptr(world, entity, EcsPrefabBuilder);
            entity_builder->ops = NULL;
        }

        int32_t count = ecs_vector_count(builder->ops);
        void *new_ops = ecs_vector_addn(
            &entity_builder->ops, ecs_builder_op_t, count);
        
        memcpy(new_ops, ecs_vector_first(builder->ops), 
            sizeof(ecs_builder_op_t) * count);
    }

    return modified;
}

int32_t ecs_type_get_prefab(
    ecs_type_t type,
    int32_t n)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *buffer = ecs_vector_first(type);

    for (i = n + 1; i < count; i ++) {
        ecs_entity_t e = buffer[i];
        if (e & ECS_INSTANCEOF) {
            return i;
        }
    }

    return -1;
}

static
ecs_type_t copy_from_prefab(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t prefab,
    ecs_entity_info_t *prefab_info,
    ecs_entity_info_t *info,
    int32_t offset,
    int32_t limit,
    ecs_type_t to_add,
    ecs_type_t modified)
{
    ecs_table_t *prefab_table = prefab_info->table;
    ecs_type_t prefab_type = prefab_table->type;
    ecs_column_t *prefab_columns = prefab_table->columns;
    int32_t prefab_row = prefab_info->row;

    ecs_assert(prefab_row != -1, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(info->row != -1, ECS_INTERNAL_ERROR, NULL);

    int32_t e, add_count = ecs_vector_count(to_add);
    int32_t p = 0, prefab_count = ecs_vector_count(prefab_type);
    ecs_entity_t *to_add_buffer = ecs_vector_first(to_add);
    ecs_entity_t *prefab_type_buffer = ecs_vector_first(prefab_type);

    bool is_prefab = false;
    ecs_column_t *columns = info->columns;

    for (e = 0; e < add_count; e ++) {
        ecs_entity_t pe = 0, ee = to_add_buffer[e] & ECS_ENTITY_MASK;

        /* Keep track of whether this entity became a prefab */
        if (ee == EEcsPrefab) {
            is_prefab = true;
            continue;
        }

        /* Never copy EcsId and EcsPrefabBuilder components from base */
        if (ee == EEcsId || ee == EEcsPrefabBuilder) {
            continue;
        }

        /* If added entity is an instance of base, instantiate it */
        if (ee == prefab) {
            modified = instantiate_prefab(world, stage, info, is_prefab, 
                prefab_info, limit, modified);
            continue;
        }

        /* Find the corresponding component in the base type */
        while ((p < prefab_count) && (pe = prefab_type_buffer[p]) < ee) {
            p ++;
        }

        /* If base hasn't been found, continue */
        if (ee != pe) {
            continue;
        }

        ecs_column_t *src_column = &prefab_columns[p + 1];
        int32_t size = src_column->size;

        if (size) {
            void *src_column_data = ecs_vector_first(src_column->data);
            void *src_ptr = ECS_OFFSET(
                src_column_data, size * (prefab_row - 1));

            int32_t dst_col_index;
            if (info->table->type == to_add) {
                dst_col_index = e;
            } else {
                dst_col_index = ecs_type_index_of(info->table->type, ee);
            }
            
            ecs_column_t *dst_column = &columns[dst_col_index + 1];
            void *dst_column_data = ecs_vector_first(dst_column->data);
            void *dst_ptr = ECS_OFFSET(
                dst_column_data, size * (info->row - 1 + offset));

            int32_t i;
            for (i = 0; i < limit; i ++) {
                memcpy(dst_ptr, src_ptr, size);
                dst_ptr = ECS_OFFSET(dst_ptr, size);
            }
        }
    }

    if (modified) {
        /* Always strip EcsPrefab, as an entity will never inherit the EcsPrefab
         * component from a prefab. Same for EcsId. */
        modified = ecs_type_merge_intern(world, stage, modified, 0, ecs_type(EcsPrefab));
        modified = ecs_type_merge_intern(world, stage, modified, 0, ecs_type(EcsId));
    }    

    return modified;
}

static
ecs_type_t copy_from_prefabs(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    int32_t offset,
    int32_t limit,
    ecs_entity_t sub_prefab,
    ecs_type_t src_type,
    ecs_type_t to_add,
    ecs_type_t modified)
{
    /* Use get_type, so we have the combined staged/unstaged entity type */
    ecs_type_t type;

    if (src_type) {
        type = src_type;
    } else {
        if (world->in_progress && entity) {
            type = ecs_get_type(world, entity);
        } else {
            type = info->table->type;
        }
    }

    ecs_entity_t *type_buffer = ecs_vector_first(type);
    int32_t i = -1;

    while ((i = ecs_type_get_prefab(type, i)) != -1) {
        ecs_entity_t prefab = type_buffer[i] & ECS_ENTITY_MASK;
        ecs_entity_info_t prefab_info = { 0 };

        /* Detect small cycles */
        if (sub_prefab == prefab) {
            continue;
        }

        if (get_info(world, prefab, &prefab_info)) {
            ecs_entity_t sub = sub_prefab;
            if (!sub) {
                sub = prefab;
            }

            /* Walk over prefab components in case there are base prefabs */
            modified = copy_from_prefabs(
                world, stage, entity, info, offset, limit, sub, 
                prefab_info.table->type, to_add, modified);

            /* Copy from current prefab */
            modified = copy_from_prefab(
                world, stage, prefab, &prefab_info, info, offset, limit, to_add, 
                modified);                
        }
    }

    return modified;
}

static
bool notify_after_commit(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    int32_t offset,
    int32_t limit,
    ecs_type_t to_add_id,
    bool do_set)
{
    int32_t row = info->row - 1;
    ecs_world_t *world_arg = world;
    ecs_get_stage(&world);

    ecs_type_t modified = notify_pre_merge (
        world_arg, stage, info->table, info->columns, row + offset, limit, to_add_id, 
        world->type_sys_add_index);

    if (entity) {
        get_staged_info(world, stage, entity, info);
    }

    modified = copy_from_prefabs(
        world, stage, 0, info, offset, limit, 0, NULL, to_add_id, modified);

    if (entity) {
        get_staged_info(world, stage, entity, info);
    }
    
    /* Invoke OnSet handlers if components received their first value */
    if (do_set && modified) {
        notify_pre_merge (
            world_arg, stage, info->table, info->columns, row + offset, limit, 
            modified, world->type_sys_set_index);
    }

    return modified != NULL;
}

/** Commit an entity with a specified type to a table (probably the most 
 * important function in flecs). */
static
int32_t commit(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    bool do_set)
{
    ecs_table_t *new_table = NULL, *old_table;
    ecs_column_t *new_columns = NULL, *old_columns;
    ecs_map_t *entity_index = stage->entity_index;
    ecs_type_t old_type = NULL;
    int32_t new_index = 0, old_index = 0;
    bool in_progress = world->in_progress;
    ecs_type_t remove_type, last_remove_type = NULL;
    ecs_type_t type = NULL;

    if (info->table) {
        type = info->table->type;
    }

    /* Always update remove_merge stage when in progress. It is possible (and
     * likely) that when a component is removed, it hasn't been added in the
     * same iteration. As a result, the staged entity index does not know about
     * the component to remove, and thus the staged type of the entity when
     * merged with the to_remove type id results in the same type id. This would
     * ordinarily cause the commit action to be skipped, but when in progress,
     * we need to keep track of the remove so that it is removed from the main
     * stage once the merge takes place. */
    if (in_progress) {
        /* Update remove type. Add to_remove, and subtract to_add. */
        ecs_type_t *rm_type_ptr = ecs_map_get(stage->remove_merge, ecs_type_t, entity);
        remove_type = rm_type_ptr ? *rm_type_ptr : NULL;
        last_remove_type = remove_type;

        /* If remove_type and to_remove are 0, the result of the merge is going
         * to be 0 as well, so don't bother overwriting the remove_index */
        if (remove_type || to_remove) {
            remove_type = ecs_type_merge_intern(
                world, stage, remove_type, to_remove, to_add);

            if (rm_type_ptr && remove_type) {
                *rm_type_ptr = remove_type;
            } else if (remove_type) {
                ecs_map_set(stage->remove_merge, entity, &remove_type);
            } else {
                /* If the result is 0, remove entity from remove stage which
                 * reduces the work required during the merge */
                ecs_map_remove(stage->remove_merge, entity);
            }
        }
    }

    if ((old_table = info->table)) {
        old_type = old_table->type;

        /* If the type id is the same as that of the table where the entity is
         * currently stored, nothing needs to be committed. */
        if (old_type == type) {
            if (info->row < 0) {
                return -info->row;
            } else {
                return info->row;
            }
        }

        /* If committing while iterating, obtain component columns from the
         * stage. Otherwise, obtain columns from the table directly. */
        old_index = info->row;
        
        if (in_progress) {
            ecs_map_has(stage->data_stage, (uintptr_t)old_type, &old_columns);
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
         * is merged, which will invoke commit again. */

        if (stage->range_check_enabled) {
            ecs_assert(!world->max_handle || entity <= world->max_handle, ECS_OUT_OF_RANGE, 0);
            ecs_assert(entity >= world->min_handle, ECS_OUT_OF_RANGE, 0);
        }
    }

    /* If the entity is being watched, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (info->is_watched) {
        world->should_match = true;
    }

    /* If the new type contains components (that is, it is not 0) obtain the new
     * table and new columns. */
    if (type) {
        new_table = ecs_world_get_table(world, stage, type);

        /* This operation will automatically obtain components from the stage if
         * the application is iterating. */
        new_columns = ecs_table_get_columns(world, stage, new_table);
        ecs_assert(new_columns != NULL, ECS_INTERNAL_ERROR, 0);

        new_index = ecs_table_insert(world, new_table, new_columns, entity);
        ecs_assert(new_index != 0, ECS_INTERNAL_ERROR, 0);
    }

    /* Copy components from old table to new table, only if the entity was not
     * empty, and will not be empty */
    if (old_type && type) {
        copy_row(new_table->type, new_columns, new_index, 
            old_type, old_columns, old_index);
    }

    /* Update the entity index so that it points to the new table */
    if (type) {
        ecs_record_t new_record = (ecs_record_t){.type = type, .row = new_index};

        /* If old row was being watched, make sure new row is as well */
        if (info->is_watched) {
            new_record.row *= -1;
        }

        ecs_map_set(entity_index, entity, &new_record);
    } else {
        if (in_progress) {
            /* The entity must be kept in the stage index because otherwise the
             * merge doesn't know that it needs to merge data for the entity */
            ecs_map_set(entity_index, entity, &((ecs_record_t){0, 0}));
        } else {
            ecs_map_remove(entity_index, entity);
        }
    }

    if (!in_progress) {
        /* Invoke the OnRemove callbacks when there are components to remove,
         * but only when not in progress. If we are currently in progress, the
         * OnRemove handlers will be invoked during the merge at the end of the
         * frame, to ensure that no data is cleaned up while we are still
         * iterating. 
         * 
         * Note that this action is performed *after* the entity index has
         * been updated. An OnRemove action can itself contain operations that
         * update the entity index, so we need to make sure the index is up to
         * date before invoking the callback.
         */
        if (to_remove && old_index) {
            notify_post_merge(
                world, stage, old_table, old_columns, old_index - 1, 1, to_remove);
        }

        /* After the cleanup code has been invoked we can finally remove the
         * entity from the old table. If the entity was not stored in any table 
         * before this commit, we also don't need to perform the delete. */

        if (old_type) {
            ecs_table_delete(world, NULL, old_table, old_columns, old_index);
        }
    }

    stage->from_type = old_type;
    stage->to_type = type;

    info->row = new_index;
    info->table = new_table;
    info->columns = new_columns;

    /* After the entity has been created in the new table and the stage is
     * consistent again with the entity index, execute the OnAdd actions. */
    if (type) {
        if (to_add) {
            /* If in progress, check that overrides, OnAdd systems and prefab 
             * instantiations don't take place if the component is already added 
             * in the main stage. */
            if (in_progress) {
                ecs_record_t *main_record = get_entity(world, entity);
                ecs_type_t main_type = main_record->type;

                /* If components were removed while in progress, subtract them
                 * from the main stage type */
                if (last_remove_type) {
                    main_type = ecs_type_merge_intern(
                        world, stage, main_type, 0, last_remove_type);
                }

                to_add = ecs_type_merge_intern(world, stage, to_add, 0, main_type);
            }

            if (to_add) {
                notify_after_commit(
                    world, stage, entity, info, 0, 1, to_add, do_set);
            }
        }
    }

    /* This is a crude way to signal to the world that we need to redetermine
     * the schedule for the worker threads */
    world->valid_schedule = false;

    return new_index;
}

static
void* get_base_component(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entity_t previous,
    ecs_entity_t component)
{
    ecs_type_t type = info->table->type;
    ecs_entity_t *type_buffer = ecs_vector_first(type);
    int32_t p = -1;
    void *ptr = NULL;

    while (!ptr && (p = ecs_type_get_prefab(type, p)) != -1) {
        ecs_entity_t prefab = type_buffer[p] & ECS_ENTITY_MASK;

        /* Detect cycles with two entities */
        if (prefab == previous) {
            continue;
        }

        ecs_entity_info_t prefab_info;
        if (get_info(world, prefab, &prefab_info)) {
            ptr = get_component(&prefab_info, component);
            if (!ptr) {
                ptr = get_base_component(
                    world, stage, prefab, &prefab_info, entity, component);
            }
        }
    }

    return ptr;
}

/* -- Private functions -- */

void* ecs_get_ptr_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool staged_only,
    bool search_prefab)
{
    ecs_entity_info_t info;
    void *ptr = NULL;
    bool found = false;

    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    if (stage != &world->main_stage) {
        found = get_staged_info(world, stage, entity, &info);
    }

    if (!found && !staged_only) {
        found = get_info(world, entity, &info);
    }

    if (found) {
        if (!info.table) {
            return NULL;
        }

        ptr = get_component(&info, component);

        if (!ptr && search_prefab) {
            if (component != EEcsId && component != EEcsPrefab) {
                ptr = get_base_component(
                    world, stage, entity, &info, 0, component);
            }
        }        
    }

    return ptr;
}

ecs_type_t ecs_notify(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_map_t *index,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_column_t *table_columns,
    int32_t offset,
    int32_t limit)
{
    ecs_vector_t *systems;
    ecs_type_t modified = 0;

    if (ecs_map_has(index, (uintptr_t)type, &systems)) {
        ecs_entity_t *buffer = ecs_vector_first(systems);
        int32_t i, count = ecs_vector_count(systems);

        for (i = 0; i < count; i ++) {
            ecs_type_t m = ecs_notify_row_system(
                world, buffer[i], table->type, table, table_columns, offset, 
                limit);
            
            if (i) {
                modified = ecs_type_merge_intern(world, stage, modified, m, 0);
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
    ecs_record_t staged_record)
{
    ecs_record_t *old_record = {0};
    ecs_table_t *old_table = NULL;

    if ((old_record = get_entity(world, entity))) {
        if (old_record->type) {
            /* It is possible that an entity exists in the main stage but does
             * not have a type. This happens when an empty entity is being 
             * watched, in which case it will have -1 as index, but no type. */
            old_table = ecs_world_get_table(world, stage, old_record->type);
        }
    }

    ecs_type_t to_remove = NULL;
    ecs_map_has(stage->remove_merge, entity, &to_remove);

    ecs_type_t staged_type = staged_record.type;    
    ecs_type_t type = ecs_type_merge_intern(
        world, stage, old_record->type, staged_record.type, to_remove);

    ecs_entity_info_t info = {
        .table = old_table,
        .row = old_record->row
    };

    if (old_record->row < 0) {
        info.is_watched = true;
    }

    int32_t new_index = commit(
        world, &world->main_stage, entity, &info, 0, to_remove, false);
    
    if (type && staged_type) {
        ecs_table_t *new_table = ecs_world_get_table(world, &world->main_stage, type);
        assert(new_table != NULL);

        ecs_table_t *staged_table = ecs_world_get_table(world, stage, staged_type);
        ecs_column_t *staged_columns = NULL;
        ecs_map_has(stage->data_stage, (uintptr_t)staged_record.type, &staged_columns);
        ecs_assert(staged_columns != NULL, ECS_INTERNAL_ERROR, NULL);

        copy_row( new_table->type, new_table->columns, new_index,
                staged_table->type, staged_columns, staged_record.row); 
    }
}

void ecs_set_watch(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{    
    (void)world;
    ecs_record_t *record = get_staged_entity(world, stage, entity);

    if (record->row > 0) {
        record->row *= -1;
    } else if (record->row == 0) {
        /* If entity is empty, there is no index to change the sign of. In
            * this case, set the index to -1, and assign an empty type. */
        record->row = -1;
        record->type = NULL;
    }

    ecs_map_set(stage->entity_index, entity, record);
}

bool ecs_components_contains_component(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t flags,
    ecs_entity_t *entity_out)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *type_buffer = ecs_vector_first(type);

    for (i = 0; i < count; i ++) {
        if (flags) {
            if ((type_buffer[i] & flags) != flags) {
                continue;
            }
        }

        ecs_entity_t e = type_buffer[i] & ECS_ENTITY_MASK;
        ecs_type_t type = ecs_get_type(world, e);

        bool result = ecs_type_has_entity_intern(
            world, type, component, true);
        if (result) {
            if (entity_out) *entity_out = e;
            return true;
        }
    }

    return false;
}

void ecs_add_remove_intern(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    bool do_set)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    ecs_entity_info_t info;
    if (!get_or_create_staged_info(world, stage, entity, &info)) {
        info.table = NULL;
    }

    commit(world, stage, entity, &info, to_add, to_remove, do_set);
}

/* -- Public functions -- */

ecs_entity_t _ecs_new(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    ecs_entity_t entity = new_entity_handle(world);

    ecs_assert(!world->max_handle || entity <= world->max_handle, 
        ECS_OUT_OF_RANGE, NULL);

    if (type) {
        ecs_entity_info_t info = {.table = NULL};
        commit(world, stage, entity, &info, type, 0, true);
    }

    return entity;
}

static
bool has_unset_columns(
    ecs_type_t type,
    ecs_column_t *columns,
    ecs_table_data_t *data)
{
    if (!data->columns) {
        return true;
    }

    int32_t i;
    for (i = 0; i < data->column_count; i ++) {
        /* If no column is provided for component, skip it */
        ecs_entity_t component = data->components[i];
        if (component & ECS_ENTITY_FLAGS_MASK) {
            /* If this is a base or parent, don't copy anything */
            continue;
        }

        int32_t column = ecs_type_index_of(type, component);
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);

        int32_t size = columns[column + 1].size;
        if (size) { 
            if (!data->columns[i]) {
                return true;
            }
        }
    }    

    return false;
}

static
void copy_column_data(
    ecs_type_t type,
    ecs_column_t *columns,
    int32_t start_row,
    ecs_table_data_t *data)
{
    int32_t i;
    for (i = 0; i < data->column_count; i ++) {
        /* If no column is provided for component, skip it */
        if (!data->columns[i]) {
            continue;
        }

        ecs_entity_t component = data->components[i];
        if (component & ECS_ENTITY_FLAGS_MASK) {
            /* If this is a base or parent, don't copy anything */
            continue;
        }

        int32_t column = ecs_type_index_of(type, component);
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);

        int32_t size = columns[column + 1].size;
        if (size) { 
            void *column_data = ecs_vector_first(columns[column + 1].data);

            memcpy(
                ECS_OFFSET(column_data, (start_row) * size),
                data->columns[i],
                data->row_count * size
            );
        }
    }
}

static
void invoke_reactive_systems(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t src_type,
    ecs_type_t dst_type,
    ecs_table_t *src_table,
    ecs_column_t *src_columns,
    ecs_table_t *dst_table,
    ecs_column_t *dst_columns,
    int32_t src_index,
    int32_t dst_index,
    int32_t count,
    bool do_set)
{
    ecs_type_t to_remove = NULL; 
    ecs_type_t to_add = NULL;
    
    if (src_type) {
        to_remove = ecs_type_merge_intern(world, stage, src_type, NULL, dst_type);
    } else {
        to_add = dst_type;
    }

    if (dst_type) {
        to_add = ecs_type_merge_intern(world, stage, dst_type, NULL, src_type);
    } else {
        to_remove = src_type;
    }

    /* Invoke OnRemove systems */
    if (to_remove) {
        notify_post_merge(
            world,
            stage,
            src_table,
            src_columns,
            src_index,
            count,
            to_remove);
    }

    if (src_type && src_type != dst_type) {
        /* Delete column from old table. Delete in reverse, as entity indexes of
         * entities after the deletion point change as a result of the delete. */

        int32_t i;
        for (i = 0; i < count; i ++) {
            ecs_table_delete(
                world, stage, src_table, src_columns, src_index + count - i);
        }
    }

    /* Invoke OnAdd systems */
    if (to_add) {
        ecs_entity_info_t dst_info = {
            .row = dst_index + 1,
            .table = dst_table,
            .columns = dst_columns
        };

        notify_after_commit(
            world,
            stage,
            0,
            &dst_info,
            0,
            count,
            to_add,
            do_set);
    }
}

static
int32_t update_entity_index(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_column_t *columns,
    ecs_entity_t start_entity,
    int32_t start_row,
    ecs_table_data_t *data)
{
    bool has_unset = false, tested_for_unset = false;
    int32_t i, dst_start_row = start_row;
    int32_t count = data->row_count;
    ecs_entity_t *entities = ecs_vector_first(columns[0].data);
    int32_t row_count = ecs_vector_count(columns[0].data);

    /* While we're updating the entity index we may need to invoke reactive
     * systems (OnRemove, OnAdd) in case the origin of the entities is not the
     * same. The only moment in time we know the previous type and the new type
     * of the entity is in this function, which is why those systems need to be
     * invoked here.
     *
     * These variables help track the largest contiguous subsets of different
     * origins, so that reactive systems can be inovoked on arrays of entities
     * vs individual entities as much as possible. */
    bool same_origin = true;
    ecs_type_t src_type = NULL, prev_src_type = NULL;
    int32_t src_row = 0, prev_src_row = 0, dst_first_contiguous_row = start_row;
    int32_t src_first_contiguous_row = 0;

    /* Obtain the entity index in the current stage */
    ecs_map_t *entity_index = stage->entity_index;
    ecs_entity_t e;

    /* We need to commit each entity individually in order to populate
     * the entity index */
    for (i = 0; i < count; i ++) {

        /* If existing array with entities was provided, use entity ids from
         * that array. Otherwise use new entity id */
        if(data->entities) {
            e = data->entities[i];

            /* If this is not the first entity, check if the next entity in
             * the table is the next entity to set. If so, there is no need 
             * to update the entity index. This is the fast path that is
             * taken if all entities in the table are in the same order as
             * provided in the data argument. */
            if (i) {
                if (entities) {
                    if (dst_start_row + i < row_count) {
                        if (entities[dst_start_row + i] == e) {
                            continue;
                        }
                    }
                }
            }

            /* Ensure that the last issued handle will always be ahead of the
             * entities created by this operation */
            if (e > world->last_handle) {
                world->last_handle = e + 1;
            }                            
        } else {
            e = i + start_entity;
        }

        ecs_record_t *record_ptr = ecs_map_get(entity_index, ecs_record_t, e);
        if (record_ptr) {
            src_row = record_ptr->row;
            uint8_t is_monitored = 1 - (src_row < 0) * 2;
            src_row = (src_row * is_monitored) - 1;

            ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);

            /* Keep track of whether all entities come from the same origin or
             * not. If they come from the same origin, reactive system(s)
             * can be invoked with all of the entities at the same time. If the
             * existing entities are of different origins however, we try to do
             * the best we can, which is find the largest contiguous subset(s) 
             * of entities from the same origin, and invoke the reactive systems
             * on those subsets. */
            if (!i) {
                /* For the first entity we don't have to do much */
                prev_src_type = record_ptr->type;
            }

            src_type = record_ptr->type;

            /* If entity exists, and this is the first entity being iterated
             * set the start index to the current index of the entity. The
             * start_index indicates the start of where the to be added
             * entities will be stored. 
             * In the ideal scenario, the subsequent entities to be added
             * will be provided in the same order after the first entity, so
             * that the entity index does not need to be updated. */
            if (!i && record_ptr->type == type) {
                dst_start_row = src_row;
                dst_first_contiguous_row = src_row;
            } else {
                /* If the entity exists but it is not the first entity, 
                 * check if it is in the same table. If not, delete the 
                 * entity from the other table */     
                if (record_ptr->type != type) {
                    ecs_table_t *old_table = ecs_world_get_table(
                        world, stage, record_ptr->type);
                    ecs_column_t *old_columns = ecs_table_get_columns(
                        world, stage, old_table);

                    /* Insert new row into destination table */
                    int32_t dst_row = ecs_table_insert(
                        world, table, columns, e) - 1;
                    if (!i) {
                        dst_start_row = dst_row;
                        dst_first_contiguous_row = dst_row;
                        src_first_contiguous_row = src_row;
                    }

                    /* If the data structure has columns that are unset data
                     * must be copied from the old table to the new table */
                    if (!tested_for_unset) {
                        has_unset = has_unset_columns(
                            type, columns, data);
                        tested_for_unset = true;
                    }

                    if (has_unset) {
                        copy_row(type, columns, dst_row + 1, 
                            old_table->type, old_columns, record_ptr->row);
                    }

                    /* Actual deletion of the entity from the source table
                     * happens after the OnRemove systems are invoked */

                /* If entity exists in the same table but not on the right
                 * index (if so, we would've found out by now) we need to
                 * move things around. This will ensure that entities are
                 * ordered in exactly the same way as they are provided in
                 * the data argument, so that subsequent invocations of this
                 * operation with entities in the same order, 
                 * insertion can be much faster. This also allows the data
                 * from columns in data to be inserted with a single
                 * memcpy per column. */
                } else {

                    /* If we're not at the top of the table, simply swap the
                     * next entity with the one that we want at this row. */
                    if (row_count > (dst_start_row + i)) {
                        ecs_table_swap(stage, table, columns, 
                            src_row, dst_start_row + i, record_ptr, NULL);

                    /* We are at the top of the table and the entity is in
                     * the table. This scenario is a bit nasty, since we
                     * need to now move the added entities back one position
                     * and swap the entity preceding them with the current
                     * entity. This should only happen in rare cases, as any
                     * subsequent calls with the same set of entities should
                     * find the entities in the table to be in the right 
                     * order. 
                     * We could just have swapped the order in which the
                     * entities are inserted, but then subsequent calls
                     * would still be expensive, and we couldn't just memcpy
                     * the component data into the columns. */
                    } else {
                        /* First, swap the entity preceding the start of the
                         * added entities with the entity that we want at
                         * the end of the block */
                        ecs_table_swap(stage, table, columns, 
                            src_row, dst_start_row - 1, record_ptr, NULL);

                        /* Now move back the whole block back one position, 
                         * while moving the entity before the start to the 
                         * row right after the block */
                        ecs_table_move_back_and_swap(
                            stage, table, columns, dst_start_row, i);

                        dst_start_row --;
                        dst_first_contiguous_row --;
                    }
                }
            }

            /* Update entity index with the new table / row */
            record_ptr->type = type;
            record_ptr->row = dst_start_row + i + 1;
            record_ptr->row *= is_monitored;
        } else {
            ecs_record_t new_record = (ecs_record_t){
                .type = type, .row = dst_start_row + i + 1
            };

            ecs_map_set(entity_index, e, &new_record);

            if (data->entities) {
                ecs_table_insert(world, table, columns, e);

                /* Entities array may have been reallocated */
                entities = ecs_vector_first(columns[0].data);
            }
        }

        /* Now that the entity index is updated for this entity, check if we
         * need to invoke reactive systems for the current set of contiguous
         * entities. */
        if (i) {
            ecs_assert(dst_first_contiguous_row >= dst_start_row, ECS_INTERNAL_ERROR, NULL);

            if (prev_src_type != src_type || (src_row != prev_src_row && prev_src_row != (src_row - 1))) {
                /* If either the previous type is different from the current
                 * type, or the previous index is not one before the current,
                 * entities are not from the same origin or they are not stored
                 * in a contiguous way. If this happens, invoke reactive systems
                 * up to this point. */
                ecs_table_t *src_table = NULL;
                ecs_column_t *src_columns = NULL;
                
                if (prev_src_type) {
                    src_table = ecs_world_get_table(
                        world, stage, prev_src_type);
                    src_columns = ecs_table_get_columns(
                        world, stage, src_table);
                }

                invoke_reactive_systems(
                    world,
                    stage,
                    prev_src_type,
                    type,
                    src_table,
                    src_columns,
                    table,
                    columns,
                    src_first_contiguous_row,
                    dst_first_contiguous_row,
                    i - dst_start_row - dst_first_contiguous_row,
                    data->columns == NULL);

                /* Start a new contiguous set */
                dst_first_contiguous_row = dst_start_row + i;
                src_first_contiguous_row = src_row;
                prev_src_type = src_type;
                same_origin = false;
            }            
        }

        prev_src_row = src_row;
    }

    /* Invoke reactive systems on the entities in the last contiguous set. If 
     * all entities are from the same origin, this will cover all entities. */
    ecs_table_t *src_table = NULL;
    ecs_column_t *src_columns = NULL;
    int32_t contiguous_count = 0;

    if (same_origin) {
        contiguous_count = count;
    } else {
        contiguous_count = prev_src_row - src_first_contiguous_row + 1;
    }

    if (prev_src_type) {
        src_table = ecs_world_get_table(
            world, stage, prev_src_type);
        src_columns = ecs_table_get_columns(
            world, stage, src_table);
    }

    invoke_reactive_systems(
        world,
        stage,
        prev_src_type,
        type,
        src_table,
        src_columns,
        table,
        columns,
        src_first_contiguous_row,
        dst_first_contiguous_row,
        contiguous_count,
        data->columns == NULL);

    return dst_start_row;
}

static
ecs_entity_t set_w_data_intern(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_table_data_t *data)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_t result;
    int32_t count = data->row_count;

    if (data->entities) {
        result = data->entities[0];
    } else {
        result = world->last_handle + 1;        
        world->last_handle += count;
    }

    ecs_assert(!world->max_handle || world->last_handle <= world->max_handle, 
        ECS_OUT_OF_RANGE, NULL);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    if (type) {
        /* Get table, table columns and grow table to accomodate for new
         * entities */
        ecs_table_t *table = ecs_world_get_table(world, stage, type);
        ecs_column_t *columns = ecs_table_get_columns(
                world, stage, table);
        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, 0);
        int32_t start_row = 0;

        /* Obtain the entity index in the current stage */
        ecs_map_t *entity_index = stage->entity_index;

        /* Grow world entity index only if no entity ids are provided. If ids
         * are provided, it is possible that they already appear in the entity
         * index, in which case they will be overwritten. */
        if (!data->entities) {
            start_row = ecs_table_grow(world, table, columns, count, result) - 1;
            ecs_map_grow(entity_index, count);
        }

        /* Obtain list of entities */
        ecs_entity_t *entities = ecs_vector_first(columns[0].data);

        /* If the entity array is NULL, we can safely allocate space for
         * row_count number of rows, which will give a perf boost the first time
         * the entities are inserted. */
        if (!entities) {
            ecs_table_dim(table, columns, count);
            entities = ecs_vector_first(columns[0].data);
            ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
        }

        /* This is the most complex part of the set_w_data. We need to go from
         * a potentially chaotic state (entities can be anywhere) to a state
         * where all entities are in the same table, in the order specified by
         * the arguments of this function.
         *
         * This function addresses the following cases:
         * - generate new entity ids (when data->entities = NULL)
         * - entities do not yet exist
         * - entities exist, are in the same table, in the same order
         * - entities exist, are in the same table, in a different order
         * - entities exist, are in a different table, in the same order
         * - entities exist, are in a different table, in a different order
         * - entities may exist, and may be in different tables
         *
         * For each of these cases, the proper sequence of OnAdd / OnRemove 
         * systems must be executed.
         */
        start_row = update_entity_index(
            world, stage, type, table, columns, result, start_row, data);    

        /* If columns were provided, copy data from columns into table. This is
         * where a lot of the performance benefits can be achieved: now that all
         * entities are nicely ordered in the destination table, we can copy the
         * data into each column with a single memcpy. */
        if (data->columns) {
            copy_column_data(type, columns, start_row, data);
        }

        /* Invoke OnSet systems */
        if (data->columns) {
            notify_pre_merge(
                world_arg,
                stage,
                table,
                columns,
                start_row,
                data->row_count,
                type,
                world->type_sys_set_index);
        }
    }

    return result;
}

ecs_entity_t _ecs_new_w_count(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_table_data_t table_data = {
        .row_count = count
    };

    return set_w_data_intern(world, type, &table_data);
}

ecs_entity_t ecs_set_w_data(
    ecs_world_t *world,
    ecs_table_data_t *data)
{
    /* Create copy of components array as ecs_type_find orders the array */
    ecs_entity_t *components = ecs_os_alloca(ecs_entity_t, data->column_count);
    memcpy(components, data->components, data->column_count * sizeof(ecs_entity_t));
    ecs_type_t type = ecs_type_find(world, components, data->column_count);
    return set_w_data_intern(world, type, data);
}

ecs_entity_t _ecs_new_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t full_type = type;
    
    if (parent) {
        full_type = ecs_type_add(world, full_type, parent | ECS_CHILDOF);
    }

    return _ecs_new(world, full_type);
}

ecs_entity_t _ecs_new_child_w_count(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_type_t type,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t full_type = type;
    
    if (parent) {
        full_type = ecs_type_add(world, full_type, parent | ECS_CHILDOF);
    }

    return _ecs_new_w_count(world, full_type, count);
}

ecs_entity_t _ecs_new_instance(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t full_type = type;
    
    if (base) {
        full_type = ecs_type_add(world, full_type, base | ECS_INSTANCEOF);
    }

    return _ecs_new(world, full_type);
}

ecs_entity_t _ecs_new_instance_w_count(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_type_t type,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t full_type = type;
    
    if (base) {
        full_type = ecs_type_add(world, full_type, base | ECS_INSTANCEOF);
    }

    return _ecs_new_w_count(world, full_type, count);
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);

    if (stage == &world->main_stage) {
        ecs_entity_info_t info;
        if ((get_info(world, entity, &info))) {
            commit(world, stage, entity, &info, NULL, info.table->type, false);
        }
    } else {
        /* Remove the entity from the staged index. Any added components while
         * in progress will be discarded as a result. */
        ecs_delete_entity(world, stage, entity);
    }
}

void ecs_delete_w_filter_intern(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    bool is_delete)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->main_stage, ECS_UNSUPPORTED, 
        "delete_w_filter currently only supported on main stage");

    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_type_t type = table->type;

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_type_match_w_filter(world, type, filter)) {
            continue;
        }

        /* Remove entities from index */
        ecs_vector_t *entities = table->columns[0].data;
        ecs_entity_t *array = ecs_vector_first(entities);
        int32_t j, row_count = ecs_vector_count(entities);
        for (j = 0; j < row_count; j ++) {
            ecs_map_remove(world->main_stage.entity_index, array[j]);
        }

        /* Both filters passed, clear table */
        if (is_delete) {
            ecs_table_delete_all(world, table);
        } else {
            ecs_table_clear(world, table);
        }
    }
}

void ecs_delete_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_delete_w_filter_intern(world, filter, true);
}

void ecs_clear_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_delete_w_filter_intern(world, filter, false);
}

void _ecs_add_remove_w_filter(
    ecs_world_t *world,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->main_stage, ECS_UNSUPPORTED, 
        "remove_w_filter currently only supported on main stage");

    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_type_t type = table->type;

        /* Skip if the type contains none of the components in to_remove */
        if (to_remove) {
            if (!ecs_type_contains(world, type, to_remove, false, false)) {
                continue;
            }
        }

        /* Skip if the type already contains all of the components in to_add */
        if (to_add) {
            if (ecs_type_contains(world, type, to_add, true, false)) {
                continue;
            }            
        }

        if (!ecs_type_match_w_filter(world, type, filter)) {
            continue;
        }

        /* Component(s) must be removed, find table */
        ecs_type_t dst_type = ecs_type_merge(world, type, to_add, to_remove);
        if (!dst_type) {
            /* If this removes all components, clear table */
            ecs_table_merge(world, NULL, table);
        } else {
            ecs_table_t *dst_table = ecs_world_get_table(world, stage, dst_type);
            ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

            /* Merge table into dst_table */
            ecs_table_merge(world, dst_table, table);
        }
    }    
}

static
ecs_entity_t copy_from_stage(
    ecs_world_t *world,
    ecs_stage_t *src_stage,
    ecs_entity_t src_entity,
    ecs_entity_t dst_entity,
    bool copy_value)
{
    if (!src_entity) {
        return 0;
    }

    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);    

    ecs_entity_info_t src_info;

    if (get_staged_info(world, src_stage, src_entity, &src_info)) {
        ecs_assert(!dst_entity, ECS_INTERNAL_ERROR, NULL);

        dst_entity = ++ world->last_handle;
        ecs_type_t new_type = src_info.table ? src_info.table->type : NULL;

        ecs_entity_info_t info = { 0 };

        commit(world, stage, dst_entity, &info, new_type, 0, false);

        if (copy_value) {
            copy_row(info.table->type, info.columns, info.row,
                src_info.table->type, src_info.columns, src_info.row);

            ecs_notify(
                world_arg, stage, world->type_sys_set_index, 
                info.table->type, info.table, info.columns, info.row - 1, 1);                
        }        
    }

    return dst_entity;
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    ecs_entity_t result = copy_from_stage(world, &world->main_stage, entity, 0, copy_value);

    if (stage != &world->main_stage) {
        result = copy_from_stage(world, stage, entity, result, copy_value);
    }

    if (!result) {
        result = ++ world->last_handle;
    }

    return result;
}

void _ecs_add(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_add_remove_intern(world, entity, type, 0, true);
}

void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add)
{
    ecs_type_t type = ecs_type_from_entity(world, to_add);
    _ecs_add(world, entity, type);
}

void _ecs_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_add_remove_intern(world, entity, 0, type, false);
}

void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add)
{
    ecs_type_t type = ecs_type_from_entity(world, to_add);
    _ecs_remove(world, entity, type);
}

void _ecs_add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t add_type,
    ecs_type_t remove_type)
{
    ecs_add_remove_intern(world, entity, add_type, remove_type, false);
}

void ecs_adopt(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent)
{    
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    ecs_type_t add_type = ecs_type_find(
        world, 
        &(ecs_entity_t){parent | ECS_CHILDOF},
        1);

    _ecs_add_remove(world, entity, add_type, 0);
}

void ecs_orphan(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t remove_type = ecs_type_find(
        world, 
        &(ecs_entity_t){parent | ECS_CHILDOF},
        1);

    _ecs_add_remove(world, entity, 0, remove_type);
}

void ecs_inherit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t base)
{    
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    ecs_type_t add_type = ecs_type_find(
        world, 
        &(ecs_entity_t){base | ECS_INSTANCEOF},
        1);

    _ecs_add_remove(world, entity, add_type, 0);
}

void ecs_disinherit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t base)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t remove_type = ecs_type_find(
        world, 
        &(ecs_entity_t){base | ECS_INSTANCEOF},
        1);

    _ecs_add_remove(world, entity, 0, remove_type);
}

void* _ecs_get_ptr(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_get_ptr_intern(world, stage, entity, component, false, true);
}

static
ecs_entity_t _ecs_set_ptr_intern(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;

    /* If no entity is specified, create one */
    if (!entity) {
        ecs_type_t type = ecs_type_from_entity(world, component);
        entity = _ecs_new(world, type);
    }
    
    void *dst = NULL;
    if (get_or_create_staged_info(world, stage, entity, &info)) {
        dst = get_component(&info, component);
    }

    if (!dst) {
        ecs_type_t to_add = ecs_type_from_entity(world, component);

        commit(world, stage, entity, &info, to_add, NULL, false);

        dst = get_component(&info, component);
        if (!dst) {
            return entity;
        }
    }

    if (ptr) {
        memcpy(dst, ptr, size);
    } else {
        memset(dst, 0, size);
    }

    ecs_type_t component_type = ecs_type_from_entity(world, component);
    notify_pre_merge(
        world_arg, stage, info.table, info.columns, info.row - 1, 1, component_type,
        world->type_sys_set_index);

    return entity;
}

ecs_entity_t _ecs_set_ptr(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    /* If no entity is specified, create one */
    if (!entity) {
        ecs_type_t type = ecs_type_from_entity(world, component);
        entity = _ecs_new(world, type);
    }

    return _ecs_set_ptr_intern(world, entity, component, size, ptr);
}

static
bool ecs_has_intern(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    bool match_any,
    bool match_prefabs)    
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return false;
    }

    if (!type) {
        return true;
    }

    ecs_world_t *world_arg = world;
    ecs_type_t entity_type = ecs_get_type(world_arg, entity);
    return ecs_type_contains(world, entity_type, type, match_any, match_prefabs) != 0;
}

bool _ecs_has(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    return ecs_has_intern(world, entity, type, true, true) != 0;
}

bool _ecs_has_owned(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    return ecs_has_intern(world, entity, type, true, false) != 0;
}

bool _ecs_has_any(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    return ecs_has_intern(world, entity, type, false, true);
}

bool _ecs_has_any_owned(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    return ecs_has_intern(world, entity, type, false, false);
} 

bool ecs_has_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return false;
    }

    if (!component) {
        return true;
    }

    ecs_world_t *world_arg = world;
    ecs_type_t entity_type = ecs_get_type(world_arg, entity);
    return ecs_type_has_entity_intern(world, entity_type, component, true);
}

bool ecs_has_entity_owned(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return false;
    }

    if (!component) {
        return true;
    }

    ecs_world_t *world_arg = world;
    ecs_type_t entity_type = ecs_get_type(world_arg, entity);
    return ecs_type_has_entity_intern(world, entity_type, component, false);
}

bool ecs_contains(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!parent || !child) {
        return false;
    }

    ecs_type_t child_type = ecs_get_type(world, child);

    return ecs_type_has_entity_intern(
        world, child_type, parent | ECS_CHILDOF, false);
}

ecs_entity_t _ecs_get_parent(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_entity_t parent = 0;
    ecs_type_t type = ecs_get_type(world, entity);
    
    ecs_components_contains_component(
        world, type, component, 0, &parent);

    return parent;
}

const char* ecs_get_id(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    if (entity == ECS_SINGLETON) {
        return "$";
    }

    EcsId *id = ecs_get_ptr(world, entity, EcsId);
    if (id) {
        return *id;
    } else {
        return NULL;
    }
}

ecs_type_t ecs_type_from_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return NULL;
    }

    EcsTypeComponent *type = ecs_get_ptr(world, entity, EcsTypeComponent);
    if (type) {
        return type->normalized;
    }

    return ecs_type_find(world, &entity, 1);
}

ecs_entity_t ecs_type_to_entity(
    ecs_world_t *world, 
    ecs_type_t type)
{
    (void)world;

    if (!type) {
        return 0;
    }
    
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
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_record_t *record = get_staged_entity(world, stage, entity);
    ecs_type_t result = record ? record->type : NULL;

    if (world->in_progress) {
        ecs_type_t remove_type = NULL;
        ecs_map_has(stage->remove_merge, entity, &remove_type);
        
        ecs_record_t *main_record = get_entity(world, entity);
        result = ecs_type_merge_intern(
            world, stage, main_record->type, result, remove_type);
    }
    
    return result;
}

int32_t ecs_count_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_sparse_t *tables = world->main_stage.tables;
    int32_t i, count = ecs_sparse_count(tables);
    int32_t result = 0;

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);

        if (!filter || ecs_type_match_w_filter(world, table->type, filter)) {
            result += ecs_vector_count(table->columns[0].data); 
        }
    }
    
    return result;
}

int32_t _ecs_count(
    ecs_world_t *world,
    ecs_type_t type)
{    
    if (!type) {
        return 0;
    }

    return ecs_count_w_filter(world, &(ecs_filter_t){
        .include = type
    });
}

ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    const char *id,
    size_t size)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
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

/* -- Debug functionality -- */

void ecs_dbg_entity(
    ecs_world_t *world, 
    ecs_entity_t entity, 
    ecs_dbg_entity_t *dbg_out)
{
    *dbg_out = (ecs_dbg_entity_t){.entity = entity};
    
    ecs_entity_info_t info = { 0 };
    if (get_info(world, entity, &info)) {
        dbg_out->table = info.table;
        dbg_out->row = info.row;
        dbg_out->is_watched = info.is_watched;
        dbg_out->type = info.table ? info.table->type : NULL;
    }
}
