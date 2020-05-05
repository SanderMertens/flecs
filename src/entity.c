
#include "flecs_private.h"

static
int32_t new_w_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    int32_t count,
    void **component_data);

static
void override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_column_t *column,
    uint32_t row,
    uint32_t count);    

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

    ecs_assert(ecs_vector_count(data->entities) > info->row, 
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
    ecs_assert(ecs_vector_count(data->entities) > row, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(data->record_ptrs) > row, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t**record_ptrs = ecs_vector_first(data->record_ptrs);

    info->record = record_ptrs[row];

    return true;
}

/* Get entity info */
static
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

static
void run_component_trigger_for_entities(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *trigger_vec,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities)
{
    int32_t i, trigger_count = ecs_vector_count(trigger_vec);
    if (trigger_count) {
        EcsTrigger *triggers = ecs_vector_first(trigger_vec);
        int32_t index = ecs_type_index_of(table->type, component);
        ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
        index ++;

        ecs_entity_t components[1] = { component };
        int32_t columns[1] = { index };

        ecs_rows_t rows = {
            .world = world,
            .columns = columns,
            .table_count = 1,
            .inactive_table_count = 1,
            .column_count = 1,
            .table = table,
            .table_columns = data->columns,
            .components = components,
            .entities = entities,
            .offset = row,
            .count = count,
        };

        for (i = 0; i < trigger_count; i ++) {
            rows.system = triggers[i].self;
            rows.param = triggers[i].ctx;
            triggers[i].action(&rows);
        }
    }
}

static
void run_component_trigger(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *trigger_vec,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    ecs_entity_t *entities = ecs_vector_first(data->entities);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    entities = ECS_OFFSET(entities, sizeof(ecs_entity_t) * row);

    run_component_trigger_for_entities(
        world, stage, trigger_vec, component, table, data, row, count,
        entities);
}

static
int32_t find_prefab(
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

        if (e < ECS_TYPE_FLAG_START) {
            break;
        }

        if (e & ECS_INSTANCEOF) {
            override_from_base(
                world, e & ECS_ENTITY_MASK, component, column, row, count);
        }
    } while (--i >= 0);
}

static
void instantiate(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

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
    ecs_data_t *child_data = ecs_table_get_data(world, child_table);
    int32_t type_count = ecs_vector_count(type);
    ecs_entity_t *type_array = ecs_vector_first(type);
    EcsParent *parent_array = NULL;

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_entities_t components = {
        .array = ecs_os_alloca(type_count, sizeof(ecs_entity_t)),
    };

    void **component_data = NULL;

    if (child_data) {
        component_data = ecs_os_alloca(void*, type_count);
    }

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int i, base_index, pos = 0;

    for (i = 0; i < type_count; i ++) {
        ecs_entity_t c = type_array[i];
        
        /* Make sure instances don't have EcsPrefab */
        if (c == ecs_entity(EcsPrefab)) {
            continue;
        }

        /* If prefab children have a EcsParent component, that means this is a 
         * prefab hierarchy. Store pointer to array as we'll need it later */
        if (c == ecs_entity(EcsParent)) {
            ecs_assert(parent_array == NULL, ECS_INTERNAL_ERROR, NULL);
            parent_array = ecs_vector_first(child_data->columns[i].data);
            component_data[pos] = NULL;
        } else {
            /* Keep track of the element that creates the CHILDOF relationship with
            * the prefab parent. We need to replace this element to make sure the
            * created children point to the instance and not the prefab */ 
            if (c & ECS_CHILDOF && (c & ECS_ENTITY_MASK) == base) {
                base_index = pos;
            }        

            /* Store pointer to component array. We'll use this component array to
            * create our new entities in bulk with new_w_data */
            component_data[pos] = ecs_vector_first(child_data->columns[i].data);
        }

        components.array[pos] = c;
        pos ++;
    }

    components.count = pos;

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *entities = ecs_vector_first(data->entities);
    int32_t entity_count = ecs_vector_count(data->entities);
    int32_t child_count = ecs_vector_count(child_data->entities);

    for (i = row; i < (entity_count + row); i ++) {
        ecs_entity_t e = entities[i];

        /* Replace CHILDOF element in the component array with instance id */
        components.array[base_index] = ECS_CHILDOF | e;

        /* Find or create table */
        ecs_table_t *table = ecs_table_find_or_create(
            world, stage, &components);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create children */
        int32_t child_row = new_w_data(
            world, stage, table, child_count, component_data);

        /* If prefab child table has children itself, recursively instantiate */
        if (parent_array) {
            ecs_data_t *i_data = ecs_table_get_staged_data(world, stage, table);
            ecs_entity_t *children = ecs_vector_first(child_data->entities);

            int j;
            for (j = 0; j < child_count; j ++) {
                ecs_entity_t child = children[j];
                instantiate(world, stage, child, table, i_data, child_row, 1);
            }
        }
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
    if (parent_data) {
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

    /* Run OnSet handlers for prefab components */
    ecs_entity_info_t info;
    if (get_info(world, base, &info) && info.table) {
        ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data);
        ecs_type_t base_type = info.table->type;

        /* Iterate components of base type */
        ecs_vector_each(base_type, ecs_entity_t, component, {
            ecs_component_data_t *cdata = &cdata_array[*component];
            ecs_vector_t *on_set = cdata->on_set;
            if (on_set) {
                /* Run trigger separately for each entity. We can't call the
                 * trigger on the range because we must "trick" the trigger to 
                 * use data from the base but the instance id. */
                int j;
                for (j = row; j < count; j ++) {
                    ecs_entity_t *entities = ecs_vector_first(data->entities);

                    /* Run trigger on base data with instance id */
                    run_component_trigger_for_entities(world, stage, on_set, 
                        *component, info.table, info.data, info.row, 1, 
                        &entities[j]);
                }
            }
        });
    }
}

static
void run_init_actions(
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
            ctor(world, component, ecs_vector_first(data->entities), ptr, size, 
                count, ctx);
        }

        if (has_base && component != EEcsParent) {
            override_component(world, component, type, column, row, count);
        }

        if (systems) {
            run_component_trigger(
                world, stage, systems, component, table, data, 
                row, count);
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

        }

        if (dtor) {
            dtor(world, component, ecs_vector_first(data->entities), ptr, size, 
                count, ctx);
        }
    }
}

static
void run_monitors(
    ecs_world_t *world, 
    ecs_stage_t *stage, 
    ecs_table_t *dst_table, 
    ecs_data_t *dst_data, 
    int32_t dst_row, 
    int32_t count, 
    ecs_table_t *src_table)
{
    if (dst_table == src_table) {
        return;
    }

    if (!dst_table->monitors) {
        return;
    }

    if (!src_table->monitors) {
        ecs_vector_each(dst_table->monitors, ecs_monitor_t, monitor, {
            ecs_run_monitor(world, stage, monitor, dst_row, count);
        });
    } else {
        /* If both tables have monitors, run the ones that dst_table has and
         * src_table doesn't have */
        int32_t i, count = ecs_vector_count(dst_table->monitors);
        int32_t j = 0, src_count = ecs_vector_count(src_table->monitors);
        ecs_monitor_t *dst_monitors = ecs_vector_first(dst_table->monitors);
        ecs_monitor_t *src_monitors = ecs_vector_first(src_table->monitors);

        for (i = 0; i < count; i ++) {
            ecs_monitor_t *dst = &dst_monitors[i];

            ecs_monitor_t *src = 0;
            while (j < src_count) {
                src = &src_monitors[j];
                if (src->system < dst->system) {
                    j ++;
                } else {
                    break;
                }
            }

            if (src->system == dst->system) {
                continue;
            }

            ecs_run_monitor(world, stage, dst, dst_row, count);
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
        record->row = ecs_row_to_record(new_row, info->is_watched);
    } else {
        new_row = ecs_table_append(
            world, new_table, dst_data, entity, NULL);  

        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = new_table,
            .row = ecs_row_to_record(new_row, info->is_watched)
        });
    }

    ecs_assert(
        ecs_vector_count(dst_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);

    run_init_actions(world, stage, new_table, dst_data, new_row, 1, *added);

    run_monitors(world, stage, new_table, dst_data, new_row, 1, NULL);

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
    ecs_data_t *dst_data = ecs_table_get_or_create_data(world, stage, dst_table);
    if (src_data == dst_data) {
        return src_row;
    }

    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data->entities) > src_row, ECS_INTERNAL_ERROR, NULL);

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
        record->row = ecs_row_to_record(dst_row, info->is_watched);
    } else {
        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = dst_table,
            .row = ecs_row_to_record(dst_row, info->is_watched)
        });
    }

    if (added) {
        run_init_actions(
            world, stage, dst_table, dst_data, dst_row, 1, *added);
    }

    run_monitors(world, stage, dst_table, dst_data, dst_row, 1, src_table);

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

static
void commit(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *dst_table,   
    ecs_entities_t *added,
    ecs_entities_t *removed)
{
    ecs_table_t *src_table = info->table;
    bool in_progress = world->in_progress;

    if (src_table) {
        ecs_data_t *src_data = info->data;

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Only move entity when it is not moved to the root table, unless we're
         * iterating. In this case the entities need to be kept around so that
         * the merge knows to remove them from their previous tables. */
        if (dst_table->type || in_progress) {            
            info->row = move_entity(
                world, stage, entity, info, src_table, src_data, 
                info->row, dst_table, added, removed);
            info->table = dst_table;
        } else {
            delete_entity(
                world, stage, entity, src_table, src_data, info->row, 
                removed);

            ecs_eis_set(stage, entity, &(ecs_record_t){
                NULL, -info->is_watched
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
        if (dst_table->type) {
            info->row = new_entity(world, stage, entity, info, dst_table, added);
            info->table = dst_table;
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

    while (!ptr && (p = find_prefab(type, p)) != -1) {
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

static
void new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entities_t *to_add)
{
    ecs_entities_t added = {
        .array = ecs_os_alloca(ecs_entity_t, to_add->count)
    };
    ecs_entity_info_t info = {0};
    ecs_table_t *table = ecs_table_traverse_add(
        world, stage, &world->stage.root, to_add, &added);
    new_entity(world, stage, entity, &info, table, &added);
}

static
int32_t new_w_data(
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

    run_init_actions(world, stage, table, data, row, count, added);

    /* If data was provided, copy each of the columns into the new table */
    if (component_data) {
        int column_count = ecs_vector_count(type);
        for (i = 0; i < column_count; i ++) {
            /* If this is not a component, skip */
            ecs_entity_t component = added.array[i];
            if (component >= ECS_HI_COMPONENT_ID) {
                continue;
            }

            void *src_ptr = component_data[i];
            if (!src_ptr) {
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

            run_component_trigger(
                world, stage, cdata->on_set, component, table, data, 
                row, count);
        }
    }

    run_monitors(world, stage, table, data, row, count, NULL);

    return row;
}

static
void bulk_delete(
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

static
bool has_type(
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

    return ecs_type_contains(
        world, entity_type, type, match_any, match_prefabs) != 0;
}

static
void add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *to_add,
    ecs_entities_t *to_remove,
    bool do_set)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t add_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entity_t remove_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = add_buffer };
    ecs_entities_t removed = { .array = remove_buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, to_remove, &removed);

    dst_table = ecs_table_traverse_add(
        world, stage, dst_table, to_add, &added);        

    commit(world, stage, entity, &info, dst_table, &added, &removed);
}

static
void add_entities_w_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, stage, src_table, components, &added);

    commit(world, stage, entity, info, dst_table, &added, NULL);
}

static
void remove_entities_w_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, components, &removed);

    commit(world, stage, entity, info, dst_table, NULL, &removed);
}

static
void add_entities(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, stage, src_table, components, &added);

    commit(world, stage, entity, &info, dst_table, &added, NULL);
}

static
void remove_entities(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);    
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, components, &removed);

    commit(world, stage, entity, &info, dst_table, NULL, &removed);
}

static
void *get_mutable(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_entity_info_t *info,
    bool *is_added)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((component & ECS_ENTITY_MASK) == component, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    void *dst = NULL;
    if (stage == &world->stage) {
        if (get_info(world, entity, info)) {
            dst = get_component(info, component);
        }
    } else {
        if (get_staged_info(world, stage, entity, info)) {
            dst = get_component(info, component);
        } else {
            /* If the entity isn't store din the current stage, we still need to
             * get the data from the main stage to pass it to commit */
            get_info(world, entity, info);
        }
    }

    ecs_table_t *table = info->table;

    if (!dst) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        add_entities_w_info(world, stage, entity, info, &to_add);
        dst = get_component(info, component);

        if (is_added) {
            *is_added = table != info->table;
        }

        return dst;
    } else {
        if (is_added) {
            *is_added = false;
        }

        return dst;
    }
}

/* -- Private functions -- */

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


        bool result = ecs_type_has_entity(
            world, type, component);
        if (result) {
            if (entity_out) *entity_out = e;
            return true;
        }
    }

    return false;
}

/* -- Public functions -- */

ecs_entity_t ecs_new_w_type(
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

        new(world, stage, entity, &to_add);
    }

    return entity;
}

ecs_entity_t ecs_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);    
    ecs_entity_t entity = new_entity_handle(world);

    if (component) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        new(world, stage, entity, &to_add);
    }

    return entity;
}

ecs_entity_t ecs_bulk_new_w_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count,
    void **data)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    ecs_entity_t result = world->last_handle + 1;
    new_w_data(world, stage, table, count, data);
    return result;
}

ecs_entity_t ecs_bulk_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entities_t type = {
        .array = &entity,
        .count = 1
    };
    ecs_table_t *table = ecs_table_find_or_create(world, stage, &type);
    ecs_entity_t result = world->last_handle + 1;
    new_w_data(world, stage, table, count, NULL);
    return result;
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

        remove_entities_w_info(world, stage, entity, &info, &to_remove);
    }

    /* If this is a staged delete, set the table in the staged entity index to
     * NULL. That way the merge will know to delete this entity vs. just to
     * remove its components */
    if (stage != &world->stage) {
        ecs_eis_set(stage, entity, &(ecs_record_t){ NULL, 0 });
    }
}

void ecs_bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    bulk_delete(world, filter, true);
}

void ecs_clear_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    bulk_delete(world, filter, false);
}

void ecs_bulk_add_remove_type(
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

        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_components, NULL);
        
        dst_table = ecs_table_traverse_add(
            world, stage, dst_table, &to_add_components, NULL);

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

void ecs_add_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = {
        .array = ecs_vector_first(type),
        .count = ecs_vector_count(type)
    };
    add_entities(world, entity, &components);
}

void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add)
{
    ecs_entities_t components = { .array = &to_add, .count = 1 };
    add_entities(world, entity, &components);
}

void ecs_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = {
        .array = ecs_vector_first(type),
        .count = ecs_vector_count(type)
    };
    remove_entities(world, entity, &components);
}

void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_remove)
{
    ecs_entities_t components = { .array = &to_remove, .count = 1 };
    remove_entities(world, entity, &components);
}

void ecs_add_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add,
    ecs_entity_t to_remove)
{
    ecs_entities_t components_add = { .array = &to_add, .count = 1 };      
    ecs_entities_t components_remove = { .array = &to_remove, .count = 1 };      
    add_remove(world, entity, &components_add, &components_remove, false);
}

void ecs_add_remove_type(
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
    add_remove(world, entity, &components_add, &components_remove, false);
}

ecs_entity_t ecs_copy(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    if (!dst) {
        dst = ++ world->last_handle;
    }

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
    dst_info.row = new_entity(
        world, stage, dst, &dst_info, src_table, &to_add);

    if (copy_value) {
        ecs_table_move(src_table, dst_info.data, dst_info.row,
            src_table, src_info.data, src_info.row);

        int i;
        for (i = 0; i < to_add.count; i ++) {
            ecs_entity_t component = to_add.array[i];
            ecs_component_data_t *cdata = ecs_get_component_data(world, component);
            run_component_trigger(
                world, stage, cdata->on_set, component, src_table, src_info.data, 
                dst_info.row, 1);
        }
    }    

    return dst;
}

const void* ecs_get_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;
    void *ptr = NULL;

    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    bool found = ecs_get_info(world, stage, entity, &info);
    if (found) {
        if (!info.table) {
            return NULL;
        }

        ptr = get_component(&info, component);
        if (!ptr) {
            if (component != EEcsName && component != EEcsPrefab) {
                ptr = get_base_component(
                    world, stage, entity, &info, 0, component);
            }
        }        
    }

    return ptr;
}

const void* ecs_get_cached_ptr_w_entity(
    ecs_world_t *world,
    ecs_cached_ptr_t *cached_ptr,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_record_t *record = cached_ptr->record;

    if (stage != &world->stage) {
        ecs_record_t *staged = ecs_eis_get(stage, entity);
        if (staged) {
            record = staged;
        } else {
            stage = &world->stage;
        }
    }

    if (!record && stage == &world->stage) {
        record = ecs_eis_get(&world->stage, entity);
    }

    if (!record || !record->table) {
        return NULL;
    }

    ecs_table_t *table = record->table;
    ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);

    if (cached_ptr->stage == stage &&
        cached_ptr->record == record &&
        cached_ptr->table == table &&
        cached_ptr->row == record->row &&
        cached_ptr->size == ecs_vector_size(data->entities))
    {
        return cached_ptr->ptr;
    }

    cached_ptr->stage = stage;
    cached_ptr->table = table;
    cached_ptr->row = record->row;
    cached_ptr->size = ecs_vector_size(data->entities);

    ecs_entity_info_t info = {0};
    set_info_from_record(world, &info, record);
    cached_ptr->ptr = get_component(&info, component);

    if (&world->stage == stage) {
        cached_ptr->record = record;
    } else {
        cached_ptr->record = NULL;
    }

    return cached_ptr->ptr;
}

void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool *is_added)
{
    ecs_entity_info_t info;
    return get_mutable(world, entity, component, &info, is_added);
}

void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert((component & ECS_ENTITY_MASK) == component, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;

    ecs_get_info(world, stage, entity, &info);

    ecs_component_data_t *cdata = ecs_get_component_data(world, component);
    run_component_trigger(world, stage, cdata->on_set, component, 
        info.table, info.data, info.row, 1);
}

ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    
    if (!entity) {
        entity = new_entity_handle(world);
    }

    ecs_entity_info_t info;
    void *dst = get_mutable(world, entity, component, &info, NULL);
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ptr) {
        memcpy(dst, ptr, size);
    } else {
        memset(dst, 0, size);
    }

    ecs_component_data_t *cdata = ecs_get_component_data(world, component);    
    run_component_trigger(world, stage, cdata->on_set, component, 
        info.table, info.data, info.row, 1);

    return entity;
}

bool ecs_has_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_type_t type = ecs_get_type(world, entity);
    return ecs_type_has_entity(world, type, component);
}

bool ecs_has_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    return has_type(world, entity, type, true, true);
}

ecs_entity_t ecs_get_parent_w_entity(
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

    const EcsName *id = ecs_get_ptr(world, entity, EcsName);

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

    const EcsType *type = ecs_get_ptr(world, entity, EcsType);
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

int32_t ecs_count_type(
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

int32_t ecs_count_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{    
    if (!entity) {
        return 0;
    }

    ecs_type_t type = ecs_type_from_entity(world, entity);

    return ecs_count_w_filter(world, &(ecs_filter_t){
        .include = type
    });
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

