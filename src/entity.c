
#include "flecs_private.h"

static
int32_t comp_mask_index(
    int32_t value)
{
    return value >> 6;
}

static
void comp_mask_set(
    ecs_comp_mask_t mask,
    int32_t value)
{
    int32_t index = comp_mask_index(value);
    mask[index] |= 1 << (value & 0x3F);
}

static
int32_t comp_mask_is_set(
    ecs_comp_mask_t mask,
    int32_t value)
{
    int32_t index = comp_mask_index(value);
    return mask[index] & 1 << (value & 0x3F);
}

static
int32_t new_w_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    int32_t count,
    void **component_data);

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

    ecs_column_t *column = &columns[index];
    size_t size = column->size; 
    ecs_vector_t *data_vec = column->data;

    ecs_assert(!size || data_vec != NULL, ECS_INTERNAL_ERROR, NULL);

    if (size) {
        void *data = ecs_vector_first_t(data_vec, size, column->alignment);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
        return ECS_OFFSET(data, info->row * size);
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
    ecs_assert(
        ecs_vector_count(type) < ECS_MAX_ENTITIES_IN_TYPE, 
            ECS_TYPE_TOO_LARGE, NULL);

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == component) {
            return get_component_w_index(info, c_ptr_i);
        }
    });
    
    return NULL;
}

/* Get new entity handle */
static
ecs_entity_t new_entity_handle(
    ecs_world_t *world)
{
    ecs_entity_t entity = ++ world->stats.last_handle;
    ecs_assert(!world->stats.max_handle || entity <= world->stats.max_handle, 
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
    ecs_record_t**record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);

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
        EcsTrigger *triggers = ecs_vector_first(trigger_vec, EcsTrigger);
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
void override(
    ecs_world_t *world, 
    ecs_stage_t *stage,
    ecs_table_t *table, 
    ecs_data_t *data, 
    int32_t row, 
    int32_t count,
    ecs_entity_t base,
    ecs_entities_t components,
    bool base_added,
    ecs_comp_mask_t set_mask)
{
    ecs_assert(set_mask != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_info_t base_info;
    if (!get_info(world, base, &base_info) || !base_info.table) {
        return;
    }

    ecs_table_t *base_table = base_info.table;
    ecs_type_t base_type = base_table->type;
    ecs_type_t type = table->type;

    ecs_vector_each(base_type, ecs_entity_t, c_ptr, {
        ecs_entity_t c = *c_ptr;

        if (c & ECS_TYPE_FLAG_MASK) {
            /* If the base has a base, check if it has components to override */
            if (c & ECS_INSTANCEOF) {
                ecs_entity_t base_of_base = c & ECS_ENTITY_MASK;
                override(world, stage, table, data, row, count, base_of_base, 
                    components, base_added, set_mask);
            }
            continue;
        }

        /* If component already has been overridden by a base higher up in the
         * hierarchy, ignore. This mask will also be set by operations that
         * provide a value for a component (like new_w_data). */
        if (comp_mask_is_set(set_mask, c)) {
            continue;
        }

        comp_mask_set(set_mask, c);

        /* This is a quadratic search, but since it only happens when a base is
        * added to an entity and the loop only iterates over the components
        * that have just been added (typically just one) this isn't a big issue */
        int32_t i;
        for (i = 0; i < components.count; i ++) {
            if (components.array[i] == c) {
                /* Component is overridden, find column in entity type */
                int32_t column_index = ecs_type_index_of(type, c);
                ecs_assert(column_index != -1, ECS_INTERNAL_ERROR, NULL);
                ecs_column_t *column = &data->columns[column_index];
                ecs_component_data_t *cdata = ecs_get_component_data(world, c);

                uint32_t data_size = column->size;
                void *data_array = ecs_vector_first_t(column->data, column->size, column->alignment);
                void *data_ptr = ECS_OFFSET(data_array, data_size * row);
                void *base_ptr = get_component_w_index(&base_info, c_ptr_i);
                uint32_t index;

                ecs_copy_t copy = cdata->lifecycle.copy;
                if (copy) {
                    void *ctx = cdata->lifecycle.ctx;
                    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
                    for (index = 0; index < count; index ++) {
                        copy(world, c, &base, &entities[row], 
                            data_ptr, base_ptr, data_size, 1, ctx);
                        data_ptr = ECS_OFFSET(data_ptr, data_size);
                    }
                } else {
                    for (index = 0; index < count; index ++) {
                        memcpy(data_ptr, base_ptr, data_size);
                        data_ptr = ECS_OFFSET(data_ptr, data_size);
                    }                    
                }

                /* Only run OnSet if base was added. OnSet triggers are not
                 * executed on an override because conceptually only the storage
                 * of the component changed, not its value. */
                if (base_added && !(table->flags & EcsTableIsDisabled)) {
                    if (cdata->on_set) {
                        ecs_run_component_trigger(world, stage, cdata->on_set, c, 
                            table, data, row, count);
                    }
                }

                break;
            }
        }

        /* If component wasn't overridden we still need to run OnSet triggers,
         * but now using the base component ptr */
        if (base_added && i == components.count && !(table->flags & EcsTableIsDisabled)) {
            ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
            ecs_component_data_t *cdata = ecs_get_component_data(
                world, c);

            if (cdata->on_set) {
                /* We have to run the triggers for each entity separately, as the
                * base has the component, not the entity (there is no array to
                * iterate over) */
                int32_t i;
                for (i = row; i < row + count; i ++) {
                    run_component_trigger_for_entities(world, stage, cdata->on_set, 
                        c, base_table, base_info.data, base_info.row, 1, 
                        &entities[i]);
                }
            }
        }
    });
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
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_entities_t components = {
        .array = ecs_os_alloca(ecs_entity_t, type_count),
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
        ecs_column_t *column = &child_data->columns[i];
        component_data[pos] = ecs_vector_first_t(column->data, column->size, column->alignment);

        components.array[pos] = c;
        pos ++;
    }

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
            world, stage, table, child_count, component_data);

        /* If prefab child table has children itself, recursively instantiate */
        ecs_data_t *i_data = ecs_table_get_staged_data(world, stage, table);
        ecs_entity_t *children = ecs_vector_first(child_data->entities, ecs_entity_t);

        int j;
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, stage, child, table, i_data, child_row, 1);
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
    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, base);

    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, child_table_ptr, {
            ecs_table_t *child_table = *child_table_ptr;
            if (!ecs_table_count(child_table)) {
                continue;
            }

            instantiate_children(
                world, stage, base, table, data, row, count, child_table);
        });
    }
}

void ecs_run_init_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    uint32_t row,
    uint32_t count,
    ecs_entities_t components,
    ecs_comp_mask_t set_mask)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Array that contains component callbacks & systems */
    ecs_component_data_t* cdata_array = ecs_vector_first(
            world->component_data, ecs_component_data_t);
    ecs_column_t *component_columns = data->columns;
    bool has_base = table->flags & EcsTableHasPrefab;
    ecs_type_t type;
    uint32_t type_count;
    ecs_entity_t *type_array;

    ecs_entities_t base_entities = {
        .array = ecs_os_alloca(ecs_entity_t, components.count),
        .count = 0
    };

    if (components.count) {
        type = table->type;
        type_count = ecs_vector_count(type);
        type_array = ecs_vector_first(type, ecs_entity_t);
    }

    int i, cur;
    for (i = 0; i < components.count; i ++) {
        /* Retrieve component callbacks & triggers for component */
        ecs_entity_t component = components.array[i];

        if (component >= ECS_HI_COMPONENT_ID) {
            if (component & ECS_INSTANCEOF) {
                ecs_entity_t base = component & ECS_ENTITY_MASK;
                instantiate(world, stage, base, table, data, row, count);
                base_entities.array[base_entities.count] = base;
                base_entities.count ++;
            }
            continue;
        }
    
        ecs_component_data_t *cdata = &cdata_array[component];
        void *ctx = cdata->lifecycle.ctx;
        
        ecs_xtor_t ctor = cdata->lifecycle.ctor;
        ecs_vector_t *triggers = cdata->on_add;
        if (!ctor && !triggers && !has_base) {
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
        void *array = ecs_vector_first_t(column->data, size, column->alignment);
        void *ptr = ECS_OFFSET(array, size * row);

        if (ctor) {
            ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_entity_t *ids = ecs_vector_first(data->entities, ecs_entity_t);
            ctor(world, component, ids, ptr, size, count, ctx);
        }

        if (triggers) {
            ecs_run_component_trigger(
                world, stage, triggers, component, table, data, 
                row, count);
        }
    }

    /** Override components after all other actions have completed. This allows
     * us to run OnSet handlers on the appropriate pointers. If a component is
     * overridden by the entity, the OnSet handler should be invoked on the
     * private component of the entity. However, if an entity adds a base
     * without overriding any of its components, the OnSet handler still needs
     * to be invoked on the base component. */
    if (has_base) {
        int32_t i;
        for (i = type_count - 1; i >= 0; i --) {
            ecs_entity_t base = type_array[i];
            if (!(base & ECS_INSTANCEOF)) {
                /* If this component is not a base, no more base entities will
                 * follow */
                break;
            }

            base &= ECS_ENTITY_MASK;

            /* Keep track of which base entities were added. This will make sure
             * that OnSet triggers are invoked for new base components. This is
             * a quadratic search but N is constrained by the number of base
             * entities added, which is usually not more than one. */
            bool base_added = false;
            int32_t j;
            for (j = 0; j < base_entities.count; j ++) {
                if (base == base_entities.array[j]) {
                    base_added = true;
                    break;
                }
            }

            override(world, stage, table, data, row, count, base, 
                components, base_added, set_mask);
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
    ecs_entities_t components,
    bool run_triggers)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Array that contains component callbacks & systems */
    ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data, ecs_component_data_t);
    ecs_column_t *component_columns = data->columns;
    ecs_type_t type;
    uint32_t type_count;
    ecs_entity_t *type_array; 

    if (components.count) {
        type = table->type;
        type_count = ecs_vector_count(type);
        type_array = ecs_vector_first(type, ecs_entity_t);
    }

    int i, cur;
    for (i = 0; i < components.count; i ++) {
        /* Retrieve component callbacks & triggers for component */
        ecs_entity_t component = components.array[i];
        if (component >= ECS_HI_COMPONENT_ID) {
            continue;
        }

        ecs_component_data_t *cdata = &cdata_array[component];
        void *ctx = cdata->lifecycle.ctx;
        ecs_xtor_t dtor = cdata->lifecycle.dtor;
        ecs_vector_t *triggers = cdata->on_remove;
        
        if (!dtor && !triggers) {
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
        void *array = ecs_vector_first_t(column->data, size, column->alignment);
        void *ptr = ECS_OFFSET(array, size * row);

        if (triggers && run_triggers) {
            ecs_run_component_trigger(
                world, stage, triggers, component, table, data, 
                row, count);
        }

        if (dtor) {
            ecs_entity_t *ids = ecs_vector_first(data->entities, ecs_entity_t);
            dtor(world, component, ids, ptr, size, count, ctx);
        }
    }
}

static
void ecs_delete_children(
    ecs_world_t *world,
    ecs_stage_t *stage,
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

    if (dst_table->flags & EcsTableIsPrefab) {
        return;
    }

    if (!src_table || !src_table->monitors) {
        ecs_vector_each(dst_table->monitors, ecs_monitor_t, monitor, {
            ecs_run_monitor(world, stage, monitor, dst_row, count);
        });
    } else {
        /* If both tables have monitors, run the ones that dst_table has and
         * src_table doesn't have */
        int32_t i, count = ecs_vector_count(dst_table->monitors);
        int32_t j = 0, src_count = ecs_vector_count(src_table->monitors);
        ecs_monitor_t *dst_monitors = ecs_vector_first(dst_table->monitors, ecs_monitor_t);
        ecs_monitor_t *src_monitors = ecs_vector_first(src_table->monitors, ecs_monitor_t);

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

    ecs_comp_mask_t set_mask = { 0 };
    ecs_run_init_actions(world, stage, new_table, dst_data, new_row, 1, *added, 
        set_mask);

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

    bool main_stage = stage == &world->stage;
    ecs_record_t *record = info->record;
    ecs_assert(!record || record == ecs_eis_get(&world->stage, entity), ECS_INTERNAL_ERROR, NULL);
    int32_t dst_row = ecs_table_append(world, dst_table, dst_data, entity, record);
    bool same_stage = src_data->stage == stage;

    if (main_stage) {
        record->table = dst_table;
        record->row = ecs_row_to_record(dst_row, info->is_watched);
    } else {
        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = dst_table,
            .row = ecs_row_to_record(dst_row, info->is_watched)
        });
    }

    ecs_assert(ecs_vector_count(src_data->entities) > src_row, ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        ecs_table_move(
            world, entity, entity, dst_table, dst_data, dst_row, src_table, src_data, 
            src_row, !same_stage);

        if (removed) {
            ecs_run_deinit_actions(
                world, stage, src_table, src_data, src_row, 1, *removed, true);
        }            
    }

    /* Only delete from source table if it is in the same stage */
    if (same_stage) {
        ecs_table_delete(world, stage, src_table, src_data, src_row);
    }

    if (added) {
        ecs_comp_mask_t set_mask = { 0 };
        ecs_run_init_actions(
            world, stage, dst_table, dst_data, dst_row, 1, *added, 
            set_mask);
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
            world, stage, src_table, src_data, src_row, 1, *removed, true);
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
        ecs_assert(!world->stats.max_handle || entity <= world->stats.max_handle, ECS_OUT_OF_RANGE, 0);
        ecs_assert(entity >= world->stats.min_handle, ECS_OUT_OF_RANGE, 0);
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
    
    bool changed = false;
    ecs_type_t type = table->type;
    ecs_entity_t e = world->stats.last_handle + 1;
    world->stats.last_handle += count;

    if (!type) {
        return e;
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
    
    ecs_comp_mask_t set_mask = { 0 };

    if (component_data) {
        /* Set components that we're setting in the component mask so the init
         * actions won't call OnSet triggers for them. This ensures we won't
         * call OnSet triggers multiple times for the same component */
        ecs_vector_each(type, ecs_entity_t, c_ptr, {
            ecs_entity_t c = *c_ptr;
            if (c >= ECS_HI_COMPONENT_ID) {
                break;
            }
            
            comp_mask_set(set_mask, c);

            /* Copy component data */
            void *src_ptr = component_data[c_ptr_i];
            if (!src_ptr) {
                continue;
            }

            /* Bulk copy column data into new table */
            ecs_column_t *column = &data->columns[c_ptr_i];
            size_t size = column->size;
            size_t alignment = column->alignment;
            void *ptr = ecs_vector_first_t(column->data, size, alignment);
            ptr = ECS_OFFSET(ptr, size * row);

            ecs_component_data_t *cdata = ecs_get_component_data(world, c);
            ecs_copy_t copy = cdata->lifecycle.copy;
            if (copy) {
                ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
                copy(world, c, entities, entities, ptr, src_ptr, size, count, 
                    cdata->lifecycle.ctx);
            } else {
                memcpy(ptr, src_ptr, size * count);
            }
        });
    }

    int32_t change_count = data->change_count;
    ecs_run_init_actions(world, stage, table, data, row, count, added, set_mask);
    changed = change_count != data->change_count;

    /* Run OnSet triggers */
    if (component_data) {
        int column_count = ecs_vector_count(type);
        for (i = 0; i < column_count; i ++) {
            /* If this is not a component, skip */
            ecs_entity_t component = added.array[i];
            if (component >= ECS_HI_COMPONENT_ID) {
                continue;
            }

            /* Invoke OnSet systems */
            ecs_component_data_t *cdata = ecs_get_component_data(
                world, component);

            if (cdata->on_set) {
                if (changed) {
                    int32_t j;
                    for (j = 0; j < count; j ++) {
                        ecs_entity_info_t info;
                        ecs_get_info(world, stage, e + j, &info);
                        ecs_run_component_trigger(
                            world, stage, cdata->on_set, component, info.table, 
                            info.data, info.row, 1);
                    }
                } else {
                    ecs_run_component_trigger(
                        world, stage, cdata->on_set, component, table, data, 
                        row, count);
                }
            }
        }
    }

    /* Only invoke monitors if entity hasn't changed. If components did change,
     * the monitor will already have been invoked. */
    if (!changed) {
        run_monitors(world, stage, table, data, row, count, NULL);
    }

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

bool ecs_components_contains_component(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t flags,
    ecs_entity_t *entity_out)
{
    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        ecs_entity_t c = *c_ptr;

        if (flags) {
            if ((c & flags) != flags) {
                continue;
            }
        }

        ecs_entity_t e = c & ECS_ENTITY_MASK;
        ecs_type_t type = ecs_get_type(world, e);

        bool result = ecs_type_has_entity(
            world, type, component);
        if (result) {
            if (entity_out) *entity_out = e;
            return true;
        }
    });

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
        ecs_entities_t to_add = ecs_type_to_entities(type);
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
    ecs_entity_t result = world->stats.last_handle + 1;
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
    ecs_entity_t result = world->stats.last_handle + 1;
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

    ecs_delete_children(world, stage, entity);

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
        ecs_eis_set(stage, entity, &(ecs_record_t){ NULL, 0 });
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
    add_remove(world, entity, &components_add, &components_remove, false);
}

void ecs_add_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_entities_t components_add = ecs_type_to_entities(to_add);
    ecs_entities_t components_remove = ecs_type_to_entities(to_remove);
    add_remove(world, entity, &components_add, &components_remove, false);
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
        dst = ++ world->stats.last_handle;
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
            ecs_component_data_t *cdata = ecs_get_component_data(world, component);
            ecs_run_component_trigger(
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
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_record_t *record = ref->record;

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

    if (ref->stage == stage &&
        ref->record == record &&
        ref->table == table &&
        ref->row == record->row &&
        ref->size == ecs_vector_size(data->entities))
    {
        return ref->ptr;
    }

    ref->stage = stage;
    ref->table = table;
    ref->row = record->row;
    ref->size = ecs_vector_size(data->entities);

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
    ecs_run_component_trigger(world, stage, cdata->on_set, component, 
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
    if (!dst) {
        return entity;
    }

    if (ptr) {
        ecs_component_data_t *cdata = ecs_get_component_data(world, component);
        ecs_copy_t copy = cdata->lifecycle.copy;
        if (copy) {
            copy(world, component, &entity, &entity, dst, ptr, size, 1, 
                cdata->lifecycle.ctx);
        } else {
            memcpy(dst, ptr, size);
        }
    } else {
        memset(dst, 0, size);
    }

    ecs_component_data_t *cdata = ecs_get_component_data(world, component);    
    ecs_run_component_trigger(world, stage, cdata->on_set, component, 
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

