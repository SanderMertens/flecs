
#include "private_api.h"

static
int32_t comp_mask_index(
    int32_t value)
{
    return value >> 6;
}

static
void comp_mask_set(
    ecs_comp_mask_t mask,
    ecs_entity_t value)
{
    ecs_assert(value < UINT_MAX, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
    int32_t index = comp_mask_index((int32_t)value);
    mask[index] |= (ecs_entity_t)1 << (value & 0x3F);
}

static
bool comp_mask_is_set(
    ecs_comp_mask_t mask,
    ecs_entity_t value)
{
    ecs_assert(value < UINT_MAX, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
    int32_t index = comp_mask_index((int32_t)value);
    return (mask[index] & (ecs_entity_t)1 << (value & 0x3F)) != 0;
}

static
int32_t new_w_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *component_ids,
    int32_t count,
    void **c_info);

static 
void* get_component_w_index(
    ecs_entity_info_t *info,
    int32_t index)
{
    ecs_data_t *data = info->data;
    ecs_column_t *columns;
    if (!data || !(columns = data->columns)) {
        return NULL;
    }

    ecs_assert(index < info->table->column_count, ECS_INVALID_COMPONENT_ID, NULL);

    ecs_column_t *column = &columns[index];
    ecs_vector_t *data_vec = column->data;
    int16_t size = column->size; 

    ecs_assert(!size || data_vec != NULL, ECS_INTERNAL_ERROR, NULL);

    if (size) {
        void *ptr = ecs_vector_first_t(data_vec, size, column->alignment);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        return ECS_OFFSET(ptr, info->row * size);
    }

    return NULL;
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

    ecs_assert(info->row >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == component) {
            return get_component_w_index(info, c_ptr_i);
        }
    });
    
    return NULL;
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
    ecs_record_t**record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);

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

void ecs_get_column_info(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *components,
    ecs_column_info_t *cinfo,
    bool get_all)
{
    int32_t column_count = table->column_count;
    ecs_entity_t *type_array = ecs_vector_first(table->type, ecs_entity_t);

    if (get_all) {
        int32_t i, count = components->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = type_array[i];
            cinfo[i].id = id;
            cinfo[i].ci = ecs_get_c_info(world, id);
            cinfo[i].column = i;            
        }
    } else {
        ecs_entity_t *array = components->array;
        int32_t i, cur, count = components->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = array[i];
            cinfo[i].id = id;
            cinfo[i].ci = ecs_get_c_info(world, id);
            cinfo[i].column = -1;

            for (cur = 0; cur < column_count; cur ++) {
                if (type_array[cur] == id) {
                    cinfo[i].column = cur;
                    break;
                }
            }
        }
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
    (void)world;
    int32_t i, trigger_count = ecs_vector_count(trigger_vec);
    if (trigger_count) {
        EcsTrigger *triggers = ecs_vector_first(trigger_vec, EcsTrigger);
        int32_t index = ecs_type_index_of(table->type, component);
        ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
        index ++;

        ecs_entity_t components[1] = { component };
        int32_t columns[1] = { index };

        ecs_iter_t it = {
            .world = stage->world,
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
            it.system = triggers[i].self;
            it.param = triggers[i].ctx;
            triggers[i].action(&it);
        }
    }
}

void ecs_run_component_trigger(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *trigger_vec,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    if (table->flags & EcsTableIsPrefab) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);

    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    run_component_trigger_for_entities(
        world, stage, trigger_vec, component, table, data, row, count, entities);
}

#ifdef FLECS_SYSTEM 
static
void run_set_systems_for_entities(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities,
    bool set_all)
{   
    /* Run OnSet systems */
    if (set_all) {
        ecs_vector_t *queries = table->on_set_all;
        ecs_vector_each(queries, ecs_matched_query_t, m, {
            ecs_run_monitor(world, stage, m, components, row, count, entities);
        });
    } else {
        ecs_vector_t **on_set_systems = table->on_set;
        if (on_set_systems) {
            int32_t index = ecs_type_index_of(table->type, components->array[0]);
            ecs_vector_t *queries = on_set_systems[index];
            ecs_vector_each(queries, ecs_matched_query_t, m, {
                ecs_run_monitor(world, stage, m, components, row, count, entities);
            });
        }
    }
}
#endif

void ecs_run_set_systems(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    bool set_all)
{
#ifdef FLECS_SYSTEM    
    if (!count || !data) {
        return;
    }
    
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);

    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    run_set_systems_for_entities(world, stage, components, table, row, 
        count, entities, set_all);
#endif
}

void ecs_run_monitors(
    ecs_world_t *world, 
    ecs_stage_t *stage, 
    ecs_table_t *dst_table,
    ecs_vector_t *v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors)
{
#ifdef FLECS_SYSTEM    
    if (v_dst_monitors == v_src_monitors) {
        return;
    }

    if (!v_dst_monitors) {
        return;
    }

    if (dst_table->flags & EcsTableIsPrefab) {
        return;
    }

    if (!v_src_monitors) {
        ecs_vector_each(v_dst_monitors, ecs_matched_query_t, monitor, {
            ecs_run_monitor(world, stage, monitor, NULL, dst_row, count, NULL);
        });
    } else {
        /* If both tables have monitors, run the ones that dst_table has and
         * src_table doesn't have */
        int32_t i, m_count = ecs_vector_count(v_dst_monitors);
        int32_t j = 0, src_count = ecs_vector_count(v_src_monitors);
        ecs_matched_query_t *dst_monitors = ecs_vector_first(v_dst_monitors, ecs_matched_query_t);
        ecs_matched_query_t *src_monitors = ecs_vector_first(v_src_monitors, ecs_matched_query_t);

        for (i = 0; i < m_count; i ++) {
            ecs_matched_query_t *dst = &dst_monitors[i];

            ecs_entity_t system = dst->query->system;
            ecs_assert(system != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_matched_query_t *src = 0;
            while (j < src_count) {
                src = &src_monitors[j];
                if (src->query->system < system) {
                    j ++;
                } else {
                    break;
                }
            }

            if (src->query->system == system) {
                continue;
            }

            ecs_run_monitor(world, stage, dst, NULL, dst_row, count, NULL);
        }
    }
#endif
}

static
int32_t find_prefab(
    ecs_type_t type,
    int32_t n)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *buffer = ecs_vector_first(type, ecs_entity_t);

    for (i = n + 1; i < count; i ++) {
        ecs_entity_t e = buffer[i];
        if (e & ECS_INSTANCEOF) {
            return i;
        }
    }

    return -1;
}

static
void instantiate(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

static
void instantiate_children(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_t *child_table)
{
    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = ecs_table_get_data(world, child_table);
    int32_t column_count = child_table->column_count;
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t type_count = ecs_vector_count(type);

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_entities_t components = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * type_count)
    };

    void **c_info = NULL;

    if (child_data) {
        c_info = ecs_os_alloca(ECS_SIZEOF(void*) * column_count);
    } else {
        return;
    }

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int i, base_index = -1, pos = 0;

    for (i = 0; i < type_count; i ++) {
        ecs_entity_t c = type_array[i];
        
        /* Make sure instances don't have EcsPrefab */
        if (c == EcsPrefab) {
            continue;
        }

        /* Keep track of the element that creates the CHILDOF relationship with
        * the prefab parent. We need to replace this element to make sure the
        * created children point to the instance and not the prefab */ 
        if (c & ECS_CHILDOF && (c & ECS_ENTITY_MASK) == base) {
            base_index = pos;
        }        

        /* Store pointer to component array. We'll use this component array to
        * create our new entities in bulk with new_w_data */
        if (i < column_count) {
            ecs_column_t *column = &child_data->columns[i];
            c_info[pos] = ecs_vector_first_t(column->data, column->size, column->alignment);
        }

        components.array[pos] = c;
        pos ++;
    }

    ecs_assert(base_index != -1, ECS_INTERNAL_ERROR, NULL);

    components.count = pos;

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t child_count = ecs_vector_count(child_data->entities);

    for (i = row; i < count + row; i ++) {
        ecs_entity_t e = entities[i];

        /* Replace CHILDOF element in the component array with instance id */
        components.array[base_index] = ECS_CHILDOF | e;

        /* Find or create table */
        ecs_table_t *table = ecs_table_find_or_create(
            world, stage, &components);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create children */
        int32_t child_row = new_w_data(
            world, stage, table, NULL, child_count, c_info);

        /* If prefab child table has children itself, recursively instantiate */
        ecs_data_t *i_data = ecs_table_get_staged_data(world, stage, table);
        ecs_entity_t *children = ecs_vector_first(child_data->entities, ecs_entity_t);

        int j;
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, stage, child, i_data, child_row, 1);
        }
    }    
}

static
void instantiate(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{    
    /* If base is a parent, instantiate children of base for instances */
    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, base);

    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, child_table_ptr, {
            ecs_table_t *child_table = *child_table_ptr;
            if (!ecs_table_count(child_table)) {
                continue;
            }

            instantiate_children(
                world, stage, base, data, row, count, child_table);
        });
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count);

static
bool override_from_base(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t component,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_entity_info_t base_info;
    if (!get_info(world, base, &base_info)) {
        return false;
    }

    void *base_ptr = get_component(&base_info, component);
    if (base_ptr) {
        int16_t data_size = column->size;
        void *data_array = ecs_vector_first_t(column->data, column->size, column->alignment);
        void *data_ptr = ECS_OFFSET(data_array, data_size * row);
        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        int32_t index;

        ecs_copy_t copy = cdata ? cdata->lifecycle.copy : NULL;
        if (copy) {
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);

            void *ctx = cdata->lifecycle.ctx;
            for (index = 0; index < count; index ++) {
                copy(world, component, &base, &entities[row], 
                    data_ptr, base_ptr, ecs_to_size_t(data_size), 1, ctx);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }
        } else {
            for (index = 0; index < count; index ++) {
                ecs_os_memcpy(data_ptr, base_ptr, data_size);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }                    
        }

        return true;
    } else {
        /* If component not found on base, check if base itself inherits */
        ecs_type_t base_type = base_info.table->type;
        return override_component(world, component, base_type, data, column, row, 
            count);
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t i, type_count = ecs_vector_count(type);

    /* Walk prefabs */
    i = type_count - 1;
    do {
        ecs_entity_t e = type_array[i];

        if (e < ECS_TYPE_ROLE_START) {
            break;
        }

        if (e & ECS_INSTANCEOF) {
            return override_from_base(world, e & ECS_ENTITY_MASK, component, 
                data, column, row, count);
        }
    } while (--i >= 0);

    return false;
}

void ecs_components_construct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    (void) world;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_xtor_t ctor;
        if (!c_info || !(ctor = c_info->lifecycle.ctor)) {
            continue;
        }

        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_column_t *column = &columns[component_info[i].column];
        int16_t size = column->size;
        void *array = ecs_vector_first_t(column->data, size, column->alignment);
        ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_entity_t component = component_info[i].id;
        ctor(stage->world, component, &entities[row], 
            ECS_OFFSET(array, size * row), ecs_to_size_t(size), count, 
            c_info->lifecycle.ctx);
    }
}

void ecs_components_destruct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    (void)world;

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);
    if (!component_count) {
        return;
    }

    ecs_column_t *columns = data->columns;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_xtor_t dtor;
        if (!c_info || !(dtor = c_info->lifecycle.dtor)) {
            continue;
        }

        ecs_column_t *column = &columns[component_info[i].column];
        int16_t size = column->size;
        void *array = ecs_vector_first_t(column->data, size, column->alignment);
        ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_entity_t component = component_info[i].id;
        dtor(stage->world, component, &entities[row], 
            ECS_OFFSET(array, size * row), ecs_to_size_t(size), count, 
            c_info->lifecycle.ctx);
    }
}

void ecs_components_override(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count,
    ecs_comp_mask_t set_mask,
    bool run_on_set)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);
    if (!component_count) {
        return;
    }

    ecs_table_t *table_without_base = table;
    ecs_column_t *columns = data->columns;
    ecs_type_t type = table->type;
    int32_t column_count = table->column_count;

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_entity_t component = component_info[i].id;

        if (component >= ECS_HI_COMPONENT_ID) {
            if (component & ECS_INSTANCEOF) {
                ecs_entity_t base = component & ECS_ENTITY_MASK;
                instantiate(world, stage, base, data, row, count);

                /* If table has on_set systems, get table without the base
                 * entity that was just added. This is needed to determine the
                 * diff between the on_set systems of the current table and the
                 * table without the base, as these are the systems that need to
                 * be invoked */
                ecs_entities_t to_remove = {
                    .array = &component,
                    .count = 1
                };
                table_without_base = ecs_table_traverse_remove(world, stage, 
                    table_without_base, &to_remove, NULL);
            }
        }

        int32_t column_index = component_info[i].column;
        if (column_index == -1 || column_index >= column_count) {
            continue;
        }

        if (!comp_mask_is_set(set_mask, component)) {
            ecs_column_t *column = &columns[column_index];
            if (override_component(world, component, type, data, column, 
                row, count)) 
            {
                ecs_entities_t to_remove = {
                    .array = &component,
                    .count = 1
                };
                table_without_base = ecs_table_traverse_remove(world, 
                    stage, table_without_base, &to_remove, NULL);
            }
        }
    }

    /* Run OnSet actions when a base entity is added to the entity for 
     * components not overridden by the entity. */
    if (run_on_set && table_without_base != table) {
        ecs_run_monitors(world, stage, table, table->on_set_all, row, count, 
            table_without_base->on_set_all);
    }
}

void ecs_components_on_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_vector_t *triggers;
        if (!c_info || !(triggers = c_info->on_add)) {
            continue;
        }

        ecs_entity_t component = component_info[i].id;
        ecs_run_component_trigger(
            world, stage, triggers, component, table, data, row, count);
    }
}

void ecs_components_on_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_vector_t *triggers;
        if (!c_info || !(triggers = c_info->on_remove)) {
            continue;
        }

        ecs_entity_t component = component_info[i].id;
        ecs_run_component_trigger(
            world, stage, triggers, component, table, data, row, count);
    }
}

static
void ecs_delete_children(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, parent);

    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, tptr, {
            ecs_table_t *table = *tptr;
            ecs_table_clear(world, table);
        });
    }
}

void ecs_run_add_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *added,
    ecs_comp_mask_t set_mask,
    bool get_all,
    bool run_on_set)
{
    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(added->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);

    ecs_column_info_t cinfo[ECS_MAX_ADD_REMOVE];
    ecs_get_column_info(world, table, added, cinfo, get_all);
    int added_count = added->count;

    if (table->flags & EcsTableHasCtors) {
        ecs_components_construct(
            world, stage, data, row, count, cinfo, added_count);
    }

    if (table->flags & EcsTableHasBase) {
        ecs_components_override(
            world, stage, table, data, row, count, cinfo, 
            added_count, set_mask, run_on_set);
    }

    if (table->flags & EcsTableHasOnAdd) {
        ecs_components_on_add(world, stage, table, data, row, count, 
            cinfo, added_count);
    }
}

void ecs_run_remove_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *removed,
    bool get_all)
{
    ecs_assert(removed != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(removed->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);

    ecs_column_info_t cinfo[ECS_MAX_ADD_REMOVE];
    ecs_get_column_info(world, table, removed, cinfo, get_all);
    int removed_count = removed->count;

    if (table->flags & EcsTableHasOnRemove) {
        ecs_components_on_remove(world, stage, table, data, 
            row, count, cinfo, removed_count);
    }

    if (table->flags & EcsTableHasDtors) {
        ecs_components_destruct(
            world, stage, data, row, count, cinfo, removed_count);
    }    
}

static
int32_t new_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *new_table,
    ecs_entities_t *added)
{
    ecs_record_t *record = info->record;
    ecs_data_t *new_data = ecs_table_get_or_create_data(world, stage, new_table);
    int32_t new_row;

    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);

    if (stage == &world->stage) {
        if (!record) {
            record = ecs_eis_get_or_create(stage, entity);
        }

        new_row = ecs_table_append(
            world, new_table, new_data, entity, record);

        record->table = new_table;
        record->row = ecs_row_to_record(new_row, info->is_watched);
    } else {
        new_row = ecs_table_append(
            world, new_table, new_data, entity, NULL);  

        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = new_table,
            .row = ecs_row_to_record(new_row, info->is_watched)
        });
    }

    ecs_assert(
        ecs_vector_count(new_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);    

    if (new_table->flags & EcsTableHasAddActions) {
        ecs_comp_mask_t set_mask = {0};
        ecs_run_add_actions(
            world, stage, new_table, new_data, new_row, 1, added, set_mask, 
            true, true);

        if (new_table->flags & EcsTableHasMonitors) {
            ecs_run_monitors(
                world, stage, new_table, new_table->monitors, new_row, 1, NULL);              
        }        
    }   

    info->data = new_data;
    
    return new_row;
}

static
bool compare_stage_w_data(
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data)
{
    ecs_data_t *data_array = ecs_vector_first(table->data, ecs_data_t);
    if (&data_array[stage->id] == data) {
        return true;
    }
    return false;
}

static
int32_t move_entity(
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
    bool same_stage = compare_stage_w_data(stage, src_table, src_data);

    if (main_stage && record) {
        record->table = dst_table;
        record->row = ecs_row_to_record(dst_row, info->is_watched);
    } else {
        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = dst_table,
            .row = ecs_row_to_record(dst_row, info->is_watched)
        });
    }

    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        ecs_table_move(
            world, entity, entity, dst_table, dst_data, dst_row, src_table, 
            src_data, src_row, !same_stage);

        /* If entity was moved, invoke UnSet monitors for each component that
         * the entity no longer has */
        ecs_run_monitors(world, stage, dst_table, src_table->un_set_all, 
            dst_row, 1, dst_table->un_set_all);

        /* If components were removed, invoke remove actions before deleting */
        if (removed && (src_table->flags & EcsTableHasRemoveActions)) {   
            ecs_run_remove_actions(
                world, stage, src_table, src_data, src_row, 1, removed, false);
        }            
    }

    /* Only delete from source table if moving to the same stage */
    if (same_stage) {
        ecs_table_delete(world, stage, src_table, src_data, src_row);
    }

    /* If components were added, invoke add actions */
    if (src_table != dst_table) {
        if (added && (dst_table->flags & EcsTableHasAddActions)) {
            ecs_comp_mask_t set_mask = {0};
            ecs_run_add_actions(
                world, stage, dst_table, dst_data, dst_row, 1, added, set_mask, 
                false, true);
        }

        /* Run monitors */
        if (dst_table->flags & EcsTableHasMonitors) {
            ecs_run_monitors(world, stage, dst_table, dst_table->monitors, dst_row, 
                1, src_table->monitors);
        }

        /* If removed components were overrides, run OnSet systems for those, as 
         * the value of those components changed from the removed component to 
         * the value of component on the base entity */
        if (removed && dst_table->flags & EcsTableHasBase) {
            ecs_run_monitors(world, stage, dst_table, src_table->on_set_override, 
                dst_row, 1, dst_table->on_set_override);          
        }
    }

    info->data = dst_data;

    return dst_row;
}

static
void delete_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_entities_t *removed)
{
    if (removed) {
        ecs_run_monitors(world, stage, src_table, src_table->un_set_all, 
            src_row, 1, NULL);

        /* Invoke remove actions before deleting */
        if (src_table->flags & EcsTableHasRemoveActions) {   
            ecs_run_remove_actions(
                world, stage, src_table, src_data, src_row, 1, removed, true);
        } 
    }

    ecs_table_delete(world, stage, src_table, src_data, src_row);
}

/* Updating component monitors is a relatively expensive operation that only
 * happens for entities that are monitored. The approach balances the amount of
 * processing between the operation on the entity vs the amount of work that
 * needs to be done to rematch queries, as a simple brute force approach does
 * not scale when there are many tables / queries. Therefore we need to do a bit
 * of bookkeeping that is more intelligent than simply flipping a flag */
static
bool update_component_monitor_w_array(
    ecs_component_monitor_t *mon,
    ecs_entities_t *entities)
{
    bool childof_changed = false;

    if (!entities) {
        return false;
    }

    int i;
    for (i = 0; i < entities->count; i ++) {
        ecs_entity_t component = entities->array[i];
        if (component < ECS_HI_COMPONENT_ID) {
            ecs_component_monitor_mark(mon, component);
        } else if (component & ECS_CHILDOF) {
            childof_changed = true;
        }
    }

    return childof_changed;
}

static
void update_component_monitors(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *added,
    ecs_entities_t *removed)
{
    bool childof_changed = update_component_monitor_w_array(
        &world->component_monitors, added);

    childof_changed |= update_component_monitor_w_array(
        &world->component_monitors, removed);

    /* If this entity is a parent, check if anything changed that could impact
     * its place in the hierarchy. If so, we need to mark all of the parent's
     * entities as dirty. */
    if (childof_changed && 
        ecs_map_get(world->child_tables, ecs_vector_t*, entity)) 
    {
        ecs_type_t type = ecs_get_type(world, entity);
        ecs_entities_t entities = ecs_type_to_entities(type);
        update_component_monitor_w_array(&world->parent_monitors, &entities);
    }
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

    if (src_table == dst_table && &world->stage == stage) {
        /* If source and destination table are the same, and stage is main stage
         * no action is required. When this is not the main stage, this can
         * happen when an entity is copied from the main stage table to the
         * same table in the stage, for example when ecs_set is invoked. */
        return;
    }

    if (src_table) {
        ecs_data_t *src_data = info->data;
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Only move entity when it is not moved to the root table, unless we're
         * iterating. In this case the entities need to be kept around so that
         * the merge knows to remove them from their previous tables. */
        if (dst_table->type || in_progress) {            
            info->row = move_entity(world, stage, entity, info, src_table, 
                src_data, info->row, dst_table, added, removed);
            info->table = dst_table;
        } else {
            delete_entity(
                world, stage, src_table, src_data, info->row, 
                removed);

            ecs_eis_set(stage, entity, &(ecs_record_t){
                NULL, -info->is_watched
            });
        }      
    } else {        
        if (dst_table->type) {
            info->row = new_entity(world, stage, entity, info, dst_table, added);
            info->table = dst_table;
        }        
    }

    /* If the entity is being watched, it is being monitored for changes and
    * requires rematching systems when components are added or removed. This
    * ensures that systems that rely on components from containers or prefabs
    * update the matched tables when the application adds or removes a 
    * component from, for example, a container. */
    if (info->is_watched) {
        update_component_monitors(world, entity, added, removed);
    }

    if ((!src_table || !src_table->type) && stage->range_check_enabled) {
        ecs_assert(!world->stats.max_id || entity <= world->stats.max_id, ECS_OUT_OF_RANGE, 0);
        ecs_assert(entity >= world->stats.min_id, ECS_OUT_OF_RANGE, 0);
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
    ecs_entity_t *type_buffer = ecs_vector_first(type, ecs_entity_t);
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
    ecs_entity_info_t info = {0};
    ecs_table_t *table = ecs_table_traverse_add(
        world, stage, stage->scope_table, to_add, NULL);
    new_entity(world, stage, entity, &info, table, to_add);
}

static
int32_t new_w_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *component_ids,
    int32_t count,
    void **component_data)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_type_t type = table->type;
    ecs_entity_t e = world->stats.last_id + 1;
    world->stats.last_id += ecs_to_entity(count);

    if (!type) {
        return 0;
    }

    ecs_entities_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = ecs_vector_first(type, ecs_entity_t);
        component_array.count = ecs_vector_count(type);
    }

    ecs_data_t *data = ecs_table_get_or_create_data(world, stage, table);
    int32_t row = ecs_table_grow(world, table, data, count, e);
    ecs_entities_t added = ecs_type_to_entities(type);
    
    /* Update entity index. If entities are being created in the main stage,
     * set the record pointers in the table. */
    int i;
    if (stage == &world->stage) {
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
        for (i = 0; i < count; i ++) { 
            record_ptrs[row + i] = ecs_eis_set(stage, e + ecs_to_entity(i), 
            &(ecs_record_t){
                .table = table,
                .row = row + i + 1
            });
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_eis_set(stage, e + ecs_to_entity(i), &(ecs_record_t){
                .table = table,
                .row = row + i + 1
            });
        }
    }
    
    ecs_defer_begin(world, stage, EcsOpNone, 0, 0, NULL, 0);

    ecs_comp_mask_t set_mask = { 0 };
    ecs_run_add_actions(world, stage, table, data, row, count, &added, set_mask, 
        true, component_data == NULL);

    if (component_data) {
        /* Set components that we're setting in the component mask so the init
         * actions won't call OnSet triggers for them. This ensures we won't
         * call OnSet triggers multiple times for the same component */
        int32_t c_i;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            ecs_entity_t c = component_ids->array[c_i];
            if (c >= ECS_HI_COMPONENT_ID) {
                break;
            }
            
            comp_mask_set(set_mask, c);

            /* Copy component data */
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            /* Bulk copy column data into new table */
            int32_t table_index = ecs_type_index_of(type, c);
            ecs_column_t *column = &data->columns[table_index];
            int16_t size = column->size;
            int16_t alignment = column->alignment;
            void *ptr = ecs_vector_first_t(column->data, size, alignment);
            ptr = ECS_OFFSET(ptr, size * row);

            ecs_c_info_t *cdata = ecs_get_c_info(world, c);
            ecs_copy_t copy = cdata->lifecycle.copy;
            if (copy) {
                ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
                copy(world, c, entities, entities, ptr, src_ptr, 
                    ecs_to_size_t(size), count, cdata->lifecycle.ctx);
            } else {
                ecs_os_memcpy(ptr, src_ptr, size * count);
            }
        };

        ecs_run_set_systems(world, stage, &added, 
            table, data, row, count, true);        
    }

    ecs_run_monitors(world, stage, table, table->monitors, row, count, NULL);

    ecs_defer_end(world, stage);

    return row;
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
    ecs_entities_t *to_remove)
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

    if (ecs_defer_begin(world, stage, EcsOpAdd, entity, components, NULL, 0)) {
        return;
    }

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, stage, src_table, components, &added);

    commit(world, stage, entity, &info, dst_table, &added, NULL);

    ecs_defer_end(world, stage);
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

    if (ecs_defer_begin(world, stage, EcsOpRemove, entity, components, NULL, 0)) {
        return;
    }

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, components, &removed);

    commit(world, stage, entity, &info, dst_table, NULL, &removed);

    ecs_defer_end(world, stage);
}

static
void *get_mutable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_entity_info_t *info,
    bool *is_added)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((component & ECS_ENTITY_MASK) == component || component & ECS_TRAIT, ECS_INVALID_PARAMETER, NULL);

    void *dst = NULL;
    if (stage == &world->stage) {
        if (get_info(world, entity, info)) {
            dst = get_component(info, component);
        }
    } else {
        if (get_staged_info(world, stage, entity, info)) {
            dst = get_component(info, component);
        } else {
            /* If the entity isn't stored in the current stage, we still need to
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

        /* Reobtain info, as triggers could have changed the entity */
        ecs_get_info(world, stage, entity, info);

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

ecs_entities_t ecs_type_to_entities(
    ecs_type_t type)
{
    return (ecs_entities_t){
        .array = ecs_vector_first(type, ecs_entity_t),
        .count = ecs_vector_count(type)
    };
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

ecs_entity_t ecs_find_in_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t flags)
{
    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        ecs_entity_t c = *c_ptr;

        if (flags) {
            if ((c & flags) != flags) {
                continue;
            }
        }

        ecs_entity_t e = c & ECS_ENTITY_MASK;

        if (component) {
           ecs_type_t component_type = ecs_get_type(world, e);
           if (!ecs_type_has_entity(world, component_type, component)) {
               continue;
           }
        }

        return e;
    });

    return 0;
}


/* -- Public functions -- */

ecs_entity_t ecs_new_id(
    ecs_world_t *world)
{
    ecs_entity_t entity;

    if (!world->in_progress) {
        if (!(entity = ecs_eis_recycle(&world->stage))) {
            entity = ++ world->stats.last_id;
        }
    } else {
        int32_t thread_count = ecs_vector_count(world->workers);
        if (thread_count >= 1) { 
            /* Can't atomically increase number above max int */
            ecs_assert(
                world->stats.last_id < UINT_MAX, ECS_INTERNAL_ERROR, NULL);

            entity = (ecs_entity_t)ecs_os_ainc((int32_t*)&world->stats.last_id);
        } else {
            entity = ++ world->stats.last_id;
        } 
    }

    ecs_assert(!world->stats.max_id || entity <= world->stats.max_id, 
        ECS_OUT_OF_RANGE, NULL);

    return entity;
}

ecs_entity_t ecs_new_component_id(
    ecs_world_t *world)
{
    /* Cannot issue new component ids while world is in progress */
    ecs_assert(!world->in_progress, ECS_INVALID_PARAMETER, NULL);

    if (world->stats.last_component_id < ECS_HI_COMPONENT_ID) {
        return world->stats.last_component_id ++;
    } else {
        /* If the low component ids are depleted, return a regular entity id */
        return ecs_new_id(world);
    }
}

ecs_entity_t ecs_new_w_type(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    if (type || stage->scope) {
        ecs_entities_t to_add = ecs_type_to_entities(type);
        new(world, stage, entity, &to_add);
    } else {
        ecs_eis_set(&world->stage, entity, &(ecs_record_t){ 0 });
    }

    return entity;
}

ecs_entity_t ecs_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    if (component || stage->scope) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        ecs_entity_t old_scope = 0;
        if (component & ECS_CHILDOF) {
            old_scope = ecs_set_scope(world, 0);
        }

        new(world, stage, entity, &to_add);

        if (component & ECS_CHILDOF) {
            ecs_set_scope(world, old_scope);
        }
    } else {
        ecs_eis_set(&world->stage, entity, &(ecs_record_t){ 0 });
    }

    return entity;
}

ecs_entity_t ecs_bulk_new_w_data(
    ecs_world_t *world,
    int32_t count,
    ecs_entities_t *component_ids,
    void *data)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_type_t type = ecs_type_find(world, 
        component_ids->array, component_ids->count);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    ecs_entity_t result = world->stats.last_id + 1;
    new_w_data(world, stage, table, NULL, count, data);
    return result;
}

ecs_entity_t ecs_bulk_new_w_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    ecs_entity_t result = world->stats.last_id + 1;
    new_w_data(world, stage, table, NULL, count, NULL);
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
    ecs_entity_t result = world->stats.last_id + 1;
    new_w_data(world, stage, table, NULL, count, NULL);
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

    ecs_delete_children(world, entity);

    /* If entity has components, remove them */
    ecs_table_t *table = info.table;
    if (table) {
        ecs_type_t type = table->type;

        /* Remove all components */
        ecs_entities_t to_remove = ecs_type_to_entities(type);
        remove_entities_w_info(world, stage, entity, &info, &to_remove);
    }

    /* If this is a staged delete, set the table in the staged entity index to
     * NULL. That way the merge will know to delete this entity vs. just to
     * remove its components */
    if (stage != &world->stage) {
        if (!table) {
            /* If entity was empty, add it to the root table so its id will be
             * recycled when merging the stage */
            table = &stage->root;
            ecs_data_t *data = ecs_table_get_or_create_data(world, stage, table);
            ecs_table_append(world, table, data, entity, NULL);
        }
        ecs_eis_set(stage, entity, &(ecs_record_t){ NULL, 0 });
    } else {
        ecs_eis_delete(stage, entity);
    }
}

void ecs_add_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = ecs_type_to_entities(type);
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
    ecs_entities_t components = ecs_type_to_entities(type);
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
    add_remove(world, entity, &components_add, &components_remove);
}

void ecs_add_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_entities_t components_add = ecs_type_to_entities(to_add);
    ecs_entities_t components_remove = ecs_type_to_entities(to_remove);
    add_remove(world, entity, &components_add, &components_remove);
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    if (!dst) {
        dst = ++ world->stats.last_id;
    }

    ecs_entity_info_t src_info;
    bool found = ecs_get_info(world, stage, src, &src_info);
    ecs_table_t *src_table = src_info.table;

    if (!found || !src_table) {
        return dst;
    }

    ecs_type_t src_type = src_table->type;
    ecs_entities_t to_add = ecs_type_to_entities(src_type);

    ecs_entity_info_t dst_info = {0};
    dst_info.row = new_entity(
        world, stage, dst, &dst_info, src_table, &to_add);

    if (copy_value) {
        ecs_table_move(world, dst, src, src_table, dst_info.data, dst_info.row,
            src_table, src_info.data, src_info.row, true);

        int i;
        for (i = 0; i < to_add.count; i ++) {
            ecs_entity_t component = to_add.array[i];
            ecs_c_info_t *cdata = ecs_get_c_info(world, component);
            if (!cdata) {
                continue;
            }

            ecs_run_set_systems(world, stage, &to_add, 
                src_table, src_info.data, dst_info.row, 1, true);
        }
    }    

    return dst;
}

const void* ecs_get_w_entity(
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
            if (component != ecs_entity(EcsName) && component != EcsPrefab) {
                ptr = get_base_component(
                    world, stage, entity, &info, 0, component);
            }
        }        
    }

    return ptr;
}

const void* ecs_get_ref_w_entity(
    ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!entity || !ref->entity || entity == ref->entity, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!component || !ref->component || component == ref->component, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_record_t *record = ref->record;

    entity |= ref->entity;

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

    if (ref->stage == stage &&
        ref->record == record &&
        ref->table == table &&
        ref->row == record->row &&
        ref->alloc_count == table->alloc_count)
    {
        return ref->ptr;
    }

    component |= ref->component;

    ref->entity = entity;
    ref->component = component;
    ref->stage = stage;
    ref->table = table;
    ref->row = record->row;
    ref->alloc_count = table->alloc_count;

    ecs_entity_info_t info = {0};
    set_info_from_record(world, &info, record);
    ref->ptr = get_component(&info, component);

    if (&world->stage == stage) {
        ref->record = record;
    } else {
        ref->record = NULL;
    }

    return ref->ptr;
}

void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool *is_added)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;
    return get_mutable(world, stage, entity, component, &info, is_added);
}

void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert((component & ECS_ENTITY_MASK) == component, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info = {0};

    if (ecs_get_info(world, stage, entity, &info)) {
        ecs_entities_t added = {
            .array = &component,
            .count = 1
        };
        ecs_run_set_systems(world, stage, &added, 
            info.table, info.data, info.row, 1, false);
    }
}

ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{    
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entities_t added = {
        .array = &component,
        .count = 1
    };

    if (!entity) {
        entity = ecs_new_id(world);
        ecs_entity_t scope = stage->scope;
        if (scope) {
            ecs_add_entity(world, entity, ECS_CHILDOF | scope);
        }
    }

    if (ecs_defer_begin(world, stage, EcsOpSet, entity, &added, ptr, 
        ecs_from_size_t(size))) 
    {
        return entity;
    }

    ecs_entity_info_t info;
    void *dst = get_mutable(world, stage, entity, component, &info, NULL);

    /* This can no longer happen since we defer operations */
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ptr) {
        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        ecs_copy_t copy;

        if (cdata && (copy = cdata->lifecycle.copy)) {
            copy(world, component, &entity, &entity, dst, ptr, size, 1, 
                cdata->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst, ptr, ecs_from_size_t(size));
        }
    } else {
        memset(dst, 0, size);
    }

    ecs_table_mark_dirty(info.table, component);

    ecs_run_set_systems(world, stage, &added, 
        info.table, info.data, info.row, 1, false);

    ecs_defer_end(world, stage);

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
    ecs_type_t type = ecs_get_type(world, entity);    
    ecs_entity_t parent = ecs_find_in_type(world, type, component, ECS_CHILDOF);
    return parent;
}

const char* ecs_get_name(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    if (entity == EcsSingleton) {
        return "$";
    }

    const EcsName *id = ecs_get(world, entity, EcsName);

    if (id) {
        return id->value;
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

    const EcsType *type = ecs_get(world, entity, EcsType);
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

    return *(ecs_vector_first(type, ecs_entity_t));
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

/* Enter safe section. Record all operations so they can be executed after
 * leaving the safe section. */
bool ecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entities_t *components,
    const void *value,
    ecs_size_t size)
{
    (void)world;
    
    if (stage->defer && op_kind != EcsOpNone) {
        if (!components->count) {
            return true;
        }

        ecs_op_t *op = ecs_vector_add(&stage->defer_queue, ecs_op_t);
        op->kind = op_kind;
        op->entity = entity;
        if (components->count == 1) {
            op->component = components->array[0];
            op->components = (ecs_entities_t) {
                .array = NULL,
                .count = 1
            };
        } else {
            ecs_size_t array_size = components->count * ECS_SIZEOF(ecs_entity_t);
            op->components.array = ecs_os_malloc(array_size);
            ecs_os_memcpy(op->components.array, components->array, array_size);
            op->components.count = components->count;
        }

        if (size) {
            ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
            op->value = ecs_os_memdup(value, size);
        } else {
            op->value = NULL;
        }

        op->size = size;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

/* Leave safe section. Run all deferred commands. */
void ecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    if (!--stage->defer) {
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;
        if (defer_queue) {
            ecs_vector_each(defer_queue, ecs_op_t, op, {
                if (op->components.count == 1) {
                    op->components.array = &op->component;
                }

                switch(op->kind) {
                case EcsOpNone:
                    break;
                case EcsOpAdd:
                    add_entities(world, op->entity, &op->components);
                    break;
                case EcsOpRemove:
                    remove_entities(world, op->entity, &op->components);
                    break;
                case EcsOpSet:
                    ecs_set_ptr_w_entity(world, op->entity, 
                        op->components.array[0], ecs_to_size_t(op->size), 
                        op->value);
                    break;
                }

                if (op->components.count > 1) {
                    ecs_os_free(op->components.array);
                }

                if (op->value) {
                    ecs_os_free(op->value);
                }
            });

            ecs_vector_free(defer_queue);
        }
    }
}


