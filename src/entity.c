
#include "flecs_private.h"

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
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

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

static
int32_t convert_record_row(int32_t row, bool *is_watched_out) {
    bool is_watched = row < 0;
    row = row * -(is_watched * 2 - 1) - 1 * (row != 0);
    *is_watched_out = is_watched;
    return row;
}

/* Utility to compute actual row from row in record */
static
int32_t set_row_info(
    ecs_entity_info_t *info,
    int32_t row)
{
    return info->row = convert_record_row(row, &info->is_watched);
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
    ecs_record_t *record = ecs_eis_get_or_create(&world->stage, entity);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

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
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_get(stage, entity);
    if (!record) {
        info->table = NULL;
        info->is_watched = false;
        return false;
    }

    int32_t row = set_row_info(info, record->row);    

    ecs_table_t *table = record->table;

    info->table = table;
    if (!info->table) {
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
    if (stage != &world->stage) {
        if (get_staged_info(world, stage, entity, info)) {
            return true;
        }

        return false;
    } else {
        get_or_create_info(world, entity, info);
        return true;
    }
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

    int s;
    for (s = 0; s < sys_count; s++) {
        ecs_run_row_system(
            world, sys_array[s], table, data, row, count);
    }
}

static
ecs_component_data_t *get_component_data(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(component < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
    ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data);
    return &cdata_array[component];
}

static
void run_component_set_action(
    ecs_world_t *world,
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

void ecs_run_component_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    uint32_t row,
    uint32_t count,
    ecs_entities_t components,
    bool is_init)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Array that contains component callbacks & systems */
    ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data);
    ecs_column_t *component_columns = data->columns;
    ecs_type_t type = NULL;
    uint32_t type_count;
    ecs_entity_t *type_array;
    bool has_base = table->flags & EcsTableHasPrefab;

    int i, cur = 0;
    for (i = 0; i < components.count; i ++) {
        /* Retrieve component callbacks & systems for component */
        ecs_entity_t component = components.array[i];


        if (component >= ECS_HI_COMPONENT_ID) {
            continue;
        }

        ecs_component_data_t *cdata = &cdata_array[component];
        ecs_vector_t *systems;
        ecs_init_t init = NULL;
        ecs_init_t fini = NULL;
        void *ctx;
        
        if (is_init) {
            init = cdata->init;
            systems = cdata->on_add;
            if (!init && !systems && !has_base) {
                continue;
            }
            ctx = cdata->ctx;
        } else {
            fini = cdata->fini;
            systems = cdata->on_remove;
            if (!fini && !systems) {
                continue;
            }
            ctx = cdata->ctx;
        }

        if (!type) {
            type = table->type;
            type_count = ecs_vector_count(type);
            type_array = ecs_vector_first(type);
        }

        /* Find column index of current component */
        while (type_array[cur] != component) {
            cur ++;
        }

        /* Removed components should always be in the old type */
        ecs_assert(cur <= type_count, ECS_INTERNAL_ERROR, NULL);

        /* Get column and pointer to data */
        ecs_column_t *column = &component_columns[cur];
        void *array = ecs_vector_first(column->data);
        void *ptr = ECS_OFFSET(array, column->size * row);

        if (is_init) {
            if (init) {
                init(ptr, ctx);
            }

            if (has_base) {
                override_component(world, component, type, column, row, count);
                run_component_set_action(
                    world, cdata, component, table, data, row, count);
            }
        }

        if (systems) {
            run_row_systems(world, systems, table, data, row, count);

            if (is_init) {
                run_component_set_action(
                    world, cdata, component, table, data, row, count);
            }
        }

        if (fini) {
            fini(ptr, ctx);
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
    ecs_data_t *dst_data = ecs_table_get_staged_data(world, stage, new_table);
    uint32_t new_row;

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

    ecs_run_component_actions(
        world, stage, new_table, dst_data, new_row, 1, *added, true);

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
    ecs_entities_t *added,
    ecs_entities_t *removed)
{    
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data[0].entities) > src_row, ECS_INTERNAL_ERROR, NULL);
    
    ecs_record_t *record = info->record;
    ecs_data_t *dst_data = ecs_table_get_staged_data(world, stage, dst_table);
    int32_t dst_row = ecs_table_append(world, dst_table, dst_data, entity, record);
    ecs_assert(ecs_vector_count(dst_data[0].entities) > dst_row, ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    ecs_table_move(dst_table, dst_data, dst_row, src_table, src_data, src_row);

    if (removed) {
        ecs_run_component_actions(
            world, stage, src_table, src_data, src_row, 1, *removed, false);
    }

    ecs_table_delete(world, stage, src_table, src_data, src_row);

    if (stage == &world->stage) {
        record->table = dst_table;
        record->row = dst_row + 1;
    } else {
        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = dst_table,
            .row = dst_row + 1
        });
    }

    if (added) {
        ecs_run_component_actions(
            world, stage, dst_table, dst_data, dst_row, 1, *added, true);
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
        ecs_run_component_actions(
            world, stage, src_table, src_data, src_row, 1, *removed, false);
    }

    ecs_table_delete(world, stage, src_table, src_data, src_row);
    ecs_eis_delete(stage, entity);
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

        if (dst_table) {
            if (dst_table != src_table) {
                dst_row = move_entity(
                    world, stage, entity, info, src_table, info->data, info->row, 
                    dst_table, added_ptr, removed_ptr);
            } else {
                return false;
            }

            info->table = dst_table;
            info->row = dst_row;            
        } else {
            delete_entity(
                world, stage, entity, src_table, info->data, info->row, 
                removed_ptr);
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
        
        if (dst_table) {
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
            if (component != EEcsId && component != EEcsPrefab) {
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
    if (!get_or_create_staged_info(world, stage, entity, &info)) {
        info.table = NULL;
    }

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
    ecs_data_t *data,
    int32_t start_row,
    ecs_table_data_t *user_data)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < user_data->column_count; i ++) {
        /* If no column is provided for component, skip it */
        if (!user_data->columns[i]) {
            continue;
        }

        ecs_entity_t component = user_data->components[i];
        if (component & ECS_ENTITY_FLAGS_MASK) {
            /* If this is a base or parent, don't copy anything */
            continue;
        }

        int32_t column = ecs_type_index_of(type, component);
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);

        int32_t size = columns[column].size;
        if (size) { 
            void *column_data = ecs_vector_first(columns[column].data);

            memcpy(
                ECS_OFFSET(column_data, (start_row) * size),
                user_data->columns[i],
                user_data->row_count * size
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
    ecs_data_t *src_data,
    ecs_table_t *dst_table,
    ecs_data_t *dst_data,
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
        ecs_entities_t to_remove_array = {
            .array = ecs_vector_first(to_remove),
            .count = ecs_vector_count(to_remove)
        };

        ecs_run_component_actions(
            world,
            stage,
            src_table,
            src_data,
            src_index,
            count,
            to_remove_array,
            false
        );
    }

    if (src_type && src_type != dst_type) {
        /* Delete column from old table. Delete in reverse, as entity indexes of
         * entities after the deletion point change as a result of the delete. */

        int32_t i;
        for (i = 0; i < count; i ++) {
            ecs_table_delete(
                world, stage, src_table, src_data, src_index + count - i - 1);
        }
    }

    /* Invoke OnAdd systems */
    if (to_add) {
        ecs_entities_t to_add_array = {
            .array = ecs_vector_first(to_add),
            .count = ecs_vector_count(to_add)
        };

        ecs_run_component_actions(
            world,
            stage,
            dst_table,
            dst_data,
            dst_index,
            count,
            to_add_array,
            true
        );
    }
}

static
int32_t update_entity_index(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t start_entity,
    int32_t start_row,
    ecs_table_data_t *user_data)
{
    bool has_unset = false, tested_for_unset = false;
    int32_t i, dst_start_row = start_row;
    int32_t count = user_data->row_count;
    ecs_column_t *columns = data->columns;
    ecs_entity_t *entities = ecs_vector_first(data->entities);
    int32_t row_count = ecs_vector_count(data->entities);

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
    ecs_table_t *src_table = NULL, *prev_src_table = NULL;
    int32_t src_row = 0, prev_src_row = 0, dst_first_contiguous_row = start_row;
    int32_t src_first_contiguous_row = 0;

    ecs_entity_t e;

    /* We need to commit each entity individually in order to populate
     * the entity index */
    for (i = 0; i < count; i ++) {

        /* If existing array with entities was provided, use entity ids from
         * that array. Otherwise use new entity id */
        if(user_data->entities) {
            e = user_data->entities[i];

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

        ecs_record_t *record_ptr = ecs_eis_get(stage, e);
        if (record_ptr) {
            src_row = record_ptr->row;
            int8_t is_monitored = (src_row < 0) ? -1 : 1;
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
                prev_src_table = record_ptr->table;
            }

            src_table = record_ptr->table;

            /* If entity exists, and this is the first entity being iterated
             * set the start index to the current index of the entity. The
             * start_index indicates the start of where the to be added
             * entities will be stored. 
             * In the ideal scenario, the subsequent entities to be added
             * will be provided in the same order after the first entity, so
             * that the entity index does not need to be updated. */
            if (!i && src_table == table) {
                dst_start_row = src_row;
                dst_first_contiguous_row = src_row;
            } else {
                /* If the entity exists but it is not the first entity, 
                 * check if it is in the same table. If not, delete the 
                 * entity from the other table */     
                if (src_table != table) {
                    ecs_data_t *old_data = ecs_table_get_staged_data(
                        world, stage, src_table);

                    /* Insert new row into destination table */
                    int32_t dst_row = ecs_table_append(
                        world, table, data, e, record_ptr);
                    if (!i) {
                        dst_start_row = dst_row;
                        dst_first_contiguous_row = dst_row;
                        src_first_contiguous_row = src_row;
                    }

                    /* If the data structure has columns that are unset data
                     * must be copied from the old table to the new table */
                    if (!tested_for_unset) {
                        has_unset = has_unset_columns(
                            type, columns, user_data);
                        tested_for_unset = true;
                    }

                    if (has_unset) {
                        bool is_watching;
                        int32_t src_row = convert_record_row(
                            record_ptr->row, &is_watching);

                        ecs_table_move(
                            table, data, dst_row, src_table, old_data, src_row);
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
                        ecs_table_swap(world, stage, table, data, 
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
                        ecs_table_swap(world, stage, table, data, 
                            src_row, dst_start_row - 1, record_ptr, NULL);

                        /* Now move back the whole block back one position, 
                         * while moving the entity before the start to the 
                         * row right after the block */
                        ecs_table_move_back_and_swap(
                            world, stage, table, data, dst_start_row, i);

                        dst_start_row --;
                        dst_first_contiguous_row --;
                    }
                }
            }

            /* Update entity index with the new table / row */
            record_ptr->table = table;
            record_ptr->row = dst_start_row + i + 1;
            record_ptr->row *= is_monitored;
        } else {
            ecs_record_t *record = ecs_eis_get_or_create(stage, e);
            record->table = table,
            record->row = dst_start_row + i + 1;

            if (entities) {
                entities[dst_start_row] = e;
                //record_ptrs[dst_start_row] = record;
            } else {
                int32_t row = ecs_table_append(
                    world, table, data, e, record);

                ecs_assert(row == dst_start_row, ECS_INTERNAL_ERROR, NULL);
            }
        }

        /* Now that the entity index is updated for this entity, check if we
         * need to invoke reactive systems for the current set of contiguous
         * entities. */
        if (i) {
            ecs_assert(dst_first_contiguous_row >= dst_start_row, ECS_INTERNAL_ERROR, NULL);

            if (prev_src_table != src_table || (src_row != prev_src_row && prev_src_row != (src_row - 1))) {
                /* If either the previous type is different from the current
                 * type, or the previous index is not one before the current,
                 * entities are not from the same origin or they are not stored
                 * in a contiguous way. If this happens, invoke reactive systems
                 * up to this point. */
                ecs_table_t *prev_table = NULL;
                ecs_data_t *prev_data = NULL;

                if (prev_src_table) {
                    prev_table = prev_src_table;
                    prev_data = ecs_table_get_staged_data(
                        world, stage, prev_table);
                }

                invoke_reactive_systems(
                    world,
                    stage,
                    prev_table ? prev_table->type : NULL,
                    type,
                    src_table,
                    prev_data,
                    table,
                    data,
                    src_first_contiguous_row,
                    dst_first_contiguous_row,
                    i - dst_start_row - dst_first_contiguous_row,
                    user_data->columns == NULL);

                /* Start a new contiguous set */
                dst_first_contiguous_row = dst_start_row + i;
                src_first_contiguous_row = src_row;
                prev_src_table = src_table;
                same_origin = false;
            }            
        }

        prev_src_row = src_row;
    }

    /* Invoke reactive systems on the entities in the last contiguous set. If 
     * all entities are from the same origin, this will cover all entities. */
    ecs_data_t *src_data = NULL;
    int32_t contiguous_count = 0;

    if (same_origin) {
        contiguous_count = count;
    } else {
        contiguous_count = prev_src_row - src_first_contiguous_row + 1;
    }

    if (prev_src_table) {
        src_table = prev_src_table;
        src_data = ecs_table_get_staged_data(
            world, stage, src_table);
    } else {
        src_table = NULL;
    }

    invoke_reactive_systems(
        world,
        stage,
        src_table ? src_table->type : NULL,
        type,
        src_table,
        src_data,
        table,
        data,
        src_first_contiguous_row,
        dst_first_contiguous_row,
        contiguous_count,
        user_data->columns == NULL);

    return dst_start_row;
}

static
ecs_entity_t set_w_data_intern(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_type_t type,
    ecs_table_data_t *user_data)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_t result;
    int32_t count = user_data->row_count;

    if (user_data->entities) {
        result = user_data->entities[0];
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
        ecs_data_t *data = ecs_table_get_or_create_data(
                world, stage, table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, 0);
        int32_t start_row = 0;

        /* Grow world entity index only if no entity ids are provided. If ids
         * are provided, it is possible that they already appear in the entity
         * index, in which case they will be overwritten. */
        if (!user_data->entities) {
            start_row = ecs_table_grow(world, table, data, count, result);
            ecs_eis_grow(stage, count);
        }

        /* Obtain list of entities */
        ecs_entity_t *entities = ecs_vector_first(data->entities);

        /* If the entity array is NULL, we can safely allocate space for
         * row_count number of rows, which will give a perf boost the first time
         * the entities are inserted. */
        if (!entities) {
            ecs_table_set_size(table, data, count);
            entities = ecs_vector_first(data->entities);
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
            world, stage, type, table, data, result, start_row, user_data); 

        /* If columns were provided, copy data from columns into table. This is
         * where a lot of the performance benefits can be achieved: now that all
         * entities are nicely ordered in the destination table, we can copy the
         * data into each column with a single memcpy. */
        if (user_data->columns) {
            copy_column_data(type, data, start_row, user_data);

            uint32_t i, count = user_data->column_count;
            for (i = 0; i < count; i ++) {
                ecs_entity_t c = user_data->components[i];
                if (c < ECS_HI_COMPONENT_ID) {
                    ecs_component_data_t *cdata = get_component_data(world, c);
                    run_component_set_action(
                        world, cdata, c, table, data, start_row, 
                        user_data->row_count);
                }
            }            
        }            
    }

    return result;
}

ecs_entity_t _ecs_new_w_count(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_table_data_t table_data = {
        .row_count = count
    };

    ecs_table_t *table = ecs_table_from_type(world, stage, type);

    return set_w_data_intern(world, table, type, &table_data);
}

ecs_entity_t ecs_set_w_data(
    ecs_world_t *world,
    ecs_table_data_t *data)
{
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entities_t entities = {
        .array = data->components,
        .count = data->column_count
    };

    ecs_table_t *table = ecs_table_find_or_create(world, stage, &entities);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return set_w_data_intern(world, table, table->type, data);
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

    ecs_record_t *record;
    ecs_stage_t *stage = ecs_get_stage(&world);

    if (stage == &world->stage) {
        if ((record = ecs_eis_get(stage, entity))) {
            ecs_table_t *table = record->table;
            ecs_entities_t removed = {
                .array = ecs_vector_first(table->type),
                .count = ecs_vector_count(table->type)
            };

            ecs_data_t *data = ecs_table_get_data(world, table);

            // TODO: deal with monitored entities
            delete_entity(world, stage, entity, table, data, 
                record->row - 1, &removed);
        }
    } else {
        /* Remove the entity from the staged index. Any added components while
         * in progress will be discarded as a result. */
        ecs_eis_delete(stage, entity);
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
        ecs_type_t type = table->type;

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_type_match_w_filter(world, type, filter)) {
            continue;
        }

        /* Remove entities from index */
        ecs_data_t *data = ecs_table_get_data(world, table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

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

        if (!ecs_type_match_w_filter(world, type, filter)) {
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

        ecs_entities_t added = {0};
        ecs_entities_t removed = {0};

        ecs_table_t *dst_table = ecs_table_traverse(
            world, stage, table, &to_add_components, &to_remove_components,
            &added, &removed);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);            

        if (!dst_table->type) {
            /* If this removes all components, clear table */
            ecs_table_merge(world, NULL, table);
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
    ecs_world_t *world_arg = world;
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info = {0};

    void *dst = NULL;
    if (get_or_create_staged_info(world, stage, entity, &info)) {
        dst = get_component(&info, component);
    }

    if (!dst) {
        ecs_entities_t components = {
            .array = &component,
            .count = 1
        };

        ecs_add_remove_intern(world_arg, entity, &components, NULL, false);
        dst = get_component(&info, component);

        /* Check if data is available in main stage. If so, copy value from main
         * stage into staged value */
        if (stage != &world->stage) {
            ecs_entity_info_t main_info;
            if (get_info(world, entity, &main_info)) {
                void *main_dst = get_component(&main_info, component);
                if (main_dst) {
                    memcpy(dst, main_dst, size);
                    
                    /* We did add the component to the stage, but for the
                     * application this can be transparent as there is no need
                     * to initialize the component. */
                    if (is_added) {
                        *is_added = false;
                    }
                    return dst;
                }
            }
        }

        if (is_added) {
            *is_added = true;
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
    ecs_entity_info_t info = {0};

    if (stage == &world->stage) {
        get_info(world, entity, &info);
    } else {
        get_staged_info(world, stage, entity, &info);
    }

    ecs_component_data_t *cdata = get_component_data(world, component);    
    
    run_component_set_action(world, cdata, component, 
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
    ecs_entity_info_t info = {0};
    ecs_entities_t to_set = {
        .array = &component,
        .count = 1
    };

    /* If no entity is specified, create one */
    if (!entity) {
        entity = new_entity_handle(world);
        new_intern(world, stage, entity, &to_set);
    }
    
    void *dst = NULL;
    if (get_or_create_staged_info(world, stage, entity, &info)) {
        dst = get_component(&info, component);
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

    ecs_component_data_t *cdata = get_component_data(world, component);    
    
    run_component_set_action(world, cdata, component, 
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

const char* ecs_get_id(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    if (entity == EcsSingleton) {
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

        if (!filter || ecs_type_match_w_filter(world, table->type, filter)) {
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
