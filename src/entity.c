
#include "private_api.h"
#include <ctype.h>

static
void flecs_notify_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_diff_t *diff,
    bool run_on_set);

static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_entity_t *entities,
    ecs_ids_t *component_ids,
    int32_t count,
    void **c_info,
    bool move,
    int32_t *row_out,
    ecs_table_diff_t *diff);

static
void* get_component_w_index(
    ecs_table_t *table,
    int32_t column_index,
    int32_t row)
{
    ecs_check(column_index < ecs_table_storage_count(table), 
        ECS_NOT_A_COMPONENT, NULL);

    ecs_column_t *column = &table->storage.columns[column_index];

    /* If size is 0, component does not have a value. This is likely caused by
    * an application trying to call ecs_get with a tag. */
    int32_t size = column->size;    
    ecs_check(size != 0, ECS_INVALID_PARAMETER, NULL);

    void *ptr = ecs_vector_first_t(column->data, size, column->alignment); 
    return ECS_OFFSET(ptr, size * row);  
error:
    return NULL;
}

static
void* get_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t id)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    if (!table->storage_table) {
        ecs_check(ecs_search(world, table, id, 0) == -1, 
            ECS_NOT_A_COMPONENT, NULL);
        return NULL;
    }

    ecs_table_record_t *tr = flecs_get_table_record(
        world, table->storage_table, id);
    if (!tr) {
        ecs_check(ecs_search(world, table, id, 0) == -1, 
            ECS_NOT_A_COMPONENT, NULL);
       return NULL;
    }

    return get_component_w_index(table, tr->column, row);
error:
    return NULL;
}

static
void* get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_id_record_t *table_index,
    int32_t recur_depth)
{
    /* Cycle detected in IsA relation */
    ecs_check(recur_depth < ECS_MAX_RECURSION, ECS_INVALID_PARAMETER, NULL);

    /* Table (and thus entity) does not have component, look for base */
    if (!(table->flags & EcsTableHasIsA)) {
        return NULL;
    }

    /* Exclude Name */
    if (id == ecs_pair(ecs_id(EcsIdentifier), EcsName)) {
        return NULL;
    }

    /* Table should always be in the table index for (IsA, *), otherwise the
     * HasBase flag should not have been set */
    const ecs_table_record_t *tr_isa = flecs_id_record_table(
        world->idr_isa_wildcard, table);
    ecs_check(tr_isa != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i = tr_isa->column, end = tr_isa->count + tr_isa->column;
    void *ptr = NULL;

    do {
        ecs_id_t pair = ids[i ++];
        ecs_entity_t base = ecs_pair_object(world, pair);

        ecs_record_t *r = ecs_eis_get(world, base);
        if (!r) {
            continue;
        }

        table = r->table;
        if (!table) {
            continue;
        }

        const ecs_table_record_t *tr = NULL;

        ecs_table_t *storage_table = table->storage_table;
        if (storage_table) {
            tr = flecs_id_record_table(table_index, storage_table);
        } else {
            ecs_check(!ecs_owns_id(world, base, id), 
                ECS_NOT_A_COMPONENT, NULL);
        }

        if (!tr) {
            ptr = get_base_component(world, table, id, table_index, 
                recur_depth + 1);
        } else {
            int32_t row = ECS_RECORD_TO_ROW(r->row);
            ptr = get_component_w_index(table, tr->column, row);
        }
    } while (!ptr && (i < end));

    return ptr;
error:
    return NULL;
}

/* Utility to compute actual row from row in record */
static
void set_row_info(
    ecs_entity_info_t *info,
    uint32_t row)
{
    info->row = ECS_RECORD_TO_ROW(row);
    info->row_flags = ECS_RECORD_TO_ROW_FLAGS(row);
}

/* Utility to set info from main stage record */
static
void set_info_from_record(
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

    info->data = &table->storage;

    ecs_assert(ecs_vector_count(table->storage.entities) > info->row, 
        ECS_INTERNAL_ERROR, NULL);
}

static
const ecs_type_info_t *get_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_entity_t real_id = ecs_get_typeid(world, component);
    if (real_id) {
        return flecs_get_c_info(world, real_id);
    } else {
        return NULL;
    }
}

static
void ids_merge(
    ecs_ids_t *ids,
    ecs_ids_t *add)
{
    if (!add || !add->count) {
        return;
    }
    
    int32_t new_count = ids->count + add->count;
    if (new_count >= ids->size) {
        ids->size = flecs_next_pow_of_2(new_count);
        ecs_id_t *arr = ecs_os_malloc(ids->size * ECS_SIZEOF(ecs_id_t));
        ecs_os_memcpy_n(arr, ids->array, ecs_id_t, ids->count);

        if (ids->count >= ECS_MAX_ADD_REMOVE) {
            ecs_os_free(ids->array);
        }
        
        ids->array = arr;
    }

    ecs_os_memcpy_n(&ids->array[ids->count], add->array, ecs_id_t, add->count);
    ids->count += add->count;
}

#define ECS_TABLE_DIFF_INIT {\
    .added = {.array = (ecs_id_t[ECS_MAX_ADD_REMOVE]){0}, .size = ECS_MAX_ADD_REMOVE},\
    .removed = {.array = (ecs_id_t[ECS_MAX_ADD_REMOVE]){0}, .size = ECS_MAX_ADD_REMOVE},\
    .on_set = {.array = (ecs_id_t[ECS_MAX_ADD_REMOVE]){0}, .size = ECS_MAX_ADD_REMOVE},\
    .un_set = {.array = (ecs_id_t[ECS_MAX_ADD_REMOVE]){0}, .size = ECS_MAX_ADD_REMOVE},\
}

static
void diff_append(
    ecs_table_diff_t *dst,
    ecs_table_diff_t *src)
{
    ids_merge(&dst->added, &src->added);
    ids_merge(&dst->removed, &src->removed);
    ids_merge(&dst->on_set, &src->on_set);
    ids_merge(&dst->un_set, &src->un_set);
}

static
void diff_free(
    ecs_table_diff_t *diff)
{
    if (diff->added.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(diff->added.array);
    }
    if (diff->removed.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(diff->removed.array);
    }
    if (diff->on_set.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(diff->on_set.array);
    }
    if (diff->un_set.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(diff->un_set.array);
    }
}

static
ecs_table_t* table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_diff_t *diff)
{
    ecs_table_diff_t temp_diff;
    table = flecs_table_traverse_add(world, table, &id, &temp_diff);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    diff_append(diff, &temp_diff);
    return table;
error:
    return NULL;
}

static
void notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    ecs_entity_t event,
    ecs_ids_t *ids,
    ecs_entity_t relation)
{
    ecs_emit(world, &(ecs_event_desc_t) {
        .event = event,
        .ids = ids,
        .table = table,
        .other_table = other_table,
        .offset = row,
        .count = count,
        .observable = world,
        .relation = relation
    });
}

static
void instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

static
void instantiate_children(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_t *child_table)
{
    if (!ecs_table_count(child_table)) {
        return;
    }

    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = &child_table->storage;

    ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);
    int32_t type_count = ecs_vector_count(type);

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_ids_t components = {
        .array = ecs_os_alloca_n(ecs_entity_t, type_count + 1)
    };

    void **component_data = ecs_os_alloca_n(void*, type_count + 1);

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int j, i, childof_base_index = -1, pos = 0;
    for (i = 0; i < type_count; i ++) {
        ecs_id_t id = ids[i];

        /* Make sure instances don't have EcsPrefab */
        if (id == EcsPrefab) {
            continue;
        }

        /* Keep track of the element that creates the ChildOf relationship with
         * the prefab parent. We need to replace this element to make sure the
         * created children point to the instance and not the prefab */ 
        if (ECS_HAS_RELATION(id, EcsChildOf) && (ECS_PAIR_OBJECT(id) == base)) {
            childof_base_index = pos;
        }

        int32_t storage_index = ecs_table_type_to_storage_index(child_table, i);
        if (storage_index != -1) {
            ecs_column_t *column = &child_data->columns[storage_index];
            component_data[pos] = ecs_vector_first_t(
                column->data, column->size, column->alignment);
        } else {
            component_data[pos] = NULL;
        }

        components.array[pos] = id;
        pos ++;
    }

    /* Table must contain children of base */
    ecs_assert(childof_base_index != -1, ECS_INTERNAL_ERROR, NULL);

    /* If children are added to a prefab, make sure they are prefabs too */
    if (table->flags & EcsTableIsPrefab) {
        components.array[pos] = EcsPrefab;
        component_data[pos] = NULL;
        pos ++;
    }

    components.count = pos;

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t child_count = ecs_vector_count(child_data->entities);

    for (i = row; i < count + row; i ++) {
        ecs_entity_t instance = entities[i];
        ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
        ecs_table_t *i_table = NULL;
 
        /* Replace ChildOf element in the component array with instance id */
        components.array[childof_base_index] = ecs_pair(EcsChildOf, instance);

        /* Find or create table */
        for (j = 0; j < components.count; j ++) {
            i_table = table_append(world, i_table, components.array[j], &diff);
        }

        ecs_assert(i_table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vector_count(i_table->type) == components.count,
            ECS_INTERNAL_ERROR, NULL);

        /* The instance is trying to instantiate from a base that is also
         * its parent. This would cause the hierarchy to instantiate itself
         * which would cause infinite recursion. */
        ecs_entity_t *children = ecs_vector_first(
            child_data->entities, ecs_entity_t);

#ifdef FLECS_SANITIZE
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];        
            ecs_check(child != instance, ECS_INVALID_PARAMETER, NULL);
        }
#else
        /* Bit of boilerplate to ensure that we don't get warnings about the
         * error label not being used. */
        ecs_check(true, ECS_INVALID_OPERATION, NULL);
#endif

        /* Create children */
        int32_t child_row; 
        new_w_data(world, i_table, NULL, &components, child_count, 
            component_data, false, &child_row, &diff);
        diff_free(&diff);

        /* If prefab child table has children itself, recursively instantiate */
        ecs_data_t *i_data = &i_table->storage;
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, child, i_table, i_data, child_row + j, 1);
        }
    }   
error:
    return;    
}

static
void instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    ecs_table_t *base_table = ecs_get_table(world, ecs_get_alive(world, base));
    if (!base_table || !(base_table->flags & EcsTableIsPrefab)) {
        /* Don't instantiate children from base entities that aren't prefabs */
        return;
    }

    ecs_table_iter_t it;
    if (flecs_table_iter(world, ecs_pair(EcsChildOf, base), &it) != NULL) {
        for (; it.cur < it.end; ++ it.cur) {
            instantiate_children(
                world, base, table, data, row, count, it.cur->table);
        }
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_table_t *other_table,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool notify_on_set);

static
bool override_from_base(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_table_t *other_table,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool notify_on_set)
{
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_info_t base_info;
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    if (!flecs_get_info(world, base, &base_info) || !base_info.table) {
        return false;
    }

    void *base_ptr = get_component(
        world, base_info.table, base_info.row, component);
    if (base_ptr) {
        int16_t data_size = column->size;
        void *data_array = ecs_vector_first_t(
            column->data, column->size, column->alignment);
        void *data_ptr = ECS_OFFSET(data_array, data_size * row);

        component = ecs_get_typeid(world, component);
        const ecs_type_info_t *cdata = flecs_get_c_info(world, component);
        int32_t index;

        ecs_copy_t copy = cdata ? cdata->lifecycle.copy : NULL;
        if (copy) {
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);

            void *ctx = cdata->lifecycle.ctx;
            for (index = 0; index < count; index ++) {
                copy(world, component, &entities[row], &base,
                    data_ptr, base_ptr, flecs_itosize(data_size), 1, ctx);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }
        } else {
            for (index = 0; index < count; index ++) {
                ecs_os_memcpy(data_ptr, base_ptr, data_size);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }                    
        }

        ecs_ids_t ids = {
            .array = (ecs_id_t[]){ component },
            .count = 1
        };

        if (notify_on_set) {
            /* Check if the component was available for the previous table. If
             * the override is caused by an add operation, it does not introduce
             * a new component value, and the application should not be 
             * notified. 
             * 
             * If the override is the result if adding a IsA relation
             * with an entity that has components with the OVERRIDE flag, an
             * event should be generated, since this represents a new component
             * (and component value) for the entity.
             * 
             * Note that this is an edge case, regular (self) triggers won't be 
             * notified because the event id is not the component but an IsA
             * relationship. Superset triggers will not be invoked because the
             * component is owned. */
            int32_t c = ecs_search_relation(world, other_table, 0, component, 
                EcsIsA, 1, 0, 0, 0, 0);
            if (c == -1) {
                notify(
                    world, table, other_table, row, count, EcsOnSet, &ids, 0);
            }
        }

        return true;
    } else {
        /* If component not found on base, check if base itself inherits */
        ecs_type_t base_type = base_info.table->type;
        return override_component(world, component, base_type, table, 
            other_table, data, column, row, count, notify_on_set);
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_table_t *other_table,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool notify_on_set)
{
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t i, type_count = ecs_vector_count(type);

    /* Walk prefabs */
    i = type_count - 1;
    do {
        ecs_entity_t e = type_array[i];

        if (!(e & ECS_ROLE_MASK)) {
            break;
        }

        if (ECS_HAS_RELATION(e, EcsIsA)) {
            if (override_from_base(world, ecs_pair_object(world, e), component,
                table, other_table, data, column, row, count, notify_on_set))
            {
                return true;
            }
        }
    } while (--i >= 0);

    return false;
}

static
void components_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *added,
    bool notify_on_set)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_type_t type = table->type;
    ecs_table_t *storage_table = table->storage_table;

    int i;
    for (i = 0; i < added->count; i ++) {
        ecs_entity_t id = added->array[i];

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            ecs_entity_t base = ECS_PAIR_OBJECT(id);

            /* Cannot inherit from base if base is final */
            ecs_check(!ecs_has_id(world, ecs_get_alive(world, base), EcsFinal),
                ECS_CONSTRAINT_VIOLATED, NULL);
            ecs_check(base != 0, ECS_INVALID_PARAMETER, NULL);

            if (!world->stage.base) {
                /* Setting base prevents instantiating the hierarchy multiple
                 * times. The instantiate function recursively iterates the
                 * hierarchy to instantiate children. While this is happening,
                 * new tables are created which end up calling this function,
                 * which would call instantiate multiple times for the same
                 * level in the hierarchy. */
                world->stage.base = base;
                instantiate(world, base, table, data, row, count);
                world->stage.base = 0;
            }
        }

        if (!storage_table) {
            continue;
        }

        ecs_table_record_t *tr = flecs_get_table_record(
            world, storage_table, id);
        if (!tr) {
            continue;
        }

        ecs_column_t *column = &columns[tr->column];
        if (!column->size) {
            continue;
        }

        override_component(world, id, type, table, other_table, data, column, 
            row, count, notify_on_set);
    }
error:
    return;
}

static
void set_switch(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,    
    ecs_ids_t *entities,
    bool reset)
{
    ecs_entity_t *array = entities->array;
    int32_t i, comp_count = entities->count;

    for (i = 0; i < comp_count; i ++) {
        ecs_entity_t e = array[i];

        if (ECS_HAS_ROLE(e, CASE)) {
            e = e & ECS_COMPONENT_MASK;

            ecs_entity_t sw_case = 0;
            if (!reset) {
                sw_case = e;
                ecs_assert(sw_case != 0, ECS_INTERNAL_ERROR, NULL);
            }

            int32_t sw_index = flecs_table_switch_from_case(world, table, e);
            ecs_assert(sw_index != -1, ECS_INTERNAL_ERROR, NULL);
            ecs_switch_t *sw = data->sw_columns[sw_index].data;
            ecs_assert(sw != NULL, ECS_INTERNAL_ERROR, NULL);
            
            int32_t r;
            for (r = 0; r < count; r ++) {
                flecs_switch_set(sw, row + r, sw_case);
            }
        }
    }
}

static
void ecs_components_switch(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *added,
    ecs_ids_t *removed)
{
    if (added) {
        set_switch(world, table, data, row, count, added, false);
    }
    if (removed) {
        set_switch(world, table, data, row, count, removed, true);
    } 
}

static
int32_t new_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *new_table,
    ecs_table_diff_t *diff,
    bool construct,
    bool notify_on_set)
{
    ecs_record_t *record = info->record;
    ecs_data_t *new_data = &new_table->storage;
    int32_t new_row;

    if (!record) {
        record = ecs_eis_ensure(world, entity);
    }

    new_row = flecs_table_append(
        world, new_table, new_data, entity, record, construct);

    record->table = new_table;
    record->row = ECS_ROW_TO_RECORD(new_row, info->row_flags);

    ecs_assert(ecs_vector_count(new_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);

    if (new_table->flags & EcsTableHasAddActions) {
        flecs_notify_on_add(
            world, new_table, NULL, new_data, new_row, 1, diff, notify_on_set);       
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
    ecs_table_diff_t *diff,
    bool construct,
    bool notify_on_set)
{    
    ecs_data_t *dst_data = &dst_table->storage;
    ecs_assert(src_data != dst_data, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_record_t *record = info->record;
    ecs_assert(!record || record == ecs_eis_get(world, entity), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t dst_row = flecs_table_append(world, dst_table, dst_data, entity, 
        record, false);

    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        flecs_notify_on_remove(
            world, src_table, dst_table, src_row, 1, diff);

        flecs_table_move(world, entity, entity, dst_table, dst_data, dst_row, 
            src_table, src_data, src_row, construct);                
    }

    /* Update entity index & delete old data after running remove actions */
    record->table = dst_table;
    record->row = ECS_ROW_TO_RECORD(dst_row, info->row_flags);
    
    flecs_table_delete(world, src_table, src_data, src_row, false);

    /* If components were added, invoke add actions */
    if (src_table != dst_table || diff->added.count) {
        if (diff->added.count && (dst_table->flags & EcsTableHasAddActions)) {
            flecs_notify_on_add(world, dst_table, src_table, dst_data, 
                dst_row, 1, diff, notify_on_set);
        }
    }

    info->data = dst_data;
    return dst_row;
error:
    return -1;
}

static
void delete_entity(
    ecs_world_t *world,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_table_diff_t *diff)
{
    if (src_table) {
        /* Invoke remove actions before deleting */
        if (src_table->flags & EcsTableHasRemoveActions) {   
            flecs_notify_on_remove(world, src_table, NULL, src_row, 1, diff);
        } 
    }

    flecs_table_delete(world, src_table, src_data, src_row, true);
}

/* Updating component monitors is a relatively expensive operation that only
 * happens for entities that are monitored. The approach balances the amount of
 * processing between the operation on the entity vs the amount of work that
 * needs to be done to rematch queries, as a simple brute force approach does
 * not scale when there are many tables / queries. Therefore we need to do a bit
 * of bookkeeping that is more intelligent than simply flipping a flag */
static
void update_component_monitor_w_array(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t relation,
    ecs_ids_t *entities)
{
    if (!entities) {
        return;
    }

    int i;
    for (i = 0; i < entities->count; i ++) {
        ecs_entity_t id = entities->array[i];
        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_entity_t rel = ECS_PAIR_RELATION(id);
            
            /* If a relationship has changed, check if it could have impacted
             * the shape of the graph for that relationship. If so, mark the
             * relationship as dirty */        
            if (rel != relation && flecs_get_id_record(
                world, ecs_pair(rel, entity))) 
            {
                update_component_monitor_w_array(world, entity, rel, entities);
            }
        }
        
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            /* If an IsA relationship is added to a monitored entity (can
             * be either a parent or a base) component monitors need to be
             * evaluated for the components of the prefab. */
            ecs_entity_t base = ecs_pair_object(world, id);
            ecs_type_t type = ecs_get_type(world, base);
            ecs_ids_t base_entities = flecs_type_to_ids(type);

            /* This evaluates the component monitor for all components of the
             * base entity. If the base entity contains IsA relationships
             * these will be evaluated recursively as well. */
            update_component_monitor_w_array(
                world, entity, relation, &base_entities);               
        } else {
            flecs_monitor_mark_dirty(world, relation, id);
        }
    }
}

static
void update_component_monitors(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_ids_t *added,
    ecs_ids_t *removed)
{
    update_component_monitor_w_array(world, entity, 0, added);
    update_component_monitor_w_array(world, entity, 0, removed);
}

static
void commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *dst_table,   
    ecs_table_diff_t *diff,
    bool construct,
    bool notify_on_set)
{
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *src_table = info->table;
    if (src_table == dst_table) {
        /* If source and destination table are the same no action is needed *
         * However, if a component was added in the process of traversing a
         * table, this suggests that a case switch could have occured. */
        if (((diff->added.count) || (diff->removed.count)) && 
             src_table && src_table->flags & EcsTableHasSwitch) 
        {
            ecs_components_switch(
                world, src_table, info->data, info->row, 1, 
                    &diff->added, &diff->removed);
        }

        return;
    }

    if (src_table) {
        ecs_data_t *src_data = info->data;
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (dst_table->type) { 
            info->row = move_entity(world, entity, info, src_table, 
                src_data, info->row, dst_table, diff, construct, notify_on_set);
            info->table = dst_table;
        } else {
            delete_entity(world, src_table, src_data, info->row, diff);

            ecs_eis_set(world, entity, &(ecs_record_t){
                NULL, info->row_flags
            });
        }      
    } else {        
        if (dst_table->type) {
            info->row = new_entity(
                world, entity, info, dst_table, diff, construct, notify_on_set);
            info->table = dst_table;
        }        
    }

    /* If the entity is being watched, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (info->row_flags) {
        update_component_monitors(world, entity, &diff->added, &diff->removed);
    }

    if ((!src_table || !src_table->type) && world->range_check_enabled) {
        ecs_check(!world->stats.max_id || entity <= world->stats.max_id, 
            ECS_OUT_OF_RANGE, 0);
        ecs_check(entity >= world->stats.min_id, 
            ECS_OUT_OF_RANGE, 0);
    } 
error:
    return;
}

static
void new(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_ids_t *to_add)
{
    ecs_entity_info_t info = {0};
    int32_t i, count = to_add->count;
    ecs_table_t *table = &world->store.root;
    
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    for (i = 0; i < count; i ++) {
        table = table_append(world, table, to_add->array[i], &diff);
    }

    new_entity(world, entity, &info, table, &diff, true, true);

    diff_free(&diff);
}

static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_entity_t *entities,
    ecs_ids_t *component_ids,
    int32_t count,
    void **component_data,
    bool is_move,
    int32_t *row_out,
    ecs_table_diff_t *diff)
{
    int32_t sparse_count = 0;
    if (!entities) {
        sparse_count = ecs_eis_count(world);
        entities = flecs_sparse_new_ids(world->store.entity_index, count);
    }

    if (!table) {
        return entities;
    }

    ecs_type_t type = table->type;   
    if (!type) {
        return entities;        
    }

    ecs_ids_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = ecs_vector_first(type, ecs_entity_t);
        component_array.count = ecs_vector_count(type);
    }

    ecs_data_t *data = &table->storage;
    int32_t row = flecs_table_appendn(world, table, data, count, entities);
    
    /* Update entity index. */
    int i;
    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    for (i = 0; i < count; i ++) { 
        record_ptrs[row + i] = ecs_eis_set(world, entities[i], 
            &(ecs_record_t){
                .table = table,
                .row = ECS_ROW_TO_RECORD(row + i, 0)
            });
    }

    flecs_defer_none(world, &world->stage);

    flecs_notify_on_add(world, table, NULL, data, row, count, diff, 
        component_data == NULL);

    if (component_data) {
        /* Set components that we're setting in the component mask so the init
         * actions won't call OnSet triggers for them. This ensures we won't
         * call OnSet triggers multiple times for the same component */
        int32_t c_i;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            /* Find component in storage type */
            ecs_entity_t id = component_ids->array[c_i];
            ecs_column_t *column = ecs_table_column_for_id(world, table, id);
            ecs_assert(column != NULL, ECS_INTERNAL_ERROR, NULL);

            int16_t size = column->size;
            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

            int16_t alignment = column->alignment;
            void *ptr = ecs_vector_first_t(column->data, size, alignment);
            ptr = ECS_OFFSET(ptr, size * row);

            const ecs_type_info_t *cdata = get_c_info(world, id);
            ecs_copy_t copy;
            ecs_move_t move;
            if (cdata && is_move && (move = cdata->lifecycle.move)) {
                ecs_entity_t *eids = ecs_vector_first(data->entities, ecs_entity_t);
                move(world, id, eids, eids, ptr, src_ptr, 
                    flecs_itosize(size), count, cdata->lifecycle.ctx);
            } else if (cdata && !is_move && (copy = cdata->lifecycle.copy)) {
                ecs_entity_t *eids = ecs_vector_first(data->entities, ecs_entity_t);
                copy(world, id, eids, eids, ptr, src_ptr, 
                    flecs_itosize(size), count, cdata->lifecycle.ctx);
            } else {
                ecs_os_memcpy(ptr, src_ptr, size * count);
            } 
        };

        flecs_notify_on_set(world, table, row, count, NULL, true);
        flecs_notify_on_set(world, table, row, count, &diff->on_set, false);
    }

    flecs_defer_flush(world, &world->stage);

    if (row_out) {
        *row_out = row;
    }

    if (sparse_count) {
        entities = flecs_sparse_ids(world->store.entity_index);
        return &entities[sparse_count];
    } else {
        return entities;
    }
}

static
void add_id_w_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_id_t id,
    bool construct)
{
    ecs_table_diff_t diff;

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &id, &diff);

    commit(world, entity, info, dst_table, &diff, construct, 
        false); /* notify_on_set = false, this function is only called from
                 * functions that are about to set the component. */
}

static
void add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_add(world, stage, entity, id)) {
        return;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_table_diff_t diff;
    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &id, &diff);

    commit(world, entity, &info, dst_table, &diff, true, true);

    flecs_defer_flush(world, stage);
}

static
void remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_remove(world, stage, entity, id)) {
        return;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_table_diff_t diff;
    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, &id, &diff);

    commit(world, entity, &info, dst_table, &diff, true, true);

    flecs_defer_flush(world, stage);
}

static
void *get_mutable(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_entity_info_t *info,
    bool *is_added)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check((component & ECS_COMPONENT_MASK) == component || 
        ECS_HAS_ROLE(component, PAIR), ECS_INVALID_PARAMETER, NULL);

    void *dst = NULL;
    if (flecs_get_info(world, entity, info) && info->table) {
        dst = get_component(world, info->table, info->row, component);
    }

    if (!dst) {
        ecs_table_t *table = info->table;
        add_id_w_info(world, entity, info, component, true);
        flecs_get_info(world, entity, info);
        
        ecs_assert(info->table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(info->table->storage_table != NULL, ECS_INTERNAL_ERROR, NULL);
        dst = get_component(world, info->table, info->row, component);

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
error:
    return NULL;
}


/* -- Private functions -- */
static
void flecs_notify_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_diff_t *diff,
    bool run_on_set)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);

    if (diff->added.count) {
        if (table->flags & EcsTableHasIsA) {
            components_override(world, table, other_table, data, row, count, 
                &diff->added, run_on_set);
        }

        if (table->flags & EcsTableHasSwitch) {
            ecs_components_switch(
                world, table, data, row, count, &diff->added, NULL);
        }

        if (table->flags & EcsTableHasOnAdd) {
            notify(world, table, other_table, row, count, EcsOnAdd, 
                &diff->added, 0);
        }
    }

    /* When a IsA relation is added to an entity, that entity inherits the
     * components from the base. Send OnSet notifications so that an application
     * can respond to these new components. */
    if (run_on_set && diff->on_set.count) {
        notify(world, table, other_table, row, count, EcsOnSet, &diff->on_set, 
            EcsIsA);
    }
}

void flecs_notify_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    ecs_table_diff_t *diff)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);

    if (count) {
        if (diff->un_set.count) {
            notify(world, table, other_table, row, count, EcsUnSet, &diff->un_set, 0);
        }

        if (table->flags & EcsTableHasOnRemove && diff->removed.count) {
            notify(world, table, other_table, row, count, EcsOnRemove, 
                &diff->removed, 0);
        }

        if (table->flags & EcsTableHasIsA && diff->on_set.count) {
            notify(world, table, other_table, row, count, EcsOnSet, 
                &diff->on_set, 0);
        }
    }
}

void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_ids_t *ids,
    bool owned)
{
    ecs_data_t *data = &table->storage;

    ecs_entity_t *entities = ecs_vector_get(data->entities, ecs_entity_t, row);
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), 
        ECS_INTERNAL_ERROR, NULL);
    entities = ecs_offset(entities, ecs_entity_t, row);

    ecs_ids_t local_ids;
    if (!ids) {
        local_ids.array = ecs_vector_first(table->storage_type, ecs_id_t);
        local_ids.count = ecs_vector_count(table->storage_type);
        ids = &local_ids;
    }

    if (owned) {
        int i;
        for (i = 0; i < ids->count; i ++) {
            ecs_id_t id = ids->array[i];
            const ecs_type_info_t *info = get_c_info(world, id);
            ecs_iter_action_t on_set;
            if (info && (on_set = info->lifecycle.on_set)) {
                ecs_column_t *c = ecs_table_column_for_id(world, table, id);
                ecs_size_t size = c->size;
                void *ptr = ecs_vector_get_t(c->data, size, c->alignment, row);
                ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

                ecs_iter_t it = {.term_count = 1};
                it.entities = entities;
                
                flecs_iter_init(&it);
                it.world = world;
                it.real_world = world;
                it.ptrs[0] = ptr;
                it.sizes[0] = size;
                it.ids[0] = id;
                it.ctx = info->lifecycle.ctx;
                it.count = count;
                
                on_set(&it);
            }
        }
    }

    /* Run OnSet notifications */
    if (table->flags & EcsTableHasOnSet && ids->count) {
        notify(world, table, NULL, row, count, EcsOnSet, ids, 0);
    }
}

bool flecs_get_info(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    info->table = NULL;
    info->record = NULL;
    info->data = NULL;
    info->row_flags = 0;

    if (entity & ECS_ROLE) {
        return false;
    }
    
    ecs_record_t *record = ecs_eis_get(world, entity);

    if (!record) {
        return false;
    }

    set_info_from_record(info, record);

    return true;
}

uint32_t flecs_record_to_row(
    uint32_t row, 
    bool *is_watched_out) 
{
    *is_watched_out = (row & ECS_ROW_FLAGS_MASK) != 0;
    return row & ECS_ROW_MASK;
}

uint32_t flecs_row_to_record(
    uint32_t row, 
    bool is_watched) 
{
    return row | (ECS_FLAG_OBSERVED * is_watched);
}

ecs_ids_t flecs_type_to_ids(
    ecs_type_t type)
{
    return (ecs_ids_t){
        .array = ecs_vector_first(type, ecs_entity_t),
        .count = ecs_vector_count(type)
    };
}

void flecs_add_flag(
    ecs_world_t *world,
    ecs_entity_t entity,
    uint32_t flag)
{    
    (void)world;

    ecs_record_t *record = ecs_eis_get(world, entity);
    if (!record) {
        ecs_record_t new_record = {.row = flag, .table = NULL};
        ecs_eis_set(world, entity, &new_record);
    } else {
        record->row |= flag;
    }
}


/* -- Public functions -- */

bool ecs_commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *table,
    ecs_ids_t *added,
    ecs_ids_t *removed)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *src_table = NULL;
    if (!record) {
        record = ecs_eis_get(world, entity);
        src_table = record->table;
    }

    ecs_entity_info_t info = {0};
    if (record) {
        set_info_from_record(&info, record);
    }

    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    if (added) {
        diff.added = *added;
    }
    if (removed) {
        diff.added = *removed;
    }
    
    commit(world, entity, &info, table, &diff, true, true);

    return src_table != table;
error:
    return false;
}

ecs_entity_t ecs_set_with(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_id_t prev = stage->with;
    stage->with = id;
    return prev;
error:
    return 0;
}

ecs_id_t ecs_get_with(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->with;
error:
    return 0;
}

ecs_entity_t ecs_new_id(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);

    /* It is possible that the world passed to this function is a stage, so
     * make sure we have the actual world. Cast away const since this is one of
     * the few functions that may modify the world while it is in readonly mode,
     * since it is thread safe (uses atomic inc when in threading mode) */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);

    ecs_entity_t entity;
    int32_t stage_count = ecs_get_stage_count(unsafe_world);
    if (stage->asynchronous || (ecs_os_has_threading() && stage_count > 1)) {
        /* Can't atomically increase number above max int */
        ecs_assert(unsafe_world->stats.last_id < UINT_MAX, 
            ECS_INVALID_OPERATION, NULL);
        entity = (ecs_entity_t)ecs_os_ainc(
            (int32_t*)&unsafe_world->stats.last_id);
    } else {
        entity = ecs_eis_recycle(unsafe_world);
    }

    ecs_assert(!unsafe_world->stats.max_id || 
        ecs_entity_t_lo(entity) <= unsafe_world->stats.max_id, 
        ECS_OUT_OF_RANGE, NULL);

    return entity;
error:
    return 0;
}

ecs_entity_t ecs_new_low_id(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    /* It is possible that the world passed to this function is a stage, so
     * make sure we have the actual world. Cast away const since this is one of
     * the few functions that may modify the world while it is in readonly mode,
     * but only if single threaded. */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);
    if (unsafe_world->is_readonly) {
        /* Can't issue new comp id while iterating when in multithreaded mode */
        ecs_check(ecs_get_stage_count(world) <= 1,
            ECS_INVALID_WHILE_ITERATING, NULL);
    }

    ecs_entity_t id = 0;
    if (unsafe_world->stats.last_component_id < ECS_HI_COMPONENT_ID) {
        do {
            id = unsafe_world->stats.last_component_id ++;
        } while (ecs_exists(unsafe_world, id) && id <= ECS_HI_COMPONENT_ID);        
    }

    if (!id || id >= ECS_HI_COMPONENT_ID) {
        /* If the low component ids are depleted, return a regular entity id */
        id = ecs_new_id(unsafe_world);
    }

    return id;
error: 
    return 0;
}

ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    ecs_id_t ids[3];
    ecs_ids_t to_add = { .array = ids, .count = 0 };

    if (id) {
        ids[to_add.count ++] = id;
    }

    ecs_id_t with = stage->with;
    if (with) {
        ids[to_add.count ++] = with;
    }

    ecs_entity_t scope = stage->scope;
    if (scope) {
        if (!id || !ECS_HAS_RELATION(id, EcsChildOf)) {
            ids[to_add.count ++] = ecs_pair(EcsChildOf, scope);
        }
    }
    if (to_add.count) {
        if (flecs_defer_new(world, stage, entity, to_add.array[0])) {
            int i;
            for (i = 1; i < to_add.count; i ++) {
                flecs_defer_add(world, stage, entity, to_add.array[i]);
            }
            return entity;
        }

        new(world, entity, &to_add);
    } else {
        if (flecs_defer_new(world, stage, entity, 0)) {
            return entity;
        }

        ecs_eis_set(world, entity, &(ecs_record_t){ 0 });
    }
    flecs_defer_flush(world, stage);

    return entity;
error:
    return 0;
}

#ifdef FLECS_PARSER

/* Traverse table graph by either adding or removing identifiers parsed from the
 * passed in expression. */
static
ecs_table_t *traverse_from_expr(
    ecs_world_t *world,
    ecs_table_t *table,
    const char *name,
    const char *expr,
    ecs_table_diff_t *diff,
    bool replace_and,
    bool *error)
{
    const char *ptr = expr;
    if (ptr) {
        ecs_term_t term = {0};
        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))){
            if (!ecs_term_is_initialized(&term)) {
                break;
            }

            if (term.pred.set.mask == EcsDefaultSet) {
                term.pred.set.mask = EcsSelf;
            }
            if (term.subj.set.mask == EcsDefaultSet) {
                term.subj.set.mask = EcsSelf;
            }
            if (term.obj.set.mask == EcsDefaultSet) {
                term.obj.set.mask = EcsSelf;
            }

            if (ecs_term_finalize(world, name, &term)) {
                ecs_term_fini(&term);
                if (error) {
                    *error = true;
                }
                return NULL;
            }

            if (!ecs_term_is_trivial(&term)) {
                ecs_term_fini(&term);
                if (error) {
                    *error = true;
                }
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid non-trivial term in add expression");
                return NULL;
            }

            if (term.role == ECS_CASE) {
                table = table_append(world, table, 
                    ECS_SWITCH | ECS_PAIR_RELATION(term.id), diff);
                term.id = ECS_CASE | ECS_PAIR_OBJECT(term.id);
            }

            if (term.oper == EcsAnd || !replace_and) {
                /* Regular AND expression */
                table = table_append(world, table, term.id, diff);
            } else if (term.oper == EcsAndFrom) {
                /* Add all components from the specified type */
                const EcsType *t = ecs_get(world, term.id, EcsType);
                if (!t) {
                    if (error) {
                        *error = true;
                    }
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "expected type for AND role");
                    return NULL;
                }
                
                ecs_id_t *ids = ecs_vector_first(t->normalized->type, ecs_id_t);
                int32_t i, count = ecs_vector_count(t->normalized->type);
                for (i = 0; i < count; i ++) {
                    table = table_append(world, table, ids[i], diff);
                }
            }

            ecs_term_fini(&term);
        }

        if (!ptr) {
            if (error) {
                *error = true;
            }
            return NULL;
        }
    }

    return table;
}

/* Add/remove components based on the parsed expression. This operation is 
 * slower than traverse_from_expr, but safe to use from a deferred context. */
static
void defer_from_expr(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const char *expr,
    bool is_add,
    bool replace_and)
{
    const char *ptr = expr;
    if (ptr) {
        ecs_term_t term = {0};
        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))){
            if (!ecs_term_is_initialized(&term)) {
                break;
            }

            if (ecs_term_finalize(world, name, &term)) {
                return;
            }

            if (!ecs_term_is_trivial(&term)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid non-trivial term in add expression");
                return;
            }

            if (term.oper == EcsAnd || !replace_and) {
                /* Regular AND expression */
                if (is_add) {
                    ecs_add_id(world, entity, term.id);
                } else {
                    ecs_remove_id(world, entity, term.id);
                }
            } else if (term.oper == EcsAndFrom) {
                /* Add all components from the specified type */
                const EcsType *t = ecs_get(world, term.id, EcsType);
                if (!t) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "expected type for AND role");
                    return;
                }
                
                ecs_id_t *ids = ecs_vector_first(t->normalized->type, ecs_id_t);
                int32_t i, count = ecs_vector_count(t->normalized->type);
                for (i = 0; i < count; i ++) {
                    if (is_add) {
                        ecs_add_id(world, entity, ids[i]);
                    } else {
                        ecs_remove_id(world, entity, ids[i]);
                    }
                }
            }

            ecs_term_fini(&term);
        }
    }
}
#endif

/* If operation is not deferred, add components by finding the target
 * table and moving the entity towards it. */
static 
int traverse_add(
    ecs_world_t *world,
    ecs_entity_t result,
    const char *name,
    const ecs_entity_desc_t *desc,
    ecs_entity_t scope,
    ecs_id_t with,
    bool new_entity,
    bool name_assigned)
{
    const char *sep = desc->sep;

    /* Find existing table */
    ecs_entity_info_t info = {0};
    ecs_table_t *src_table = NULL, *table = NULL;
    if (!new_entity) {
        if (flecs_get_info(world, result, &info)) {
            table = info.table;
        }
    }

    /* Find destination table */
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;

    /* If this is a new entity without a name, add the scope. If a name is
     * provided, the scope will be added by the add_path_w_sep function */
    if (new_entity) {
        if (new_entity && scope && !name && !name_assigned) {
            table = table_append(
                world, table, ecs_pair(EcsChildOf, scope), &diff);
        }
        if (with) {
            table = table_append(world, table, with, &diff);
        }
    }

    /* If a name is provided but not yet assigned, add the Name component */
    if (name && !name_assigned) {
        table = table_append(world, table, 
            ecs_pair(ecs_id(EcsIdentifier), EcsName), &diff);
    }

    /* Add components from the 'add' id array */
    int32_t i = 0;
    ecs_id_t id;
    const ecs_id_t *ids = desc->add;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        table = table_append(world, table, id, &diff);
        if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_RELATION(id) == EcsChildOf) {
            scope = ECS_PAIR_OBJECT(id);
        }
    }

    /* Add components from the 'add_expr' expression */
    if (desc->add_expr) {
#ifdef FLECS_PARSER
        bool error = false;
        table = traverse_from_expr(
            world, table, name, desc->add_expr, &diff, true, &error);
        if (error) {
            return -1;
        }
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Commit entity to destination table */
    if (src_table != table) {
        ecs_defer_begin(world);
        commit(world, result, &info, table, &diff, true, true);
        ecs_defer_end(world);
    }

    /* Set name */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, result, scope, name, sep, NULL);
        ecs_assert(ecs_get_name(world, result) != NULL,
            ECS_INTERNAL_ERROR, NULL);
    }

    if (desc->symbol) {
        const char *sym = ecs_get_symbol(world, result);
        if (sym) {
            ecs_assert(!ecs_os_strcmp(desc->symbol, sym),
                ECS_INCONSISTENT_NAME, desc->symbol);
        } else {
            ecs_set_symbol(world, result, desc->symbol);
        }
    }

    diff_free(&diff);

    return 0;
}

/* When in deferred mode, we need to add/remove components one by one using
 * the regular operations. */
static 
void deferred_add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const ecs_entity_desc_t *desc,
    ecs_entity_t scope,
    ecs_id_t with,
    bool new_entity,
    bool name_assigned)
{
    const char *sep = desc->sep;

    /* If this is a new entity without a name, add the scope. If a name is
     * provided, the scope will be added by the add_path_w_sep function */
    if (new_entity) {
        if (new_entity && scope && !name && !name_assigned) {
            ecs_add_id(world, entity, ecs_pair(EcsChildOf, scope));
        }

        if (with) {
            ecs_add_id(world, entity, with);
        }
    }

    /* Add components from the 'add' id array */
    int32_t i = 0;
    ecs_id_t id;
    const ecs_id_t *ids = desc->add;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        ecs_add_id(world, entity, id);
        if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_RELATION(id) == EcsChildOf) {
            scope = ECS_PAIR_OBJECT(id);
        }
    }

    /* Add components from the 'add_expr' expression */
    if (desc->add_expr) {
#ifdef FLECS_PARSER
        defer_from_expr(world, entity, name, desc->add_expr, true, true);
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Set name */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, entity, scope, name, sep, NULL);
    }

    /* Currently it's not supported to set the symbol from a deferred context */
    if (desc->symbol) {
        const char *sym = ecs_get_symbol(world, entity);
        ecs_assert(!ecs_os_strcmp(sym, desc->symbol), ECS_UNSUPPORTED, NULL);
        (void)sym;
    }
}

ecs_entity_t ecs_entity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_entity_t scope = stage->scope;
    ecs_id_t with = ecs_get_with(world);

    const char *name = desc->name;
    const char *sep = desc->sep;
    if (!sep) {
        sep = ".";
    }

    const char *root_sep = desc->root_sep;
    bool new_entity = false;
    bool name_assigned = false;

    /* Remove optional prefix from name. Entity names can be derived from 
     * language identifiers, such as components (typenames) and systems
     * function names). Because C does not have namespaces, such identifiers
     * often encode the namespace as a prefix.
     * To ensure interoperability between C and C++ (and potentially other 
     * languages with namespacing) the entity must be stored without this prefix
     * and with the proper namespace, which is what the name_prefix is for */
    const char *prefix = world->name_prefix;
    if (name && prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(name, prefix, len) && 
           (isupper(name[len]) || name[len] == '_')) 
        {
            if (name[len] == '_') {
                name = name + len + 1;
            } else {
                name = name + len;
            }
        }
    }

    /* Find or create entity */
    ecs_entity_t result = desc->entity;
    if (!result) {
        if (name) {
            result = ecs_lookup_path_w_sep(
                world, scope, name, sep, root_sep, false);
            if (result) {
                name_assigned = true;
            }
        }

        if (!result) {
            if (desc->use_low_id) {
                result = ecs_new_low_id(world);
            } else {
                result = ecs_new_id(world);
            }
            new_entity = true;
            ecs_assert(ecs_get_type(world, result) == NULL, 
                ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        /* Make sure provided id is either alive or revivable */
        ecs_ensure(world, result);

        name_assigned = ecs_has_pair(
            world, result, ecs_id(EcsIdentifier), EcsName);
        if (name && name_assigned) {
            /* If entity has name, verify that name matches */
            char *path = ecs_get_path_w_sep(world, scope, result, sep, NULL);
            if (path) {
                if (ecs_os_strcmp(path, name)) {
                    /* Mismatching name */
                    ecs_os_free(path);
                    return 0;
                }
                ecs_os_free(path);
            }
        }
    }

    ecs_assert(name_assigned == ecs_has_pair(
        world, result, ecs_id(EcsIdentifier), EcsName),
            ECS_INTERNAL_ERROR, NULL);

    if (stage->defer) {
        deferred_add_remove((ecs_world_t*)stage, result, name, desc, 
            scope, with, new_entity, name_assigned);
    } else {
        if (traverse_add(world, result, name, desc,
            scope, with, new_entity, name_assigned)) 
        {
            return 0;
        }
    }

    return result;
error:
    return 0;
}

const ecs_entity_t* ecs_bulk_init(
    ecs_world_t *world,
    const ecs_bulk_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_entity_t *entities = desc->entities;
    int32_t count = desc->count;

    int32_t sparse_count = 0;
    if (!entities) {
        sparse_count = ecs_eis_count(world);
        entities = flecs_sparse_new_ids(world->store.entity_index, count);
        ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    } else {
        int i;
        for (i = 0; i < count; i ++) {
            ecs_ensure(world, entities[i]);
        }
    }

    ecs_ids_t ids;
    ecs_table_t *table = desc->table;
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    if (!table) {
        int32_t i = 0;
        ecs_id_t id;
        while ((id = desc->ids[i])) {
            table = table_append(world, table, id, &diff);
            i ++;
        }

        ids.array = (ecs_id_t*)desc->ids;
        ids.count = i;
    } else {
        diff.added.array = ecs_vector_first(table->type, ecs_id_t);
        diff.added.count = ecs_vector_count(table->type);

        ids = diff.added;
    }

    new_w_data(
        world, table, entities, &ids, count, desc->data, true, NULL, &diff);

    if (!sparse_count) {
        return entities;
    } else {
        /* Refetch entity ids, in case the underlying array was reallocated */
        entities = flecs_sparse_ids(world->store.entity_index);
        return &entities[sparse_count];
    }
error:
    return NULL;
}

ecs_entity_t ecs_component_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);

    ecs_suspend_readonly_state_t readonly_state;
    world = flecs_suspend_readonly(world, &readonly_state);

    ecs_entity_desc_t entity_desc = desc->entity;
    entity_desc.use_low_id = true;
    if (!entity_desc.symbol) {
        entity_desc.symbol = entity_desc.name;
    }

    ecs_entity_t e = desc->entity.entity;
    ecs_entity_t result = ecs_entity_init(world, &entity_desc);
    if (!result) {
        goto error;
    }

    bool added = false;
    EcsComponent *ptr = ecs_get_mut(world, result, EcsComponent, &added);

    if (added) {
        ptr->size = flecs_utosize(desc->size);
        ptr->alignment = flecs_utosize(desc->alignment);
        if (!ptr->size) {
            ecs_trace("#[green]tag#[reset] %s created", 
                ecs_get_name(world, result));
        } else {
            ecs_trace("#[green]component#[reset] %s created", 
                ecs_get_name(world, result));
        }
    } else {
        if (ptr->size != flecs_utosize(desc->size)) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, desc->entity.name);
        }
        if (ptr->alignment != flecs_utosize(desc->alignment)) {
            ecs_abort(ECS_INVALID_COMPONENT_ALIGNMENT, desc->entity.name);
        }
    }

    ecs_modified(world, result, EcsComponent);

    if (e >= world->stats.last_component_id && e < ECS_HI_COMPONENT_ID) {
        world->stats.last_component_id = e + 1;
    }

    /* Ensure components cannot be deleted */
    ecs_add_pair(world, result, EcsOnDelete, EcsThrow);

    flecs_resume_readonly(world, &readonly_state);
    
    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_has(world, result, EcsComponent), ECS_INTERNAL_ERROR, NULL);

    return result;
error:
    return 0;
}

ecs_entity_t ecs_type_init(
    ecs_world_t *world,
    const ecs_type_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = ecs_entity_init(world, &desc->entity);
    if (!result) {
        goto error;
    }

    ecs_table_t *table = NULL, *normalized = NULL;
    ecs_table_diff_t temp_diff, diff = ECS_TABLE_DIFF_INIT;

    /* Find destination table (and type) */

    /* Add components from the 'add' id array */
    int32_t i = 0;
    ecs_id_t id;
    const ecs_id_t *ids = desc->ids;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        normalized = flecs_table_traverse_add(
            world, normalized, &id, &temp_diff);
        table = flecs_table_traverse_add(world, table, &id, NULL);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
        diff_append(&diff, &temp_diff);
    }

    /* If expression is set, add it to the table */
    if (desc->ids_expr) {
#ifdef FLECS_PARSER
        bool error = false;

        normalized = traverse_from_expr(world, normalized, desc->entity.name, 
            desc->ids_expr, &diff, true, &error);
        if (error) {
            goto error;
        }

        table = traverse_from_expr(world, table, desc->entity.name, 
            desc->ids_expr, &diff, false, &error);
        if (error) {
            goto error;
        }
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    diff_free(&diff);

    ecs_type_t type = NULL;
    
    if (table) {
        type = table->type;
    }

    bool add = false;
    EcsType *type_ptr = ecs_get_mut(world, result, EcsType, &add);
    if (add) {
        type_ptr->type = type;
        type_ptr->normalized = normalized;

        /* This will allow the type to show up in debug tools */
        if (type) {
            ecs_map_set(world->type_handles, (uintptr_t)type, &result);
        }

        ecs_modified(world, result, EcsType);
    } else {
        ecs_check(type_ptr->type == type, ECS_ALREADY_DEFINED, 
            desc->entity.name);
        ecs_check(type_ptr->normalized == normalized, ECS_ALREADY_DEFINED,
            desc->entity.name);      
    }

    return result;
error:
    return 0;
}

const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t id,
    int32_t count)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    const ecs_entity_t *ids;
    if (flecs_defer_bulk_new(world, stage, count, id, &ids)) {
        return ids;
    }

    ecs_table_t *table = &world->store.root;
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    
    if (id) {
        table = table_append(world, table, id, &diff);
    }

    ids = new_w_data(world, table, NULL, NULL, count, NULL, false, NULL, &diff);
    flecs_defer_flush(world, stage);

    return ids;
error:
    return NULL;
}

void ecs_clear(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_clear(world, stage, entity)) {
        return;
    }

    ecs_entity_info_t info;
    info.table = NULL;

    flecs_get_info(world, entity, &info);

    ecs_table_t *table = info.table;
    if (table) {
        ecs_table_diff_t diff = {
            .removed = flecs_type_to_ids(table->type),
            .un_set = flecs_type_to_ids(table->storage_type)
        };

        delete_entity(world, table, &table->storage, info.row, &diff);
        info.record->table = NULL;
        info.record->row = 0;
    }    

    flecs_defer_flush(world, stage);
error:
    return;
}

static
void on_delete_any_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t action,
    uint32_t flags);

static
void throw_invalid_delete(
    ecs_world_t *world,
    ecs_id_t id)
{
    char *id_str = ecs_id_str(world, id);
    ecs_throw(ECS_CONSTRAINT_VIOLATED, id_str);
error:
    ecs_os_free(id_str);
}

static
void remove_from_table(
    ecs_world_t *world,
    ecs_table_t *src_table,
    ecs_id_t id,
    int32_t column,
    int32_t column_count)
{
    ecs_table_diff_t temp_diff, diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *dst_table = src_table; 
    ecs_id_t *ids = ecs_vector_first(src_table->type, ecs_id_t);

    /* If id is pair but the column pointed to is not a pair, the record is
     * pointing to an instance of the id that has a (non-PAIR) role. */
    bool is_pair = ECS_HAS_ROLE(id, PAIR);     
    bool is_role = is_pair && !ECS_HAS_ROLE(ids[column], PAIR);
    ecs_assert(!is_role || ((ids[column] & ECS_ROLE_MASK) != 0), 
        ECS_INTERNAL_ERROR, NULL);
    bool is_wildcard = ecs_id_is_wildcard(id);

    int32_t i, count = ecs_vector_count(src_table->type), removed_count = 0;
    ecs_entity_t entity = ECS_PAIR_RELATION(id);

    for (i = column; i < count; i ++) {
        ecs_id_t e = ids[i];

        if (is_role) {
            if ((e & ECS_COMPONENT_MASK) != entity) {
                continue;
            }
        } else if (is_wildcard && !ecs_id_match(e, id)) {
            continue;
        }

        dst_table = flecs_table_traverse_remove(
            world, dst_table, &e, &temp_diff);
        diff_append(&diff, &temp_diff);
        
        removed_count ++;
        if (removed_count == column_count) {
            break;
        }
    }

    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!dst_table->type) {
        /* If this removes all components, clear table */
        flecs_table_clear_entities(world, src_table);
    } else {
        /* Otherwise, merge table into dst_table */
        if (dst_table != src_table) {
            ecs_data_t *src_data = &src_table->storage;
            int32_t src_count = ecs_table_count(src_table);
            if (diff.removed.count) {
                flecs_notify_on_remove(world, src_table, NULL, 
                    0, src_count, &diff);
            }

            flecs_table_merge(world, dst_table, src_table, 
                &dst_table->storage, src_data);
        }
    }

    diff_free(&diff);
}

static
void delete_objects(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = &table->storage;
    if (data) {
        ecs_entity_t *entities = ecs_vector_first(
            data->entities, ecs_entity_t);
        ecs_record_t **records = ecs_vector_first(
            data->record_ptrs, ecs_record_t*);

        int32_t i, count = ecs_vector_count(data->entities);
        for (i = 0; i < count; i ++) {
            ecs_record_t *r = records[i];
            
            /* If entity is flagged, it could have delete actions. */
            uint32_t flags;
            if (r && (flags = (r->row & ECS_ROW_FLAGS_MASK))) {
                /* Strip mask to prevent infinite recursion */
                r->row = r->row & ECS_ROW_MASK;

                /* Run delete actions for objects */
                on_delete_any_w_entity(world, entities[i], 0, flags);
            }        
        }

        /* Clear components from table (invokes destructors, OnRemove) */
        flecs_table_delete_entities(world, table);            
    } 
}

static
void on_delete_object_action(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action)
{
    ecs_table_iter_t it;
    if (flecs_table_iter(world, id, &it)) {
        for (; it.cur < it.end; ++ it.cur) {
            const ecs_table_record_t *tr = it.cur;
            ecs_table_t *table = tr->table;
            ecs_assert(ecs_table_count(table) != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_id_t *rel_id = ecs_vector_get(table->type, ecs_id_t, tr->column);
            ecs_assert(rel_id != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_entity_t rel = ECS_PAIR_RELATION(*rel_id);
            /* delete_object_action should be invoked for relations */
            ecs_assert(rel != 0, ECS_INTERNAL_ERROR,  NULL);

            /* Find delete action for relation */
            if (!action) {
                ecs_id_record_t *idrr = flecs_get_id_record(world, rel);
                if (idrr) {
                    action = ECS_ID_ON_DELETE_OBJECT(idrr->flags);
                }
            }

            if (!action || action == EcsRemove) {
                remove_from_table(world, table, id, tr->column, tr->count);
            } else if (action == EcsDelete) {
                delete_objects(world, table);
            } else if (action == EcsThrow) {
                throw_invalid_delete(world, id);
            }
        }

        flecs_clear_id_record(world, id);
    }
}

static
void on_delete_id_action(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action)
{
    ecs_table_iter_t it;
    ecs_id_record_t *idr = flecs_table_iter(world, id, &it);
    if (idr) {
        if (!action) {
            action = ECS_ID_ON_DELETE(idr->flags);
        }

        if (action == EcsThrow) {
            throw_invalid_delete(world, id);
        }

        for (; it.cur < it.end; ++ it.cur) {
            const ecs_table_record_t *tr = it.cur;
            ecs_table_t *table = tr->table;

            if (!action || action == EcsRemove) {
                remove_from_table(world, table, id, tr->column, tr->count);
            } else if (action == EcsDelete) {
                delete_objects(world, table);
            }
        }

        flecs_clear_id_record(world, id);
    }
}

static
void on_delete_action(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action)
{
    if (ecs_id_is_wildcard(id)) {
        /* If id is wildcard, check if the relation or object is a wildcard.
         * Relation wildcard ids are implemented differently as relations
         * with the same object aren't guaranteed to occupy neighboring
         * elements in the type, other wildcards with the same relation. */
        if (ECS_PAIR_RELATION(id) == EcsWildcard) {
            on_delete_object_action(world, id, action);
        } else {
            on_delete_id_action(world, id, action);
        }
    } else {
        /* If the id is not a wildcard it's simple, as a table can only have
         * at most one instance of the id */
        on_delete_id_action(world, id, action);
    }
}

static
void on_delete_any_w_entity(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t action,
    uint32_t flags)
{
    /* Make sure any references to the entity are cleaned up */
    if (flags & ECS_FLAG_OBSERVED_ID) {
        on_delete_action(world, e, action);
        on_delete_action(world, ecs_pair(e, EcsWildcard), action);
    }
    if (flags & ECS_FLAG_OBSERVED_OBJECT) {
        on_delete_action(world, ecs_pair(EcsWildcard, e), action);
    }
}

void ecs_delete_with(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(world, stage, id, EcsDelete)) {
        return;
    }

    on_delete_action(world, id, EcsDelete);
    flecs_defer_flush(world, stage);
}

void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(world, stage, id, EcsRemove)) {
        return;
    }

    on_delete_action(world, id, EcsRemove);
    flecs_defer_flush(world, stage);
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_delete(world, stage, entity)) {
        return;
    }

    ecs_record_t *r = flecs_sparse_get(
        world->store.entity_index, ecs_record_t, entity);
    if (r) {
        ecs_entity_info_t info = {0};
        set_info_from_record(&info, r);

        ecs_table_t *table = info.table;
        uint64_t table_id = 0;
        if (table) {
            table_id = table->id;
        }

        if (info.row_flags) {
            /* Prevent infinite recursion in case of cyclic delete actions */
            r->row &= ECS_ROW_MASK;

            /* Ensure that the store contains no dangling references to the
             * deleted entity (as a component, or as part of a relation) */
            on_delete_any_w_entity(world, entity, 0, info.row_flags);

            /* Refetch data. In case of circular relations, the entity may have
             * moved to a different table. */
            set_info_from_record(&info, r);
            
            table = info.table;
            if (table) {
                table_id = table->id;
            } else {
                table_id = 0;
            }

            if (r->table) {
                ecs_ids_t to_remove = flecs_type_to_ids(r->table->type);
                update_component_monitors(world, entity, NULL, &to_remove);
            }
        }

        ecs_assert(!table_id || table, ECS_INTERNAL_ERROR, NULL);

        /* If entity has components, remove them. Check if table is still alive,
         * as delete actions could have deleted the table already. */
        if (table_id && flecs_sparse_is_alive(world->store.tables, table_id)) {
            ecs_table_diff_t diff = {
                .removed = flecs_type_to_ids(table->type),
                .un_set = flecs_type_to_ids(table->storage_type)
            };

            delete_entity(world, table, info.data, info.row, &diff);
            r->table = NULL;
        }

        r->row = 0;
        r->table = NULL;

        /* Remove (and invalidate) entity after executing handlers */
        flecs_sparse_remove(world->store.entity_index, entity);
    }

    flecs_defer_flush(world, stage);
error:
    return;
}

void ecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    add_id(world, entity, id);
error:
    return;
}

void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    remove_id(world, entity, id);
error:
    return;
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(src != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, src), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (!dst) {
        dst = ecs_new_id(world);
    }

    if (flecs_defer_clone(world, stage, dst, src, copy_value)) {
        return dst;
    }

    ecs_entity_info_t src_info;
    bool found = flecs_get_info(world, src, &src_info);
    ecs_table_t *src_table = src_info.table;

    if (!found || !src_table) {
        goto done;
    }

    ecs_type_t src_type = src_table->type;
    ecs_table_diff_t diff = {.added = flecs_type_to_ids(src_type)};

    ecs_entity_info_t dst_info = {0};
    dst_info.row = new_entity(world, dst, &dst_info, src_table, &diff, 
        true, true);

    if (copy_value) {
        flecs_table_move(world, dst, src, src_table, dst_info.data, 
            dst_info.row, src_table, src_info.data, src_info.row, true);

        flecs_notify_on_set(world, src_table, dst_info.row, 1, NULL, true);
    }

done:
    flecs_defer_flush(world, stage);
    return dst;
error:
    return 0;
}

const void* ecs_get_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(flecs_stage_from_readonly_world(world)->asynchronous == false, 
        ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = ecs_eis_get(world, entity);
    if (!r) {
        return NULL;
    }

    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    const ecs_table_record_t *tr = NULL;
    ecs_table_t *storage_table = table->storage_table;
    if (storage_table) {
        tr = flecs_id_record_table(idr, storage_table);
    } else {
        /* If the entity does not have a storage table (has no data) but it does
         * have the id, the id must be a tag, and getting a tag is illegal. */
        ecs_check(!ecs_owns_id(world, entity, id), ECS_NOT_A_COMPONENT, NULL);
    }

    if (!tr) {
       return get_base_component(world, table, id, idr, 0);
    }

    int32_t row = ECS_RECORD_TO_ROW(r->row);
    return get_component_w_index(table, tr->column, row);
error:
    return NULL;
}

const void* ecs_get_ref_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!entity || !ref->entity || entity == ref->entity, 
        ECS_INVALID_PARAMETER, NULL);
    ecs_check(!id || !ref->component || id == ref->component, 
        ECS_INVALID_PARAMETER, NULL);
    ecs_record_t *record = ref->record;

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    entity |= ref->entity;

    if (!record) {
        record = ecs_eis_get(world, entity);
    }

    if (!record || !record->table) {
        goto error;
    }

    ecs_table_t *table = record->table;

    if (ref->record == record &&
        ref->table == table &&
        ref->row == record->row &&
        ref->alloc_count == table->alloc_count)
    {
        return ref->ptr;
    }

    id |= ref->component;

    uint32_t row = record->row;
    ref->entity = entity;
    ref->component = id;
    ref->table = table;
    ref->row = row;
    ref->alloc_count = table->alloc_count;

    if (table && id) {
        ref->ptr = get_component(world, table, ECS_RECORD_TO_ROW(row), id);
    }

    ref->record = record;
    return ref->ptr;
error:
    return NULL;
}

void* ecs_get_mut_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_added)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    void *result;

    if (flecs_defer_set(
        world, stage, EcsOpMut, entity, id, 0, NULL, &result, is_added))
    {
        return result;
    }

    ecs_entity_info_t info;
    result = get_mutable(world, entity, id, &info, is_added);
    ecs_check(result != NULL, ECS_INVALID_PARAMETER, NULL);
    
    /* Store table so we can quickly check if returned pointer is still valid */
    ecs_table_t *table = info.record->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Keep track of alloc count of table, since even if the entity has not
     * moved, other entities could have been added to the table which could
     * reallocate arrays. Also store the row, as the entity could have 
     * reallocated. */
    int32_t alloc_count = table->alloc_count;
    uint32_t row = info.record->row;
    
    flecs_defer_flush(world, stage);

    /* Ensure that after flushing, the pointer is still valid. Flushing may
     * trigger callbacks, which could do anything with the entity */
    if (table != info.record->table || 
        alloc_count != info.record->table->alloc_count ||
        row != info.record->row) 
    {
        if (flecs_get_info(world, entity, &info) && info.table) {
            result = get_component(world, info.table, info.row, id);
        } else {
            /* A trigger has removed the component we just added. This is not
             * allowed, an application should always be able to assume that
             * get_mut returns a valid pointer. */
            ecs_throw(ECS_INVALID_OPERATION, NULL);
        }
    }

    return result;
error:
    return NULL;
}

void* ecs_emplace_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_check(!ecs_has_id(world, entity, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    void *result;

    if (flecs_defer_set(
        world, stage, EcsOpMut, entity, id, 0, NULL, &result, NULL))
    {
        return result;
    }

    ecs_entity_info_t info = {0};
    flecs_get_info(world, entity, &info);
    add_id_w_info(world, entity, &info, id, false /* Add without ctor */);

    void *ptr = get_component(world, info.table, info.row, id);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_defer_flush(world, stage);

    return ptr;
error:
    return NULL;
}

void ecs_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_modified(world, stage, entity, id)) {
        return;
    }

    /* If the entity does not have the component, calling ecs_modified is 
     * invalid. The assert needs to happen after the defer statement, as the
     * entity may not have the component when this function is called while
     * operations are being deferred. */
    ecs_check(ecs_has_id(world, entity, id), ECS_INVALID_PARAMETER, NULL);

    ecs_entity_info_t info = {0};
    if (flecs_get_info(world, entity, &info)) {
        ecs_ids_t ids = { .array = &id, .count = 1 };
        flecs_notify_on_set(world, info.table, info.row, 1, &ids, true);
    }

    flecs_table_mark_dirty(world, info.table, id);
    flecs_defer_flush(world, stage);
error:
    return;
}

static
ecs_entity_t assign_ptr_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    void *ptr,
    bool is_move,
    bool notify)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (!entity) {
        entity = ecs_new_id(world);
        ecs_entity_t scope = stage->scope;
        if (scope) {
            ecs_add_pair(world, entity, EcsChildOf, scope);
        }
    }

    if (flecs_defer_set(world, stage, EcsOpSet, entity, id, 
        flecs_utosize(size), ptr, NULL, NULL))
    {
        return entity;
    }

    ecs_entity_info_t info;

    void *dst = get_mutable(world, entity, id, &info, NULL);
    ecs_check(dst != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_entity_t real_id = ecs_get_typeid(world, id);
        const ecs_type_info_t *cdata = get_c_info(world, real_id);
        if (cdata) {
            if (is_move) {
                ecs_move_t move = cdata->lifecycle.move;
                if (move) {
                    move(world, real_id, &entity, &entity, dst, ptr, size, 1, 
                        cdata->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, ptr, flecs_utosize(size));
                }
            } else {
                ecs_copy_t copy = cdata->lifecycle.copy;
                if (copy) {
                    copy(world, real_id, &entity, &entity, dst, ptr, size, 1, 
                        cdata->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, ptr, flecs_utosize(size));
                }
            }
        } else {
            ecs_os_memcpy(dst, ptr, flecs_utosize(size));
        }
    } else {
        memset(dst, 0, size);
    }

    flecs_table_mark_dirty(world, info.table, id);

    if (notify) {
        ecs_ids_t ids = { .array = &id, .count = 1 };
        flecs_notify_on_set(world, info.table, info.row, 1, &ids, true);
    }

    flecs_defer_flush(world, stage);

    return entity;
error:
    return 0;
}

ecs_entity_t ecs_set_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!entity || ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Safe to cast away const: function won't modify if move arg is false */
    return assign_ptr_w_id(
        world, entity, id, size, (void*)ptr, false, true);
error:
    return 0;
}

ecs_entity_t ecs_get_case(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t sw_id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, sw_id), ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_entity_info_t info;
    ecs_table_t *table;
    if (!flecs_get_info(world, entity, &info) || !(table = info.table)) {
        return 0;
    }

    sw_id = sw_id | ECS_SWITCH;

    int32_t index = ecs_search(world, table, sw_id, 0);
    if (index == -1) {
        return 0;
    }

    index -= table->sw_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_assert(info.data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_switch_t *sw = info.data->sw_columns[index].data;  
    return flecs_switch_get(sw, info.row);  
error:
    return 0;
}

void ecs_enable_component_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_enable(
        world, stage, entity, id, enable))
    {
        return;
    } else {
        /* Operations invoked by enable/disable should not be deferred */
        stage->defer --;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_entity_t bs_id = (id & ECS_COMPONENT_MASK) | ECS_DISABLED;
    
    ecs_table_t *table = info.table;
    int32_t index = -1;
    if (table) {
        index = ecs_search(world, table, bs_id, 0);
    }

    if (index == -1) {
        ecs_add_id(world, entity, bs_id);
        ecs_enable_component_w_id(world, entity, id, enable);
        return;
    }

    index -= table->bs_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_assert(info.data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &info.data->bs_columns[index].data;
    ecs_assert(bs != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_bitset_set(bs, info.row, enable);
error:
    return;
}

bool ecs_is_component_enabled_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_entity_info_t info;
    ecs_table_t *table;
    if (!flecs_get_info(world, entity, &info) || !(table = info.table)) {
        return false;
    }

    ecs_entity_t bs_id = (id & ECS_COMPONENT_MASK) | ECS_DISABLED;
    int32_t index = ecs_search(world, table, bs_id, 0);
    if (index == -1) {
        /* If table does not have DISABLED column for component, component is
         * always enabled, if the entity has it */
        return ecs_has_id(world, entity, id);
    }

    index -= table->bs_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_assert(info.data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &info.data->bs_columns[index].data;  

    return flecs_bitset_get(bs, info.row);
error:
    return false;
}

bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    if (ECS_HAS_ROLE(id, CASE)) {
        ecs_entity_info_t info;
        ecs_table_t *table;
        if (!flecs_get_info(world, entity, &info) || !(table = info.table)) {
            return false;
        }

        int32_t index = flecs_table_switch_from_case(world, table, id);
        ecs_assert(index < table->sw_column_count, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = info.data;
        ecs_switch_t *sw = data->sw_columns[index].data;
        ecs_entity_t value = flecs_switch_get(sw, info.row);

        return value == (id & ECS_COMPONENT_MASK);
    } else {
        ecs_table_t *table = ecs_get_table(world, entity);
        if (!table) {
            return false;
        }

        return ecs_search_relation(
            world, table, 0, id, EcsIsA, 0, 0, 
                NULL, NULL, NULL) != -1;
    }
error:
    return false;
}

ecs_entity_t ecs_get_object(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    int32_t index)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(rel != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_table_t *table = ecs_get_table(world, entity);
    if (!table) {
        return 0;
    }

    ecs_id_t wc = ecs_pair(rel, EcsWildcard);
    ecs_table_record_t *tr = flecs_get_table_record(world, table, wc);
    if (!tr) {
        return 0;
    }

    if (index >= tr->count) {
        return 0;
    }

    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    return ecs_pair_object(world, ids[tr->column + index]);
error:
    return 0;
}

ecs_entity_t ecs_get_object_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t id)
{
    ecs_table_t *table = ecs_get_table(world, entity);
    ecs_entity_t subject = 0;

    if (rel) {
        int32_t column = ecs_search_relation(
            world, table, 0, id, rel, 0, 0, &subject, NULL, NULL);
        if (column == -1) {
            return 0;
        }
    } else {
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
        int32_t i, count = ecs_vector_count(table->type);

        for (i = 0; i < count; i ++) {
            ecs_id_t ent = ids[i];
            if (ent & ECS_ROLE_MASK) {
                /* Skip ids with pairs, roles since 0 was provided for rel */
                break;
            }

            if (ecs_has_id(world, ent, id)) {
                subject = ent;
                break;
            }
        }
    }

    if (subject == 0) {
        return entity;
    } else {
        return subject;
    }
}

static
const char* get_identifier(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    const EcsIdentifier *ptr = ecs_get_pair(
        world, entity, EcsIdentifier, tag);

    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
error:
    return NULL;
}

const char* ecs_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    return get_identifier(world, entity, EcsName);
}

const char* ecs_get_symbol(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    return get_identifier(world, entity, EcsSymbol);
}

static
ecs_entity_t set_identifier(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag,
    const char *name)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    if (!entity) {
        entity = ecs_new_id(world);
    }
    
    ecs_set_pair(world, entity, EcsIdentifier, tag, {.value = (char*)name});
    return entity;
error:
    return 0;
}


ecs_entity_t ecs_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    return set_identifier(world, entity, EcsName, name);
}

ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    return set_identifier(world, entity, EcsSymbol, name);
}

ecs_id_t ecs_make_pair(
    ecs_entity_t relation,
    ecs_entity_t object)
{
    return ecs_pair(relation, object);
}

bool ecs_is_valid(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    /* 0 is not a valid entity id */
    if (!entity) {
        return false;
    }
    
    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    /* When checking roles and/or pairs, the generation count may have been
     * stripped away. Just test if the entity is 0 or not. */
    if (ECS_HAS_ROLE(entity, PAIR)) {
        return ECS_PAIR_RELATION(entity) != 0;
    }

    /* Entities should not contain data in dead zone bits */
    if (entity & ~0xFF00FFFFFFFFFFFF) {
        return false;
    }

    if (entity & ECS_ROLE) {
        return ecs_entity_t_lo(entity) != 0;
    }

    /* If entity doesn't exist in the world, the id is valid as long as the
     * generation is 0. Using a non-existing id with a non-zero generation
     * requires calling ecs_ensure first. */
    if (!ecs_exists(world, entity)) {
        return ECS_GENERATION(entity) == 0;
    }

    /* If id exists, it must be alive (the generation count must match) */
    return ecs_is_alive(world, entity);
error:
    return false;
}

ecs_id_t ecs_strip_generation(
    ecs_entity_t e)
{
    /* If this is not a pair, erase the generation bits */
    if (!(e & ECS_ROLE_MASK)) {
        e &= ~ECS_GENERATION_MASK;
    }

    return e;
}

bool ecs_is_alive(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    return ecs_eis_is_alive(world, entity);
error:
    return false;
}

ecs_entity_t ecs_get_alive(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (!entity) {
        return 0;
    }

    if (ecs_is_alive(world, entity)) {
        return entity;
    }

    /* Make sure id does not have generation. This guards against accidentally
     * "upcasting" a not alive identifier to a alive one. */
    ecs_assert((uint32_t)entity == entity, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_entity_t current = ecs_eis_get_current(world, entity);
    if (!current || !ecs_is_alive(world, current)) {
        return 0;
    }

    return current;
error:
    return 0;
}

void ecs_ensure(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_poly_assert(world, ecs_world_t); /* Cannot be a stage */
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    /* Check if a version of the provided id is alive */
    ecs_entity_t any = ecs_get_alive(world, ecs_strip_generation(entity));
    if (any == entity) {
        /* If alive and equal to the argument, there's nothing left to do */
        return;
    }

    /* If the id is currently alive but did not match the argument, fail */
    ecs_check(!any, ECS_INVALID_PARAMETER, NULL);

    /* Set generation if not alive. The sparse set checks if the provided
     * id matches its own generation which is necessary for alive ids. This
     * check would cause ecs_ensure to fail if the generation of the 'entity'
     * argument doesn't match with its generation.
     * 
     * While this could've been addressed in the sparse set, this is a rare
     * scenario that can only be triggered by ecs_ensure. Implementing it here
     * allows the sparse set to not do this check, which is more efficient. */
    ecs_eis_set_generation(world, entity);

    /* Ensure id exists. The underlying datastructure will verify that the
     * generation count matches the provided one. */
    ecs_eis_ensure(world, entity);
error:
    return;
}

void ecs_ensure_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_HAS_ROLE(id, PAIR) || ECS_HAS_ROLE(id, CASE)) {
        ecs_entity_t r = ECS_PAIR_RELATION(id);
        ecs_entity_t o = ECS_PAIR_OBJECT(id);

        ecs_check(r != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_check(o != 0, ECS_INVALID_PARAMETER, NULL);

        if (ecs_get_alive(world, r) == 0) {
            ecs_ensure(world, r);
        }
        if (ecs_get_alive(world, o) == 0) {
            ecs_ensure(world, o);
        }
    } else {
        ecs_ensure(world, id & ECS_COMPONENT_MASK);
    }
error:
    return;
}

bool ecs_exists(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    return ecs_eis_exists(world, entity);
error:
    return false;
}

ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    
    world = ecs_get_world(world);

    ecs_record_t *record = ecs_eis_get(world, entity);
    ecs_table_t *table;
    if (record && (table = record->table)) {
        return table;
    }
error:
    return NULL;
}

ecs_table_t* ecs_get_storage_table(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
       return table->storage_table;
    }

    return NULL;   
}

ecs_type_t ecs_get_type(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
        return table->type;
    }

    return NULL;
}

ecs_id_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    if (ECS_HAS_ROLE(id, PAIR)) {
        /* Make sure we're not working with a stage */
        world = ecs_get_world(world);

        ecs_entity_t rel = ecs_get_alive(world, ECS_PAIR_RELATION(id));

        /* If relation is marked as a tag, it never has data. Return relation */
        if (ecs_has_id(world, rel, EcsTag)) {
            return 0;
        }

        const EcsComponent *ptr = ecs_get(world, rel, EcsComponent);
        if (ptr && ptr->size != 0) {
            return rel;
        }
        
        ecs_entity_t obj = ECS_PAIR_OBJECT(id);
        if (obj) {
            obj = ecs_get_alive(world, obj);
            ptr = ecs_get(world, obj, EcsComponent);
            
            if (ptr && ptr->size != 0) {
                return obj;
            }
        }

        /* Neither relation nor object have data */
        return 0;
    } else if (id & ECS_ROLE_MASK) {
        return 0;
    } else {
        const EcsComponent *ptr = ecs_get(world, id, EcsComponent);
        if (!ptr || !ptr->size) {
            return 0;
        }
    }

    return id;
error:
    return 0;
}

int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_entity_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!id) {
        return 0;
    }

    int32_t count = 0;
    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) { .id = id });
    while (ecs_term_next(&it)) {
        count += it.count;
    }

    return count;
error:
    return 0;
}

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled)
{
    const EcsType *type_ptr = ecs_get(world, entity, EcsType);
    if (type_ptr) {
        /* If entity is a type, disable all entities in the type */
        ecs_vector_each(type_ptr->normalized->type, ecs_entity_t, e, {
            ecs_enable(world, *e, enabled);
        });
    } else {
        if (enabled) {
            ecs_remove_id(world, entity, EcsDisabled);
        } else {
            ecs_add_id(world, entity, EcsDisabled);
        }
    }
}

bool ecs_defer_begin(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_none(world, stage);
error:
    return false;
}

bool ecs_defer_end(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_flush(world, stage);
error:
    return false;
}

const char* ecs_role_str(
    ecs_entity_t entity)
{
    if (ECS_HAS_ROLE(entity, PAIR)) {
        return "PAIR";
    } else
    if (ECS_HAS_ROLE(entity, DISABLED)) {
        return "DISABLED";
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
    if (ECS_HAS_ROLE(entity, OVERRIDE)) {
        return "OVERRIDE";
    } else {
        return "UNKNOWN";
    }
}

void ecs_id_str_buf(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    if (id & ECS_ROLE_MASK && !ECS_HAS_ROLE(id, PAIR)) {
        ecs_strbuf_appendstr(buf, ecs_role_str(id));
        ecs_strbuf_appendch(buf, '|');
    }

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t rel = ECS_PAIR_RELATION(id);
        ecs_entity_t obj = ECS_PAIR_OBJECT(id);

        ecs_entity_t e;
        if ((e = ecs_get_alive(world, rel))) {
            rel = e;
        }
        if ((e = ecs_get_alive(world, obj))) {
            obj = e;
        }

        ecs_strbuf_appendch(buf, '(');
        ecs_get_path_w_sep_buf(world, 0, rel, NULL, NULL, buf);
        ecs_strbuf_appendch(buf, ',');
        ecs_get_path_w_sep_buf(world, 0, obj, NULL, NULL, buf);
        ecs_strbuf_appendch(buf, ')');
    } else {
        ecs_entity_t e = id & ECS_COMPONENT_MASK;
        ecs_get_path_w_sep_buf(world, 0, e, NULL, NULL, buf);
    }

error:
    return;
}

char* ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_id_str_buf(world, id, &buf);
    return ecs_strbuf_get(&buf);
}


char* ecs_type_str(
    const ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t id = ids[i];

        if (i) {
            ecs_strbuf_appendch(&buf, ',');
            ecs_strbuf_appendch(&buf, ' ');
        }

        if (id == 1) {
            ecs_strbuf_appendstr(&buf, "Component");
        } else {
            ecs_id_str_buf(world, id, &buf);
        }
    }

    return ecs_strbuf_get(&buf);
}

static
void flush_bulk_new(
    ecs_world_t *world,
    ecs_defer_op_t *op)
{
    ecs_entity_t *entities = op->is._n.entities;

    if (op->id) {
        int i, count = op->is._n.count;
        for (i = 0; i < count; i ++) {
            add_id(world, entities[i], op->id);
        }
    }

    ecs_os_free(entities);
}

static
void free_value(
    ecs_world_t *world,
    ecs_entity_t *entities,
    ecs_id_t id,
    void *value,
    int32_t count)
{
    ecs_entity_t real_id = ecs_get_typeid(world, id);
    const ecs_type_info_t *info = flecs_get_c_info(world, real_id);
    ecs_xtor_t dtor;
    
    if (info && (dtor = info->lifecycle.dtor)) {
        ecs_size_t size = info->size;
        void *ptr;
        int i;
        for (i = 0, ptr = value; i < count; i ++, ptr = ECS_OFFSET(ptr, size)) {
            dtor(world, id, &entities[i], ptr, flecs_itosize(size), 1, 
                info->lifecycle.ctx);
        }
    }
}

static
void discard_op(
    ecs_world_t *world,
    ecs_defer_op_t *op)
{
    if (op->kind != EcsOpBulkNew) {
        void *value = op->is._1.value;
        if (value) {
            free_value(world, &op->is._1.entity, op->id, op->is._1.value, 1);
            ecs_os_free(value);
        }
    } else {
        ecs_os_free(op->is._n.entities);
    }
}

static 
bool is_entity_valid(
    ecs_world_t *world,
    ecs_entity_t e)
{
    if (ecs_exists(world, e) && !ecs_is_alive(world, e)) {
        return false;
    }
    return true;
}

static
bool remove_invalid(
    ecs_world_t *world,
    ecs_id_t *id_out)
{
    ecs_id_t id = *id_out;

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t rel = ecs_pair_relation(world, id);
        if (!rel || !is_entity_valid(world, rel)) {
            /* After relation is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        } else {
            ecs_entity_t obj = ecs_pair_object(world, id);
            if (!obj || !is_entity_valid(world, obj)) {
                /* Check the relation's policy for deleted objects */
                ecs_id_record_t *idr = flecs_get_id_record(world, rel);
                if (idr) {
                    ecs_entity_t action = ECS_ID_ON_DELETE_OBJECT(idr->flags);
                    if (action == EcsDelete) {
                        /* Entity should be deleted, don't bother checking
                         * other ids */
                        return false;
                    } else if (action == EcsThrow) {
                        /* If policy is throw this object should not have
                         * been deleted */
                        throw_invalid_delete(world, id);
                    } else {
                        *id_out = 0;
                        return true;
                    }
                } else {
                    *id_out = 0;
                    return true;
                }
            }
        }
    } else {
        id &= ECS_COMPONENT_MASK;
        if (!is_entity_valid(world, id)) {
            /* After relation is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        }
    }

    return true;
}

/* Leave safe section. Run all deferred commands. */
bool flecs_defer_flush(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!--stage->defer) {
        /* Set to NULL. Processing deferred commands can cause additional
         * commands to get enqueued (as result of reactive systems). Make sure
         * that the original array is not reallocated, as this would complicate
         * processing the queue. */
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;

        if (defer_queue) {
            ecs_defer_op_t *ops = ecs_vector_first(defer_queue, ecs_defer_op_t);
            int32_t i, count = ecs_vector_count(defer_queue);
            
            for (i = 0; i < count; i ++) {
                ecs_defer_op_t *op = &ops[i];
                ecs_entity_t e = op->is._1.entity;
                if (op->kind == EcsOpBulkNew) {
                    e = 0;
                }

                /* If entity is no longer alive, this could be because the queue
                 * contained both a delete and a subsequent add/remove/set which
                 * should be ignored. */
                if (e && !ecs_is_alive(world, e) && ecs_eis_exists(world, e)) {
                    ecs_assert(op->kind != EcsOpNew && op->kind != EcsOpClone, 
                        ECS_INTERNAL_ERROR, NULL);
                    world->discard_count ++;
                    discard_op(world, op);
                    continue;
                }

                switch(op->kind) {
                case EcsOpNew:
                case EcsOpAdd:
                    ecs_assert(op->id != 0, ECS_INTERNAL_ERROR, NULL);
                    if (remove_invalid(world, &op->id)) {
                        if (op->id) {
                            world->add_count ++;
                            add_id(world, e, op->id);
                        }
                    } else {
                        ecs_delete(world, e);
                    }
                    break;
                case EcsOpRemove:
                    remove_id(world, e, op->id);
                    break;
                case EcsOpClone:
                    ecs_clone(world, e, op->id, op->is._1.clone_value);
                    break;
                case EcsOpSet:
                    assign_ptr_w_id(world, e, 
                        op->id, flecs_itosize(op->is._1.size), 
                        op->is._1.value, true, true);
                    break;
                case EcsOpMut:
                    assign_ptr_w_id(world, e, 
                        op->id, flecs_itosize(op->is._1.size), 
                        op->is._1.value, true, false);
                    break;
                case EcsOpModified:
                    ecs_modified_id(world, e, op->id);
                    break;
                case EcsOpDelete: {
                    ecs_delete(world, e);
                    break;
                }
                case EcsOpClear:
                    ecs_clear(world, e);
                    break;
                case EcsOpOnDeleteAction:
                    on_delete_action(world, op->id, e);
                    break;
                case EcsOpEnable:
                    ecs_enable_component_w_id(world, e, op->id, true);
                    break;
                case EcsOpDisable:
                    ecs_enable_component_w_id(world, e, op->id, false);
                    break;
                case EcsOpBulkNew:
                    flush_bulk_new(world, op);
                    continue;
                }

                if (op->is._1.value) {
                    ecs_os_free(op->is._1.value);
                }                  
            }

            if (stage->defer_queue) {
                ecs_vector_free(stage->defer_queue);
            }

            /* Restore defer queue */
            ecs_vector_clear(defer_queue);
            stage->defer_queue = defer_queue;
        }

        return true;
    }

error:
    return false;
}

/* Delete operations from queue without executing them. */
bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!--stage->defer) {
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;

        if (defer_queue) {
            ecs_defer_op_t *ops = ecs_vector_first(defer_queue, ecs_defer_op_t);
            int32_t i, count = ecs_vector_count(defer_queue);
            for (i = 0; i < count; i ++) {
                discard_op(world, &ops[i]);
            }

            if (stage->defer_queue) {
                ecs_vector_free(stage->defer_queue);
            }

            /* Restore defer queue */
            ecs_vector_clear(defer_queue);
            stage->defer_queue = defer_queue;
        }

        return true;
    }

error:
    return false;
}
