
#include "flecs_private.h"

static
ecs_entity_t new_w_data_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    int32_t count,
    void **component_data);

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
    ecs_assert(
        ecs_vector_count(type) < ECS_MAX_ENTITIES_IN_TYPE, 
            ECS_TYPE_TOO_LARGE, NULL);

    ecs_data_t *data = info->data;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    if (!columns) {
        return NULL;
    }

    ecs_entity_t *array = ecs_vector_first(type);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        if (array[i] == component) {
            ecs_column_t *component = &columns[i];
            uint32_t size = component->size; 
            ecs_vector_t *data_vec = component->data;

            ecs_assert(!size || data_vec != NULL, ECS_INTERNAL_ERROR, NULL);

            if (data_vec) {
                void *data = ecs_vector_first(data_vec);
                ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
                return ECS_OFFSET(data, row * size);
            }
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

int32_t ecs_record_to_row(
    int32_t row, 
    bool *is_watched_out) 
{
    bool is_watched = row < 0;
    row = row * -(is_watched * 2 - 1) - 1 * (row != 0);
    *is_watched_out = is_watched;
    return row;
}

int32_t ecs_row_to_record(
    int32_t row, 
    bool is_watched) 
{
    return (row + 1) * -(is_watched * 2 - 1);
}

/* Utility to compute actual row from row in record */
static
int32_t set_row_info(
    ecs_entity_info_t *info,
    int32_t row)
{
    return info->row = ecs_record_to_row(row, &info->is_watched);
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

    ecs_table_t *table = record->table;

    set_row_info(info, record->row);

    info->table = table;
    if (!info->table) {
        return;
    }

    ecs_data_t *data = ecs_table_get_data(world, table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    info->data = data;

    ecs_assert(info->row < ecs_vector_count(data->entities), 
        ECS_INTERNAL_ERROR, NULL);
}

/* Get info from main stage */
static
bool get_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_record_t *record = ecs_eis_get(&world->stage, entity);
    if (!record) {
        info->table = NULL;
        info->is_watched = false;
        info->record = NULL;
        return false;
    }

    set_info_from_record(world, info, record);

    return true;
}

/* Get info from any stage but main stage */
static
bool get_staged_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_get(stage, entity);
    if (!record) {
        info->table = NULL;
        info->is_watched = false;
        info->record = NULL;
        return false;
    }

    int32_t row = set_row_info(info, record->row);    

    ecs_table_t *table = record->table;

    info->table = table;
    if (!info->table) {
        info->record = NULL;
        return true;
    }

    ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
    info->data = data;

    ecs_assert(data->record_ptrs != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(data->record_ptrs) >= row, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t**record_ptrs = ecs_vector_first(data->record_ptrs);

    info->record = record_ptrs[row];

    return true;
}

/* Get entity info */
bool ecs_get_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    if (stage == &world->stage) {
        return get_info(world, entity, info);
    } else {
        if (!get_staged_info(world, stage, entity, info)) {
            return get_info(world, entity, info);
        }

        return true;
    }    
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
void override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_column_t *column,
    uint32_t row,
    uint32_t count);

static
void override_from_base(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t component,
    ecs_column_t *column,
    uint32_t row,
    uint32_t count)
{
    ecs_entity_info_t base_info;
    if (!get_info(world, base, &base_info)) {
        return;
    }

    void *base_ptr = get_component(&base_info, component);
    if (base_ptr) {
        uint32_t data_size = column->size;
        void *data_array = ecs_vector_first(column->data);
        void *data_ptr = ECS_OFFSET(data_array, data_size * row);

        uint32_t i;
        for (i = 0; i < count; i ++) {
            memcpy(data_ptr, base_ptr, data_size);
            data_ptr = ECS_OFFSET(data_ptr, data_size);
        }
    } else {
        /* If component not found on base, check if base itself inherits */
        ecs_type_t base_type = base_info.table->type;
        override_component(world, component, base_type, column, row, count);
    }
}

static
void override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_column_t *column,
    uint32_t row,
    uint32_t count)
{
    ecs_entity_t *type_array = ecs_vector_first(type);
    int32_t i, type_count = ecs_vector_count(type);

    /* Walk prefabs */
    i = type_count - 1;
    do {
        ecs_entity_t e = type_array[i];

        if (e < ECS_ENTITY_FLAGS_START) {
            break;
        }

        if (e & ECS_INSTANCEOF) {
            override_from_base(
                world, e & ECS_ENTITY_MASK, component, column, row, count);
        }
    } while (--i >= 0);
}

static
void instantiate_children(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_t *child_table)
{
    ecs_type_t type = child_table->type;
    int32_t type_count = ecs_vector_count(type);
    ecs_entity_t *type_array = ecs_vector_first(type);

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_entities_t components = {
        .array = ecs_os_alloca(type_count, sizeof(ecs_entity_t)),
        .count = type_count
    };

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int i, base_index;
    for (i = 0; i < type_count; i ++) {
        ecs_entity_t c = type_array[i];
        components.array[i] = c;
        if (c & ECS_CHILDOF && (c & ECS_ENTITY_MASK) == base) {
            base_index = i;
        }
    }

    /* Prepare component data array */
    void **component_data = NULL;
    ecs_data_t *child_data = ecs_table_get_data(world, child_table);
    ecs_assert(child_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (child_data->columns) {
        component_data = ecs_os_alloca(void*, type_count);
        for (i = 0; i < type_count; i ++) {
            component_data[i] = ecs_vector_first(child_data->columns[i].data);
        }   
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities);
    int32_t entity_count = ecs_vector_count(data->entities);
    int32_t child_count = ecs_vector_count(child_data->entities);

    /* Instantiate children for each instance */
    for (i = 0; i < entity_count; i ++) {
        ecs_entity_t e = entities[i];
        components.array[base_index] = ECS_CHILDOF | e;

        /* Find or create table */
        ecs_table_t *table = ecs_table_find_or_create(
            world, stage, &components);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create children */
        new_w_data_intern(world, stage, table, child_count, component_data);
    }
}

static
void instantiate(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    /* If base is a parent, instantiate children of base for instances */
    const EcsParent *parent_data = ecs_get_ptr(world, base, EcsParent);
    if (!parent_data) {
        return;
    }

    int t, table_count = ecs_vector_count(parent_data->child_tables);
    ecs_table_t **tables = ecs_vector_first(parent_data->child_tables);

    for (t = 0; t < table_count; t ++) {
        ecs_table_t *child_table = tables[t];
        if (!ecs_table_count(child_table)) {
            continue;
        }

        instantiate_children(
            world, stage, base, table, data, row, count, child_table);
    }
}

static
void run_row_systems(
    ecs_world_t *world,
    ecs_vector_t *systems,
    ecs_table_t *table,
    ecs_data_t *data,
    uint32_t row,
    uint32_t count)
{
    uint32_t sys_count = ecs_vector_count(systems);
    ecs_entity_t *sys_array = ecs_vector_first(systems);

    bool in_progress = ecs_staging_begin(world);

    int s;
    for (s = 0; s < sys_count; s++) {
        ecs_run_row_system(
            world, sys_array[s], table, data, row, count);
    }

    ecs_staging_end(world, in_progress);
}

static
void run_component_set_action(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_component_data_t* cdata,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    uint32_t row,
    uint32_t count)
{
    ecs_vector_t *systems = cdata->on_set;   
    if (systems) {
        run_row_systems(world, systems, table, data, row, count);
    }
}

void ecs_run_init_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    uint32_t row,
    uint32_t count,
    ecs_entities_t components)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Array that contains component callbacks & systems */
    ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data);
    ecs_column_t *component_columns = data->columns;
    bool has_base = table->flags & EcsTableHasPrefab;
    ecs_type_t type;
    uint32_t type_count;
    ecs_entity_t *type_array; 

    if (components.count) {
        type = table->type;
        type_count = ecs_vector_count(type);
        type_array = ecs_vector_first(type);
    }

    int i, cur;
    for (i = 0; i < components.count; i ++) {
        /* Retrieve component callbacks & systems for component */
        ecs_entity_t component = components.array[i];

        if (component >= ECS_HI_COMPONENT_ID) {
            if (component & ECS_INSTANCEOF) {
                ecs_entity_t base = component & ECS_ENTITY_MASK;
                instantiate(world, stage, base, table, data, row, count);
            }
            continue;
        }
    
        ecs_component_data_t *cdata = &cdata_array[component];
        void *ctx = cdata->lifecycle.ctx;
        
        ecs_xtor_t ctor = cdata->lifecycle.ctor;
        ecs_vector_t *systems = cdata->on_add;
        if (!ctor && !systems && !has_base) {
            continue;
        }

        /* Find column index of current component */
        cur = 0;
        while (type_array[cur] != component) {
            cur ++;
        }

        /* Removed components should always be in the old type */
        ecs_assert(cur <= type_count, ECS_INTERNAL_ERROR, NULL);

        /* Get column and pointer to data */
        ecs_column_t *column = &component_columns[cur];
        int32_t size = column->size;
        void *array = ecs_vector_first(column->data);
        void *ptr = ECS_OFFSET(array, size * row);

        if (ctor) {
            ctor(world, component, ptr, size, count, ctx);
        }

        if (has_base && component != EEcsParent) {
            override_component(world, component, type, column, row, count);
            run_component_set_action(
                world, stage, cdata, component, table, data, row, count);
        }

        if (systems) {
            run_row_systems(world, systems, table, data, row, count);
            run_component_set_action(
                world, stage, cdata, component, table, data, row, count);
        }
    }
}

void ecs_run_deinit_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    uint32_t row,
    uint32_t count,
    ecs_entities_t components)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Array that contains component callbacks & systems */
    ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data);
    ecs_column_t *component_columns = data->columns;
    ecs_type_t type;
    uint32_t type_count;
    ecs_entity_t *type_array; 

    if (components.count) {
        type = table->type;
        type_count = ecs_vector_count(type);
        type_array = ecs_vector_first(type);
    }

    int i, cur;
    for (i = 0; i < components.count; i ++) {
        /* Retrieve component callbacks & systems for component */
        ecs_entity_t component = components.array[i];
        if (component >= ECS_HI_COMPONENT_ID) {
            continue;
        }

        ecs_component_data_t *cdata = &cdata_array[component];
        void *ctx = cdata->lifecycle.ctx;
        ecs_xtor_t dtor = cdata->lifecycle.dtor;
        ecs_vector_t *systems = cdata->on_remove;
        
        if (!dtor && !systems) {
            continue;
        }

        /* Find column index of current component */
        cur = 0;
        while (type_array[cur] != component) {
            cur ++;
        }

        /* Removed components should always be in the old type */
        ecs_assert(cur <= type_count, ECS_INTERNAL_ERROR, NULL);

        /* Get column and pointer to data */
        ecs_column_t *column = &component_columns[cur];
        int32_t size = column->size;
        void *array = ecs_vector_first(column->data);
        void *ptr = ECS_OFFSET(array, size * row);

        if (systems) {
            run_row_systems(world, systems, table, data, row, count);
        }

        if (dtor) {
            dtor(world, component, ptr, size, count, ctx);
        }
    }
}

static
uint32_t new_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *new_table,
    ecs_entities_t *added)
{
    ecs_record_t *record = info->record;
    ecs_data_t *dst_data = ecs_table_get_or_create_data(world, stage, new_table);
    uint32_t new_row;

    ecs_assert(added->count != 0, ECS_INTERNAL_ERROR, NULL);

    if (stage == &world->stage) {
        if (!record) {
            record = ecs_eis_get_or_create(stage, entity);
        }

        new_row = ecs_table_append(
            world, new_table, dst_data, entity, record);

        record->table = new_table;
        record->row = new_row + 1;
    } else {
        new_row = ecs_table_append(
            world, new_table, dst_data, entity, NULL);  

        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = new_table,
            .row = new_row + 1
        });
    }

    ecs_assert(
        ecs_vector_count(dst_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_run_init_actions(
        world, stage, new_table, dst_data, new_row, 1, *added);

    ecs_vector_t *new_systems = new_table->on_new;
    if (new_systems) {
        int32_t i, count = ecs_vector_count(new_systems);
        ecs_entity_t *sys_array = ecs_vector_first(new_systems);

        for (i = 0; i < count; i ++) {
            ecs_run_row_system(
                world, sys_array[i], new_table, dst_data, new_row, 1);
        }
    }

    info->data = dst_data;
    
    return new_row;
}

static
uint32_t move_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_table_t *dst_table,
    ecs_data_t *dst_data,
    ecs_entities_t *added,
    ecs_entities_t *removed)
{    
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data[0].entities) > src_row, ECS_INTERNAL_ERROR, NULL);

    bool main_stage = stage == &world->stage;
    ecs_record_t *record = info->record;
    ecs_assert(!record || record == ecs_eis_get(&world->stage, entity), ECS_INTERNAL_ERROR, NULL);
    int32_t dst_row = ecs_table_append(world, dst_table, dst_data, entity, record);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        ecs_table_move(
            dst_table, dst_data, dst_row, src_table, src_data, src_row);
    }

    if (removed) {
        ecs_run_deinit_actions(
            world, stage, src_table, src_data, src_row, 1, *removed);
    }

    /* Only delete from source table if it is in the same stage */
    if (src_data->stage == stage) {
        ecs_table_delete(world, stage, src_table, src_data, src_row);
    }

    if (main_stage) {
        record->table = dst_table;
        record->row = dst_row + 1;
    } else {
        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = dst_table,
            .row = dst_row + 1
        });
    }

    if (added) {
        ecs_run_init_actions(
            world, stage, dst_table, dst_data, dst_row, 1, *added);
    }

    info->data = dst_data;

    return dst_row;
}

static
void delete_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_entities_t *removed)
{
    if (removed) {
        ecs_run_deinit_actions(
            world, stage, src_table, src_data, src_row, 1, *removed);
    }

    ecs_table_delete(world, stage, src_table, src_data, src_row);
}

/** Commit an entity with a specified type to a table (probably the most 
 * important function in flecs). */
static
bool commit(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *to_add,
    ecs_entities_t *to_remove,
    bool do_set)
{
    ecs_table_t *dst_table = NULL, *src_table;
    int32_t dst_row = 0;
    bool in_progress = world->in_progress;

    /* Keep track of components that are actually added/removed */
    ecs_entities_t added, removed;
    ecs_entities_t *to_add_ptr = NULL, *to_remove_ptr = NULL;
    ecs_entities_t *added_ptr = NULL, *removed_ptr = NULL;

    if (to_add) {
        to_add_ptr = to_add;
        added_ptr = &added;
        added.array = ecs_os_alloca(ecs_entity_t, to_add->count);
        added.count = 0;
    }

    /* Find the new table */
    src_table = info->table;

    if (src_table) {
        ecs_data_t *src_data = info->data;

        if (to_remove) {
            to_remove_ptr = to_remove;
            removed_ptr = &removed;            

            removed.array = ecs_os_alloca(ecs_entity_t, to_remove->count);
            removed.count = 0;

            dst_table = ecs_table_traverse(
                world, stage, src_table, to_add_ptr, to_remove_ptr, added_ptr, removed_ptr);
        } else {
            dst_table = ecs_table_traverse(
                world, stage, src_table, to_add_ptr, NULL, added_ptr, NULL);
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Only move entity when it is not moved to the root table, unless we're
         * iterating. In this case the entities need to be kept around so that
         * the merge knows to remove them from their previous tables. */
        if (dst_table->type || in_progress) {            
            ecs_data_t *dst_data = ecs_table_get_or_create_data(
                world, stage, dst_table);

            if (src_data != dst_data) {
                dst_row = move_entity(
                    world, stage, entity, info, src_table, src_data, 
                    info->row, dst_table, dst_data, added_ptr, removed_ptr);
            } else {
                return false;
            }

            info->table = dst_table;
            info->row = dst_row;            
        } else {
            delete_entity(
                world, stage, entity, src_table, src_data, info->row, 
                removed_ptr);

            ecs_eis_set(stage, entity, &(ecs_record_t){
                NULL, 
                -info->is_watched 
            });
        }

        /* If the entity is being watched, it is being monitored for changes and
        * requires rematching systems when components are added or removed. This
        * ensures that systems that rely on components from containers or prefabs
        * update the matched tables when the application adds or removes a 
        * component from, for example, a container. */
        if (info->is_watched) {
            world->should_match = true;
        }        
    } else {
        dst_table = ecs_table_traverse(
            world, stage, &world->stage.root, to_add_ptr, NULL, added_ptr, NULL);
        
        if (dst_table->type) {
            dst_row = new_entity(world, stage, entity, info, dst_table, &added);

            info->table = dst_table;
            info->row = dst_row;            
        }        
    }

    if (!in_progress) {
        /* Entity ranges are only checked when not iterating. It is allowed to
         * modify entities that existed before setting the range, and thus the
         * range checks are only applied if the src_table is NULL, meaning the
         * entity did not yet exist/was empty. When iterating, src_table refers
         * to a table in the data stage, not to the table in the main stage.
         * Therefore it is not possible to check while in progress if the entity
         * already existed. Instead, the check will be applied when the entity
         * is merged, which will invoke commit again. */
        if (stage->range_check_enabled) {
            ecs_assert(!world->max_handle || entity <= world->max_handle, ECS_OUT_OF_RANGE, 0);
            ecs_assert(entity >= world->min_handle, ECS_OUT_OF_RANGE, 0);
        }
    }

    return true;
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

    if (stage != &world->stage) {
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
            if (component != EEcsName && component != EEcsPrefab) {
                ptr = get_base_component(
                    world, stage, entity, &info, 0, component);
            }
        }        
    }

    return ptr;
}

void ecs_set_watch(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{    
    (void)world;

    ecs_record_t *record = ecs_eis_get(stage, entity);
    if (!record) {
        ecs_record_t new_record = {.row = -1, .table = NULL};
        ecs_eis_set(stage, entity, &new_record);
    } else {
        if (record->row > 0) {
            record->row *= -1;

        } else if (record->row == 0) {
            /* If entity is empty, there is no index to change the sign of. In
             * this case, set the index to -1, and assign an empty type. */
            record->row = -1;
            record->table = NULL;
        }
    }
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
    ecs_entities_t *to_add,
    ecs_entities_t *to_remove,
    bool do_set)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    commit(world, stage, entity, &info, to_add, to_remove, do_set);
}

static
void new_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entities_t *to_add)
{
    ecs_entities_t added = {
        .array = ecs_os_alloca(ecs_entity_t, to_add->count)
    };
    ecs_entity_info_t info = {0};
    ecs_table_t *table = ecs_table_traverse(
        world, stage, &world->stage.root, to_add, NULL, &added, NULL);
    new_entity(world, stage, entity, &info, table, &added);
}

ecs_entity_t _ecs_new(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);    
    ecs_entity_t entity = new_entity_handle(world);

    if (type) {
        ecs_entities_t to_add = {
            .array = ecs_vector_first(type),
            .count = ecs_vector_count(type)
        };

        new_intern(world, stage, entity, &to_add);
    }

    return entity;
}

static
ecs_entity_t new_w_data_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    int32_t count,
    void **component_data)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);
        
    ecs_type_t type = table->type;
    ecs_entity_t e = world->last_handle + 1;
    world->last_handle += count;

    if (!type) {
        return e;
    }

    ecs_data_t *data = ecs_table_get_or_create_data(world, stage, table);

    int32_t row = ecs_table_grow(world, table, data, count, e);

    ecs_entities_t added = {
        .array = ecs_vector_first(type),
        .count = ecs_vector_count(type)
    };
    
    /* Update entity index. If entities are being created in the main stage,
     * set the record pointers in the table. */
    int i;
    if (stage == &world->stage) {
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs);
        for (i = 0; i < count; i ++) {
            record_ptrs[row + i] = ecs_eis_set(stage, e + i, &(ecs_record_t){
                .table = table,
                .row = row + i + 1
            });
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_eis_set(stage, e + i, &(ecs_record_t){
                .table = table,
                .row = row + i + 1
            });
        }
    }

    ecs_run_init_actions(
        world, stage, table, data, row, count, added);

    /* If data was provided, copy each of the columns into the new table */
    if (component_data) {
        int column_count = ecs_vector_count(type);
        for (i = 0; i < column_count; i ++) {
            /* If this is not a component, skip */
            ecs_entity_t component = added.array[i];
            if (component >= ECS_HI_COMPONENT_ID) {
                continue;
            }

            /* Bulk copy column data into new table */
            ecs_column_t *column = &data->columns[i];
            int32_t size = column->size;
            void *ptr = ecs_vector_first(column->data);
            ptr = ECS_OFFSET(ptr, size * row);
            memcpy(ptr, component_data[i], size * count);

            /* Invoke OnSet systems */
            ecs_component_data_t *cdata = ecs_get_component_data(
                world, component);
            run_component_set_action(
                world, stage, cdata, component, table, data, row, count);
        }
    }

    return e;
}

ecs_entity_t _ecs_new_w_count(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    return new_w_data_intern(world, stage, table, count, NULL);
}

ecs_entity_t _ecs_new_w_data(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count,
    void** data)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    return new_w_data_intern(world, stage, table, count, data);
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
    ecs_entity_info_t info;
    info.table = NULL;

    if (stage == &world->stage) {
        get_info(world, entity, &info);
    } else {
        if (!get_staged_info(world, stage, entity, &info)) {
            get_info(world, entity, &info);
        }
    }

    /* If entity has components, remove them */
    ecs_table_t *table = info.table;
    if (table) {
        ecs_type_t type = table->type;

        /* Remove all components */
        ecs_entities_t to_remove = {
            .array = ecs_vector_first(type),
            .count = ecs_vector_count(type)
        };

        commit(world, stage, entity, &info, NULL, &to_remove, false);
    }

    /* If this is a staged delete, set the table in the staged entity index to
     * NULL. That way the merge will know to delete this entity vs. just to
     * remove its components */
    if (stage != &world->stage) {
        ecs_eis_set(stage, entity, &(ecs_record_t){ NULL, 0 });
    }
}

void ecs_delete_w_filter_intern(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    bool is_delete)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, 
        "delete_w_filter currently only supported on main stage");

    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        /* Remove entities from index */
        ecs_data_t *data = ecs_table_get_data(world, table);
        if (!data) {
            /* If table has no data, there's nothing to delete */
            continue;
        }

        ecs_vector_t *entities = NULL;
        if (data) {
            entities = data->entities;
        }

        ecs_entity_t *array = ecs_vector_first(entities);
        int32_t j, row_count = ecs_vector_count(entities);

        for (j = 0; j < row_count; j ++) {
            ecs_eis_delete(&world->stage, array[j]);
        }

        /* Both filters passed, clear table */
        if (is_delete) {
            ecs_table_clear(world, table);
        } else {
            ecs_table_clear_silent(world, table);
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

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, 
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

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        ecs_entities_t to_add_components = {
            .array = ecs_vector_first(to_add),
            .count = ecs_vector_count(to_add)
        };

        ecs_entities_t to_remove_components = {
            .array = ecs_vector_first(to_remove),
            .count = ecs_vector_count(to_remove)
        };

        ecs_table_t *dst_table = ecs_table_traverse(
            world, stage, table, &to_add_components, &to_remove_components,
            NULL, NULL);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   

        if (!dst_table->type) {
            /* If this removes all components, clear table */
            ecs_table_clear(world, table);
        } else {
            /* Merge table into dst_table */
            if (dst_table != table) {
                ecs_table_merge(world, dst_table, table);
            }
        }
    }    
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t src,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    ecs_entity_t dst = ++ world->last_handle;

    ecs_entity_info_t src_info;
    bool found = ecs_get_info(world, stage, src, &src_info);
    ecs_table_t *src_table = src_info.table;

    if (!found || !src_table) {
        return dst;
    }

    ecs_type_t src_type = src_table->type;
    ecs_entities_t to_add = {
        .array = ecs_vector_first(src_type),
        .count = ecs_vector_count(src_type)
    };

    ecs_entity_info_t dst_info = {0};
    dst_info.row = new_entity(world, stage, dst, &dst_info, src_table, &to_add);

    if (copy_value) {
        ecs_table_move(src_table, dst_info.data, dst_info.row,
            src_table, src_info.data, src_info.row);
    }    

    return dst;
}

void _ecs_add(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = {
        .array = ecs_vector_first(type),
        .count = ecs_vector_count(type)
    };

    ecs_add_remove_intern(world, entity, &components, NULL, true);
}

void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add)
{
    ecs_entities_t components = {
        .array = &to_add,
        .count = 1
    };

    ecs_add_remove_intern(world, entity, &components, NULL, true);
}

void _ecs_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = {
        .array = ecs_vector_first(type),
        .count = ecs_vector_count(type)
    };
    ecs_add_remove_intern(world, entity, NULL, &components, false);
}

void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_remove)
{
    ecs_entities_t components = {
        .array = &to_remove,
        .count = 1
    };
    ecs_add_remove_intern(world, entity, NULL, &components, true);
}

void _ecs_add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_entities_t components_add = {
        .array = ecs_vector_first(to_add),
        .count = ecs_vector_count(to_add)
    };      
    ecs_entities_t components_remove = {
        .array = ecs_vector_first(to_remove),
        .count = ecs_vector_count(to_remove)
    };      
    ecs_add_remove_intern(
        world, entity, &components_add, &components_remove, false);
}

void ecs_add_childof(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent)
{    
    ecs_add_entity(world, entity, ECS_CHILDOF | parent);
}

void ecs_remove_childof(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent)
{
    ecs_remove_entity(world, entity, ECS_CHILDOF | parent);
}

void ecs_add_instanceof(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t base)
{    
    ecs_add_entity(world, entity, ECS_INSTANCEOF | base);
}

void ecs_remove_instanceof(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t base)
{
    ecs_remove_entity(world, entity, ECS_INSTANCEOF | base);
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

void* _ecs_get_cached_ptr(
    ecs_world_t *world,
    ecs_cached_ptr_t *cached_ptr,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info = {0};
    bool found = ecs_get_info(world, stage, entity, &info) && (info.table != NULL);

    if (found) {
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, info.table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

        int32_t count = ecs_vector_count(data->entities);

        if (stage == cached_ptr->stage) {
            if (info.table == cached_ptr->table && count == cached_ptr->count) {
                if (info.row == cached_ptr->row) {
                    return cached_ptr->ptr;
                }
            }
        }

        cached_ptr->stage = stage;
        cached_ptr->table = info.table;
        cached_ptr->count = count;
        cached_ptr->row = info.row;
        cached_ptr->ptr = get_component(&info, component);
    } else {
        cached_ptr->stage = NULL;
        cached_ptr->table = NULL;
        cached_ptr->count = 0;
        cached_ptr->row = 0;
        cached_ptr->ptr = NULL;
    }

    return cached_ptr->ptr;
}

void* _ecs_get_mutable(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    bool *is_added)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info;
    void *dst = NULL;

    /* Check if data is available in current stage */
    if (stage == &world->stage) {
        if (get_info(world, entity, &info)) {
            dst = get_component(&info, component);
        }
    } else {
        if (get_staged_info(world, stage, entity, &info)) {
            dst = get_component(&info, component);
        } else {
            /* If the entity isn't store din the current stage, we still need to
             * get the data from the main stage to pass it to commit */
            get_info(world, entity, &info);
        }
    }

    ecs_table_t *table = info.table;

    if (!dst) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        commit(world, stage, entity, &info, &to_add, NULL, false);
        dst = get_component(&info, component);

        if (is_added) {
            *is_added = table != info.table;
        }

        return dst;
    } else {
        if (is_added) {
            *is_added = false;
        }
        return dst;
    }
}

void _ecs_modified(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;

    ecs_get_info(world, stage, entity, &info);

    ecs_component_data_t *cdata = ecs_get_component_data(world, component);    
    
    run_component_set_action(world, stage, cdata, component, 
        info.table, info.data, info.row, 1);   
}

ecs_entity_t _ecs_set_ptr(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entities_t to_set = {
        .array = &component,
        .count = 1
    };

    /* If no entity is specified, create one */
    if (!entity) {
        entity = new_entity_handle(world);
        new_intern(world, stage, entity, &to_set);
    }
    
    ecs_entity_info_t info;
    void *dst = NULL;

    /* Check if data is available in current stage */
    if (stage == &world->stage) {
        if (get_info(world, entity, &info)) {
            dst = get_component(&info, component);
        }
    } else {
        if (get_staged_info(world, stage, entity, &info)) {
            dst = get_component(&info, component);
        } else {
            /* If the entity isn't store din the current stage, we still need to
             * get the data from the main stage to pass it to commit */
            get_info(world, entity, &info);
        }
    }

    if (!dst) {
        commit(world, stage, entity, &info, &to_set, NULL, false);

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

    ecs_component_data_t *cdata = ecs_get_component_data(world, component);    
    
    run_component_set_action(world, stage, cdata, component, 
        info.table, info.data, info.row, 1);

    return entity;
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

const char* ecs_get_name(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    if (entity == EcsSingleton) {
        return "$";
    }

    EcsName *id = ecs_get_ptr(world, entity, EcsName);

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
    ecs_record_t *record = NULL;

    if (stage != &world->stage) {
        record = ecs_eis_get(stage, entity);
    }

    if (!record) {
        record = ecs_eis_get(&world->stage, entity);
    }

    ecs_table_t *table;
    if (record && (table = record->table)) {
        return table->type;
    }
    
    return NULL;
}

int32_t ecs_count_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);
    int32_t result = 0;

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);

        if (!filter || ecs_table_match_filter(world, table, filter)) {
            result += ecs_table_count(table);
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
    ecs_set(world, result, EcsName, {id});

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

bool ecs_get_watched(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_entity_info_t info = { 0 };
    if (get_info(world, entity, &info)) {
        return info.is_watched;
    } else {
        return false;
    }
}