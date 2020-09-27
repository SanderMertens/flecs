
#include "private_api.h"

static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *component_ids,
    int32_t count,
    void **c_info,
    int32_t *row_out);

static
int32_t comp_mask_index(
    int32_t value)
{
    return value >> 6;
}

static
void comp_mask_set(
    ecs_comp_set_t *set,
    ecs_entity_t value)
{
    if (value < ECS_HI_COMPONENT_ID) {
        int32_t index = comp_mask_index((int32_t)value);
        set->lo_mask[index] |= (ecs_entity_t)1 << (value & 0x3F);
    } else {
        int32_t count = set->hi_count++;
        set->hi_array[count] = value;
    }
}

static
bool comp_mask_is_set(
    ecs_comp_set_t *set,
    ecs_entity_t value)
{
    if (value < ECS_HI_COMPONENT_ID) {
        int32_t index = comp_mask_index((int32_t)value);
        return (set->lo_mask[index] & (ecs_entity_t)1 << (value & 0x3F)) != 0;
    } else {
        int32_t i, count = set->hi_count;
        for (i = 0; i < count; i ++) {
            if (set->hi_array[i] == value) {
                return true;
            }
        }
    }

    return false;
}

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
    ecs_entity_t e,
    ecs_entity_info_t *info,
    ecs_record_t *record)
{
    (void)e;
    
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

    info->record = record;

    ecs_table_t *table = record->table;

    set_row_info(info, record->row);

    info->table = table;
    if (!info->table) {
        return;
    }

    ecs_data_t *data = ecs_table_get_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    info->data = data;

    ecs_assert(ecs_vector_count(data->entities) > info->row, 
        ECS_INTERNAL_ERROR, NULL);
}

/* Get info from main stage */
bool ecs_get_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_record_t *record = ecs_eis_get(world, entity);

    if (!record) {
        info->table = NULL;
        info->is_watched = false;
        info->record = NULL;
        return false;
    }

    set_info_from_record(entity, info, record);

    return true;
}

static
ecs_c_info_t *get_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_entity_t real_id = ecs_component_id_from_id(world, component);
    if (real_id) {
        return ecs_get_c_info(world, real_id);
    } else {
        return NULL;
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
        int32_t i, count = ecs_vector_count(table->type);
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = type_array[i];
            cinfo[i].id = id;
            cinfo[i].ci = get_c_info(world, id);
            cinfo[i].column = i;            
        }
    } else {
        ecs_entity_t *array = components->array;
        int32_t i, cur, count = components->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = array[i];
            cinfo[i].id = id;
            cinfo[i].ci = get_c_info(world, id);
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

        /* If this is a tag, don't try to retrieve data */
        if (table->column_count < index) {
            columns[0] = 0;
        } else {
            ecs_column_t *column = &data->columns[index - 1];
            if (!column->size) {
                columns[0] = 0;
            }            
        }

        ecs_iter_t it = {
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
            it.system = triggers[i].self;
            it.param = triggers[i].ctx;
            triggers[i].action(&it);
        }
    }
}

void ecs_run_component_trigger(
    ecs_world_t *world,
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
        world, trigger_vec, component, table, data, row, count, entities);
}

#ifdef FLECS_SYSTEM 
static
void run_set_systems_for_entities(
    ecs_world_t *world,
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
            ecs_run_monitor(world, m, components, row, count, entities);
        });
    } else {
        ecs_vector_t **on_set_systems = table->on_set;
        if (on_set_systems) {
            int32_t index = ecs_type_index_of(table->type, components->array[0]);
            ecs_vector_t *queries = on_set_systems[index];
            ecs_vector_each(queries, ecs_matched_query_t, m, {
                ecs_run_monitor(world, m, components, row, count, entities);
            });
        }
    }
}
#endif

void ecs_run_set_systems(
    ecs_world_t *world,
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

    run_set_systems_for_entities(world, components, table, row, 
        count, entities, set_all);
#endif
}

void ecs_run_monitors(
    ecs_world_t *world, 
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
            ecs_run_monitor(world, monitor, NULL, dst_row, count, NULL);
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

            ecs_run_monitor(world, dst, NULL, dst_row, count, NULL);
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
        if (ECS_HAS_ROLE(e, INSTANCEOF)) {
            return i;
        }
    }

    return -1;
}

static
void instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

static
void instantiate_children(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_t *child_table)
{
    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = ecs_table_get_data(child_table);
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
        if (ECS_HAS_ROLE(c, CHILDOF) && (c & ECS_COMPONENT_MASK) == base) {
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
        ecs_table_t *table = ecs_table_find_or_create(world, &components);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create children */
        int32_t child_row; 
        new_w_data(world, table, NULL, child_count, c_info, &child_row);

        /* If prefab child table has children itself, recursively instantiate */
        ecs_data_t *i_data = ecs_table_get_data(table);
        ecs_entity_t *children = ecs_vector_first(child_data->entities, ecs_entity_t);

        int j;
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, child, i_data, child_row + j, 1);
        }
    }    
}

static
void instantiate(
    ecs_world_t *world,
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
                world, base, data, row, count, child_table);
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

    if (!ecs_get_info(world, base, &base_info)) {
        return false;
    }

    void *base_ptr = get_component(&base_info, component);
    if (base_ptr) {
        int16_t data_size = column->size;
        void *data_array = ecs_vector_first_t(
            column->data, column->size, column->alignment);
        void *data_ptr = ECS_OFFSET(data_array, data_size * row);
        component = ecs_component_id_from_id(world, component);
        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        int32_t index;

        ecs_copy_t copy = cdata ? cdata->lifecycle.copy : NULL;
        if (copy) {
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);

            void *ctx = cdata->lifecycle.ctx;
            for (index = 0; index < count; index ++) {
                copy(world, component, &entities[row], &base,
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

        if (ECS_HAS_ROLE(e, INSTANCEOF)) {
            return override_from_base(world, e & ECS_COMPONENT_MASK, component, 
                data, column, row, count);
        }
    } while (--i >= 0);

    return false;
}

void ecs_components_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count,
    ecs_comp_set_t *set_mask,
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
            if (ECS_HAS_ROLE(component, INSTANCEOF)) {
                ecs_entity_t base = component & ECS_COMPONENT_MASK;
                instantiate(world, base, data, row, count);

                /* If table has on_set systems, get table without the base
                 * entity that was just added. This is needed to determine the
                 * diff between the on_set systems of the current table and the
                 * table without the base, as these are the systems that need to
                 * be invoked */
                ecs_entities_t to_remove = {
                    .array = &component,
                    .count = 1
                };
                table_without_base = ecs_table_traverse_remove(world, 
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
                    table_without_base, &to_remove, NULL);
            }
        }
    }

    /* Run OnSet actions when a base entity is added to the entity for 
     * components not overridden by the entity. */
    if (run_on_set && table_without_base != table) {
        ecs_run_monitors(world, table, table->on_set_all, row, count, 
            table_without_base->on_set_all);
    }
}

void ecs_components_switch(
    ecs_world_t *world,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *added)
{
    (void)world;

    ecs_entity_t *array = added->array;
    int32_t i, add_count = added->count;

    for (i = 0; i < add_count; i ++) {
        ecs_entity_t e = array[i];

        if (ECS_HAS_ROLE(e, CASE)) {
            e = e & ECS_COMPONENT_MASK;

            ecs_entity_t sw_case = ecs_entity_t_lo(e);
            ecs_entity_t sw_index = ecs_entity_t_hi(e);

            ecs_switch_t *sw = data->sw_columns[sw_index].data;
            int32_t r;
            for (r = 0; r < count; r ++) {
                ecs_switch_set(sw, row + r, sw_case);
            }
        }
    }
}

void ecs_components_on_add(
    ecs_world_t *world,
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
            world, triggers, component, table, data, row, count);
    }
}

void ecs_components_on_remove(
    ecs_world_t *world,
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
            world, triggers, component, table, data, row, count);
    }
}

void ecs_run_add_actions(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *added,
    ecs_comp_set_t *set_mask,
    bool get_all,
    bool run_on_set)
{
    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(added->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);

    ecs_column_info_t cinfo[ECS_MAX_ADD_REMOVE];
    ecs_get_column_info(world, table, added, cinfo, get_all);
    int added_count = added->count;

    if (table->flags & EcsTableHasBase) {
        ecs_components_override(
            world, table, data, row, count, cinfo, 
            added_count, set_mask, run_on_set);
    }

    if (table->flags & EcsTableHasSwitch) {
        ecs_components_switch(
            world, data, row, count, added);
    }

    if (table->flags & EcsTableHasOnAdd) {
        ecs_components_on_add(world, table, data, row, count, 
            cinfo, added_count);
    }
}

void ecs_run_remove_actions(
    ecs_world_t *world,
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
        ecs_components_on_remove(world, table, data, 
            row, count, cinfo, removed_count);
    }   
}

static
int32_t new_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *new_table,
    ecs_entities_t *added)
{
    ecs_record_t *record = info->record;
    ecs_data_t *new_data = ecs_table_get_or_create_data(new_table);
    int32_t new_row;

    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!record) {
        record = ecs_eis_get_or_create(world, entity);
    }

    new_row = ecs_table_append(
        world, new_table, new_data, entity, record, true);

    record->table = new_table;
    record->row = ecs_row_to_record(new_row, info->is_watched);

    ecs_assert(
        ecs_vector_count(new_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);    

    if (new_table->flags & EcsTableHasAddActions) {
        ecs_comp_set_t set_mask = {0};
        
        ecs_run_add_actions(
            world, new_table, new_data, new_row, 1, added, &set_mask, 
            true, true);

        if (new_table->flags & EcsTableHasMonitors) {
            ecs_run_monitors(
                world, new_table, new_table->monitors, new_row, 1, NULL);              
        }        
    }

    info->data = new_data;
    
    return new_row;
}

static
int32_t move_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_table_t *dst_table,
    ecs_entities_t *added,
    ecs_entities_t *removed)
{    
    ecs_data_t *dst_data = ecs_table_get_or_create_data(dst_table);
    if (src_data == dst_data) {
        return src_row;
    }

    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = info->record;
    ecs_assert(!record || record == ecs_eis_get(world, entity), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t dst_row = ecs_table_append(world, dst_table, dst_data, entity, 
        record, false);

    record->table = dst_table;
    record->row = ecs_row_to_record(dst_row, info->is_watched);

    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        ecs_table_move(world, entity, entity, dst_table, dst_data, dst_row, 
            src_table, src_data, src_row);

        /* If components were removed, invoke remove actions before deleting */
        if (removed && (src_table->flags & EcsTableHasRemoveActions)) {
            /* If entity was moved, invoke UnSet monitors for each component that
            * the entity no longer has */
            ecs_run_monitors(world, dst_table, src_table->un_set_all, 
                dst_row, 1, dst_table->un_set_all);

            ecs_run_remove_actions(
                world, src_table, src_data, src_row, 1, removed, false);
        }            
    }
    
    ecs_table_delete(world, src_table, src_data, src_row, false);

    /* If components were added, invoke add actions */
    if (src_table != dst_table || (added && added->count)) {
        if (added && (dst_table->flags & EcsTableHasAddActions)) {
            ecs_comp_set_t set_mask = {0};
            ecs_run_add_actions(
                world, dst_table, dst_data, dst_row, 1, added, &set_mask, 
                false, true);
        }

        /* Run monitors */
        if (dst_table->flags & EcsTableHasMonitors) {
            ecs_run_monitors(world, dst_table, dst_table->monitors, dst_row, 
                1, src_table->monitors);
        }

        /* If removed components were overrides, run OnSet systems for those, as 
         * the value of those components changed from the removed component to 
         * the value of component on the base entity */
        if (removed && dst_table->flags & EcsTableHasBase) {
            ecs_run_monitors(world, dst_table, src_table->on_set_override, 
                dst_row, 1, dst_table->on_set_override);          
        }
    }

    info->data = dst_data;

    return dst_row;
}

static
void delete_entity(
    ecs_world_t *world,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_entities_t *removed)
{
    if (removed) {
        ecs_run_monitors(world, src_table, src_table->un_set_all, 
            src_row, 1, NULL);

        /* Invoke remove actions before deleting */
        if (src_table->flags & EcsTableHasRemoveActions) {   
            ecs_run_remove_actions(
                world, src_table, src_data, src_row, 1, removed, true);
        } 
    }

    ecs_table_delete(world, src_table, src_data, src_row, true);
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
        } else if (ECS_HAS_ROLE(component, CHILDOF)) {
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
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *dst_table,   
    ecs_entities_t *added,
    ecs_entities_t *removed)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *src_table = info->table;
    if (src_table == dst_table) {
        /* If source and destination table are the same no action is needed */
        /* However, if a component was added in the process of traversing a
         * table, this suggests that a case switch could have occured. */
        if (added && added->count && src_table && 
            src_table->flags & EcsTableHasSwitch) 
        {
            ecs_components_switch(world, info->data, info->row, 1, added);
        }
        return;
    }

    if (src_table) {
        ecs_data_t *src_data = info->data;
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (dst_table->type) { 
            info->row = move_entity(world, entity, info, src_table, 
                src_data, info->row, dst_table, added, removed);
            info->table = dst_table;
        } else {
            delete_entity(
                world, src_table, src_data, info->row, 
                removed);

            ecs_eis_set(world, entity, &(ecs_record_t){
                NULL, (info->is_watched == true) * -1
            });
        }      
    } else {        
        if (dst_table->type) {
            info->row = new_entity(world, entity, info, dst_table, added);
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

    if ((!src_table || !src_table->type) && world->range_check_enabled) {
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
        ecs_entity_t prefab = type_buffer[p] & ECS_COMPONENT_MASK;

        /* Detect cycles with two entities */
        if (prefab == previous) {
            continue;
        }

        ecs_entity_info_t prefab_info;
        if (ecs_get_info(world, prefab, &prefab_info) && prefab_info.table) {
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
    ecs_entity_t entity,
    ecs_entities_t *to_add)
{
    ecs_entity_info_t info = {0};
    ecs_table_t *table = ecs_table_traverse_add(
        world, world->stage.scope_table, to_add, NULL);
    new_entity(world, entity, &info, table, to_add);
}

static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *component_ids,
    int32_t count,
    void **component_data,
    int32_t *row_out)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);
    
    int32_t sparse_count = ecs_eis_count(world);
    const ecs_entity_t *ids = ecs_sparse_new_ids(world->store.entity_index, count);
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_type_t type = table->type;

    if (!type) {
        return ids;        
    }

    ecs_entities_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = ecs_vector_first(type, ecs_entity_t);
        component_array.count = ecs_vector_count(type);
    }

    ecs_data_t *data = ecs_table_get_or_create_data(table);
    int32_t row = ecs_table_appendn(world, table, data, count, ids);
    ecs_entities_t added = ecs_type_to_entities(type);
    
    /* Update entity index. */
    int i;
    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    for (i = 0; i < count; i ++) { 
        record_ptrs[row + i] = ecs_eis_set(world, ids[i], 
        &(ecs_record_t){
            .table = table,
            .row = row + i + 1
        });
    }

    ecs_defer_none(world, &world->stage);

    ecs_comp_set_t set_mask = { 0 };
    ecs_run_add_actions(world, table, data, row, count, &added, &set_mask, 
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
            
            comp_mask_set(&set_mask, c);

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
            ecs_copy_t copy;
            if (cdata && (copy = cdata->lifecycle.copy)) {
                ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
                copy(world, c, entities, entities, ptr, src_ptr, 
                    ecs_to_size_t(size), count, cdata->lifecycle.ctx);
            } else {
                ecs_os_memcpy(ptr, src_ptr, size * count);
            }
        };

        ecs_run_set_systems(world, &added, table, data, row, count, true);        
    }

    ecs_run_monitors(world, table, table->monitors, row, count, NULL);

    ecs_defer_flush(world, &world->stage);

    if (row_out) {
        *row_out = row;
    }

    ids = ecs_sparse_ids(world->store.entity_index);

    return &ids[sparse_count];
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

    ecs_entity_info_t info;
    ecs_get_info(world, entity, &info);

    ecs_entity_t add_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entity_t remove_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = add_buffer };
    ecs_entities_t removed = { .array = remove_buffer };

    ecs_table_t *src_table = info.table;

    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, src_table, to_remove, &removed);

    dst_table = ecs_table_traverse_add(
        world, dst_table, to_add, &added);    

    commit(world, entity, &info, dst_table, &added, &removed);
}

static
void add_entities_w_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, src_table, components, &added);

    commit(world, entity, info, dst_table, &added, NULL);
}

static
void remove_entities_w_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, src_table, components, &removed);

    commit(world, entity, info, dst_table, NULL, &removed);
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

    if (ecs_defer_add(world, stage, entity, components)) {
        return;
    }

    ecs_entity_info_t info;
    ecs_get_info(world, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, src_table, components, &added);

    commit(world, entity, &info, dst_table, &added, NULL);

    ecs_defer_flush(world, stage);
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

    if (ecs_defer_remove(world, stage, entity, components)) {
        return;
    }

    ecs_entity_info_t info;
    ecs_get_info(world, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, src_table, components, &removed);

    commit(world, entity, &info, dst_table, NULL, &removed);

    ecs_defer_flush(world, stage);
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
    ecs_assert((component & ECS_COMPONENT_MASK) == component || ECS_HAS_ROLE(component, TRAIT), ECS_INVALID_PARAMETER, NULL);

    void *dst = NULL;
    if (ecs_get_info(world, entity, info)) {
        dst = get_component(info, component);
    }

    ecs_table_t *table = info->table;

    if (!dst) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        add_entities_w_info(world, entity, info, &to_add);

        /* Reobtain info, as triggers could have changed the entity */
        ecs_get_info(world, entity, info);

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
    ecs_entity_t entity)
{    
    (void)world;

    ecs_record_t *record = ecs_eis_get(world, entity);
    if (!record) {
        ecs_record_t new_record = {.row = -1, .table = NULL};
        ecs_eis_set(world, entity, &new_record);
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

        ecs_entity_t e = c & ECS_COMPONENT_MASK;

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
        entity = ecs_eis_recycle(world);
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

    if (type || world->stage.scope) {
        ecs_entities_t to_add = ecs_type_to_entities(type);
        if (ecs_defer_new(world, stage, entity, &to_add)) {
            return entity;
        }
        new(world, entity, &to_add);
        ecs_defer_flush(world, stage);       
    } else {
        ecs_eis_set(world, entity, &(ecs_record_t){ 0 });
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

        if (ecs_defer_new(world, stage, entity, &to_add)) {
            return entity;
        }  

        ecs_entity_t old_scope = 0;
        if (ECS_HAS_ROLE(component, CHILDOF)) {
            old_scope = ecs_set_scope(world, 0);
        }

        new(world, entity, &to_add);

        if (ECS_HAS_ROLE(component, CHILDOF)) {
            ecs_set_scope(world, old_scope);
        }

        ecs_defer_flush(world, stage);
    } else {
        ecs_eis_set(world, entity, &(ecs_record_t){ 0 });
    }

    return entity;
}

const ecs_entity_t* ecs_bulk_new_w_data(
    ecs_world_t *world,
    int32_t count,
    ecs_entities_t *components,
    void *data)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    const ecs_entity_t *ids;
    if (ecs_defer_bulk_new(world, stage, count, components, data, &ids)) {
        return ids;
    }
    ecs_type_t type = ecs_type_find(world, components->array, components->count);
    ecs_table_t *table = ecs_table_from_type(world, type);    
    ids = new_w_data(world, table, NULL, count, data, NULL);
    ecs_defer_flush(world, stage);
    return ids;
}

const ecs_entity_t* ecs_bulk_new_w_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    const ecs_entity_t *ids;
    ecs_entities_t components = ecs_type_to_entities(type);
    if (ecs_defer_bulk_new(world, stage, count, &components, NULL, &ids)) {
        return ids;
    }
    ecs_table_t *table = ecs_table_from_type(world, type);
    ids = new_w_data(world, table, NULL, count, NULL, NULL);
    ecs_defer_flush(world, stage);
    return ids;
}

const ecs_entity_t* ecs_bulk_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entities_t components = {
        .array = &entity,
        .count = 1
    };
    const ecs_entity_t *ids;
    if (ecs_defer_bulk_new(world, stage, count, &components, NULL, &ids)) {
        return ids;
    }
    ecs_table_t *table = ecs_table_find_or_create(world, &components);
    ids = new_w_data(world, table, NULL, count, NULL, NULL);
    ecs_defer_flush(world, stage);
    return ids;
}

void ecs_clear(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    if (ecs_defer_clear(world, stage, entity)) {
        return;
    }

    ecs_entity_info_t info;
    info.table = NULL;

    ecs_get_info(world, entity, &info);

    ecs_table_t *table = info.table;
    if (table) {
        ecs_type_t type = table->type;

        /* Remove all components */
        ecs_entities_t to_remove = ecs_type_to_entities(type);
        remove_entities_w_info(world, entity, &info, &to_remove);
    }    

    ecs_defer_flush(world, stage);
}

void ecs_delete_children(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, parent);

    if (child_tables) {
        ecs_table_t **tables = ecs_vector_first(child_tables, ecs_table_t*);
        int32_t i, count = ecs_vector_count(child_tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = tables[i];

            /* Recursively delete entities of children */
            ecs_data_t *data = ecs_table_get_data(table);
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);

            int32_t child, child_count = ecs_vector_count(data->entities);
            for (child = 0; child < child_count; child ++) {
                ecs_delete_children(world, entities[child]);
            }

            /* Clear components from table (invokes destructors, OnRemove) */
            ecs_table_clear(world, table);

            /* Delete table */
            ecs_delete_table(world, table);
        };
    }
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);

    if (ecs_defer_delete(world, stage, entity)) {
        return;
    }

    ecs_entity_info_t info;
    info.table = NULL;

    ecs_get_info(world, entity, &info);
    if (info.is_watched) {
        ecs_delete_children(world, entity);
    }

    /* If entity has components, remove them */
    ecs_table_t *table = info.table;
    if (table) {
        ecs_type_t type = table->type;

        /* Remove all components */
        ecs_entities_t to_remove = ecs_type_to_entities(type);
        remove_entities_w_info(world, entity, &info, &to_remove);
    }

    ecs_eis_delete(world, entity);

    ecs_defer_flush(world, stage);
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
    
    if (!dst) {
        dst = ecs_new_id(world);
    }

    if (ecs_defer_clone(world, stage, dst, src, copy_value)) {
        return dst;
    }

    ecs_entity_info_t src_info;
    bool found = ecs_get_info(world, src, &src_info);
    ecs_table_t *src_table = src_info.table;

    if (!found || !src_table) {
        return dst;
    }

    ecs_type_t src_type = src_table->type;
    ecs_entities_t to_add = ecs_type_to_entities(src_type);

    ecs_entity_info_t dst_info = {0};
    dst_info.row = new_entity(world, dst, &dst_info, src_table, &to_add);

    if (copy_value) {
        ecs_table_move(world, dst, src, src_table, dst_info.data, 
            dst_info.row, src_table, src_info.data, src_info.row);

        int i;
        for (i = 0; i < to_add.count; i ++) {
            ecs_run_set_systems(world, &to_add, 
                src_table, src_info.data, dst_info.row, 1, true);
        }
    }

    ecs_defer_flush(world, stage);

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

    bool found = ecs_get_info(world, entity, &info);
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
    ecs_record_t *record = ref->record;

    entity |= ref->entity;

    if (!record) {
        record = ecs_eis_get(world, entity);
    }

    if (!record || !record->table) {
        return NULL;
    }

    ecs_table_t *table = record->table;

    if (ref->record == record &&
        ref->table == table &&
        ref->row == record->row &&
        ref->alloc_count == table->alloc_count)
    {
        return ref->ptr;
    }

    component |= ref->component;

    ref->entity = entity;
    ref->component = component;
    ref->table = table;
    ref->row = record->row;
    ref->alloc_count = table->alloc_count;

    ecs_entity_info_t info = {0};
    set_info_from_record(entity, &info, record);
    ref->ptr = get_component(&info, component);
    ref->record = record;

    return ref->ptr;
}

void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool *is_added)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    void *result;

    if (ecs_defer_set(
        world, stage, EcsOpMut, entity, component, 0, NULL, &result, is_added))
    {
        return result;
    }

    ecs_entity_info_t info;
    result = get_mutable(world, entity, component, &info, is_added);
    
    ecs_defer_flush(world, stage);

    return result;
}

void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert((component & ECS_COMPONENT_MASK) == component, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);

    if (ecs_defer_modified(world, stage, entity, component)) {
        return;
    }

    ecs_entity_info_t info = {0};
    if (ecs_get_info(world, entity, &info)) {
        ecs_entities_t added = {
            .array = &component,
            .count = 1
        };
        ecs_run_set_systems(world, &added, 
            info.table, info.data, info.row, 1, false);
    }
    
    ecs_defer_flush(world, stage);
}

static
ecs_entity_t assign_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    void *ptr,
    bool is_move,
    bool notify)
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

    if (ecs_defer_set(world, stage, EcsOpSet, entity, component, 
        ecs_from_size_t(size), ptr, NULL, NULL))
    {
        return entity;
    }

    ecs_entity_info_t info;
    void *dst = get_mutable(world, entity, component, &info, NULL);

    /* This can no longer happen since we defer operations */
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ptr) {
        ecs_entity_t real_id = ecs_component_id_from_id(world, component);
        ecs_c_info_t *cdata = get_c_info(world, real_id);
        if (cdata) {
            if (is_move) {
                ecs_move_t move = cdata->lifecycle.move;
                if (move) {
                    move(world, real_id, &entity, &entity, dst, ptr, size, 1, 
                        cdata->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, ptr, ecs_from_size_t(size));
                }
            } else {
                ecs_copy_t copy = cdata->lifecycle.copy;
                if (copy) {
                    copy(world, real_id, &entity, &entity, dst, ptr, size, 1, 
                        cdata->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, ptr, ecs_from_size_t(size));
                }
            }
        } else {
            ecs_os_memcpy(dst, ptr, ecs_from_size_t(size));
        }
    } else {
        memset(dst, 0, size);
    }

    ecs_table_mark_dirty(info.table, component);

    if (notify) {
        ecs_run_set_systems(world, &added, 
            info.table, info.data, info.row, 1, false);
    }

    ecs_defer_flush(world, stage);

    return entity;
}

ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{
    /* Safe to cast away const: function won't modify if move arg is false */
    return assign_ptr_w_entity(
        world, entity, component, size, (void*)ptr, false, true);
}

ecs_entity_t ecs_get_case(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t sw_id)
{
    ecs_entity_info_t info;
    if (!ecs_get_info(world, entity, &info) || !info.table) {
        return 0;
    }

    sw_id = sw_id | ECS_SWITCH;

    ecs_table_t *table = info.table;
    ecs_data_t *data = info.data;
    if (!data) {
        return 0;
    }

    ecs_type_t type = table->type;
    int32_t index = ecs_type_index_of(type, sw_id);
    if (index == -1) {
        return 0;
    }

    index -= table->sw_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_switch_t *sw = data->sw_columns[index].data;  
    return ecs_switch_get(sw, info.row);  
}

bool ecs_has_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    if (ECS_HAS_ROLE(component, CASE)) {
        ecs_entity_info_t info;
        if (!ecs_get_info(world, entity, &info)) {
            return false;
        }

        ecs_table_t *table = info.table;
        int32_t index = ecs_table_switch_from_case(world, table, component);
        ecs_assert(index < table->sw_column_count, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = info.data;
        ecs_switch_t *sw = data->sw_columns[index].data;
        ecs_entity_t value = ecs_switch_get(sw, info.row);

        return value == (component & ECS_COMPONENT_MASK);
    } else {
        ecs_type_t type = ecs_get_type(world, entity);
        return ecs_type_has_entity(world, type, component);
    }
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

bool ecs_is_alive(
    ecs_world_t *world,
    ecs_entity_t e)
{
    return ecs_eis_is_alive(world, e);
}

ecs_type_t ecs_get_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_record_t *record = NULL;

    record = ecs_eis_get(world, entity);

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

    ecs_sparse_t *tables = world->store.tables;
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

void ecs_defer_begin(
    ecs_world_t *world)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    
    if (world->in_progress) {
        ecs_stage_defer_begin(world, stage);
    } else {
        ecs_defer_none(world, stage);
    }
}

void ecs_defer_end(
    ecs_world_t *world)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    
    if (world->in_progress) {
        ecs_stage_defer_end(world, stage);
    } else {
        ecs_defer_flush(world, stage);
    }
}

static
size_t append_to_str(
    char **buffer,
    const char *str,
    size_t bytes_left,
    size_t *required)
{
    char *ptr = *buffer;

    size_t len = strlen(str);
    size_t to_write;
    if (bytes_left < len) {
        to_write = bytes_left;
        bytes_left = 0;
    } else {
        to_write = len;
        bytes_left -= len;
    }
    
    if (to_write) {
        ecs_os_strncpy(ptr, str, to_write);
    }

    (*required) += len;
    (*buffer) += to_write;

    return bytes_left;
}

const char* ecs_role_str(
    ecs_entity_t entity)
{
    if (ECS_HAS_ROLE(entity, CHILDOF)) {
        return "CHILDOF";
    } else
    if (ECS_HAS_ROLE(entity, INSTANCEOF)) {
        return "INSTANCEOF";
    } else
    if (ECS_HAS_ROLE(entity, TRAIT)) {
        return "TRAIT";
    } else
    if (ECS_HAS_ROLE(entity, SWITCH)) {
        return "SWITCH";
    } else
    if (ECS_HAS_ROLE(entity, XOR)) {
        return "XOR";
    } else
    if (ECS_HAS_ROLE(entity, OR)) {
        return "OR";
    } else
    if (ECS_HAS_ROLE(entity, AND)) {
        return "AND";
    } else
    if (ECS_HAS_ROLE(entity, NOT)) {
        return "NOT";
    } else
    if (ECS_HAS_ROLE(entity, SWITCH)) {
        return "SWITCH";
    } else
    if (ECS_HAS_ROLE(entity, CASE)) {
        return "CASE";
    } else
    if (ECS_HAS_ROLE(entity, OWNED)) {
        return "OWNED";
    } else {
        return "UNKNOWN";
    }
}

size_t ecs_entity_str(
    ecs_world_t *world,
    ecs_entity_t entity,
    char *buffer,
    size_t buffer_len)
{
    char *ptr = buffer;
    size_t bytes_left = buffer_len, required = 0;

    if (entity & ECS_ROLE_MASK) {
        const char *role = ecs_role_str(entity);
        bytes_left = append_to_str(&ptr, role, bytes_left, &required);
        bytes_left = append_to_str(&ptr, "|", bytes_left, &required);
    }

    ecs_entity_t e = entity & ECS_COMPONENT_MASK;

    if (ECS_HAS_ROLE(entity, TRAIT)) {
        ecs_entity_t lo = ecs_entity_t_lo(e);
        ecs_entity_t hi = ecs_entity_t_hi(e);

        if (hi) {
            char *hi_path = ecs_get_fullpath(world, hi);
            bytes_left = append_to_str(&ptr, hi_path, bytes_left, &required);
            ecs_os_free(hi_path);

            bytes_left = append_to_str(&ptr, ">", bytes_left, &required);
        }            

        char *lo_path = ecs_get_fullpath(world, lo);
        bytes_left = append_to_str(&ptr, lo_path, bytes_left, &required);
        ecs_os_free(lo_path);
    } else {
        char *path = ecs_get_fullpath(world, e);
        bytes_left = append_to_str(&ptr, path, bytes_left, &required);
        ecs_os_free(path);
    }

    ptr[0] = '\0';

    return required;
}

static
void flush_bulk_new(
    ecs_world_t *world,
    ecs_op_t *op)
{
    ecs_entity_t *ids = op->entities;
    void **bulk_data = op->bulk_data;
    if (bulk_data) {
        ecs_entity_t *components = op->components.array;
        int c, c_count = op->components.count;
        for (c = 0; c < c_count; c ++) {
            ecs_entity_t component = components[c];
            const EcsComponent *cptr = ecs_get(world, component, EcsComponent);
            ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
            size_t size = ecs_to_size_t(cptr->size);
            void *ptr, *data = bulk_data[c];

            int i, count = op->count;
            for (i = 0, ptr = data; i < count; i ++, ptr = ECS_OFFSET(ptr, size)) {
                ecs_set_ptr_w_entity(world, ids[i], component, size, ptr);
            }

            ecs_os_free(data);
        }

        ecs_os_free(bulk_data);
    } else {
        int i, count = op->count;
        for (i = 0; i < count; i ++) {
            add_entities(world, ids[i], &op->components);
        }
    }

    ecs_os_free(ids);
}

/* Leave safe section. Run all deferred commands. */
void ecs_defer_flush(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    if (!--stage->defer) {
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;
        if (defer_queue) {
            ecs_op_t *ops = ecs_vector_first(defer_queue, ecs_op_t);
            int32_t i, count = ecs_vector_count(defer_queue);
            
            for (i = 0; i < count; i ++) {
                ecs_op_t *op = &ops[i];
                if (op->components.count == 1) {
                    op->components.array = &op->component;
                }

                switch(op->kind) {
                case EcsOpNone:
                    break;
                case EcsOpNew:
                    if (op->scope) {
                        ecs_add_entity(
                            world, op->entity, ECS_CHILDOF | op->scope);
                    }
                    /* Fallthrough */
                case EcsOpAdd:
                    add_entities(world, op->entity, &op->components);
                    break;
                case EcsOpRemove:
                    remove_entities(world, op->entity, &op->components);
                    break;
                case EcsOpClone:
                    ecs_clone(world, op->entity, op->component, op->clone_value);
                    break;
                case EcsOpSet:
                    assign_ptr_w_entity(world, op->entity, 
                        op->component, ecs_to_size_t(op->size), 
                        op->value, true, true);
                    break;
                case EcsOpMut:
                    assign_ptr_w_entity(world, op->entity, 
                        op->component, ecs_to_size_t(op->size), 
                        op->value, true, false);
                    break;
                case EcsOpModified:
                    ecs_modified_w_entity(world, op->entity, op->component);
                    break;
                case EcsOpDelete:
                    ecs_delete(world, op->entity);
                    break;
                case EcsOpClear:
                    ecs_clear(world, op->entity);
                    break;
                case EcsOpBulkNew:
                    flush_bulk_new(world, op);
                    break;
                }

                if (op->components.count > 1) {
                    ecs_os_free(op->components.array);
                }

                if (op->value) {
                    ecs_os_free(op->value);
                }
            };

            if (defer_queue != stage->defer_merge_queue) {
                ecs_vector_free(defer_queue);
            }
        }
    }
}
