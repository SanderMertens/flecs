#include "private_api.h"
#include <ctype.h>

static
void flecs_notify_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    ecs_table_diff_t *diff,
    bool run_on_set);

static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_entity_t *entities,
    ecs_type_t *component_ids,
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
    ecs_check(column_index < table->storage_count, ECS_NOT_A_COMPONENT, NULL);
    ecs_type_info_t *ti = table->type_info[column_index];
    ecs_column_t *column = &table->data.columns[column_index];
    return ecs_storage_get(column, ti->size, row);;
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

    ecs_table_record_t *tr = flecs_table_record_get(
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
    const ecs_table_record_t *tr_isa = flecs_id_record_get_table(
        world->idr_isa_wildcard, table);
    ecs_check(tr_isa != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t i = tr_isa->column, end = tr_isa->count + tr_isa->column;
    void *ptr = NULL;

    do {
        ecs_id_t pair = ids[i ++];
        ecs_entity_t base = ecs_pair_second(world, pair);

        ecs_record_t *r = flecs_entities_get(world, base);
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
            tr = flecs_id_record_get_table(table_index, storage_table);
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

static
const ecs_type_info_t *get_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_entity_t real_id = ecs_get_typeid(world, component);
    if (real_id) {
        return flecs_type_info_get(world, real_id);
    } else {
        return NULL;
    }
}

/* Utilities for creating a diff struct on the fly between two arbitrary tables.
 * This is temporary code that will eventually be replaced by a cache that 
 * stores the diff between two archetypes. */

typedef struct {
    ecs_type_t type;
    ecs_size_t size;
} ecs_type_buffer_t;

typedef struct {
    ecs_type_buffer_t added;
    ecs_type_buffer_t removed;
    ecs_type_buffer_t on_set;
    ecs_type_buffer_t un_set;
} ecs_diff_buffer_t;

static
void ids_merge(
    ecs_type_buffer_t *ids,
    ecs_type_t *add)
{
    if (!add || !add->count) {
        return;
    }
    
    int32_t new_count = ids->type.count + add->count;
    if (new_count > ids->size) {
        ids->size = flecs_next_pow_of_2(new_count);
        ecs_id_t *arr = ecs_os_malloc_n(ecs_id_t, ids->size);
        ecs_os_memcpy_n(arr, ids->type.array, ecs_id_t, ids->type.count);

        if (ids->type.count > ECS_ID_CACHE_SIZE) {
            ecs_os_free(ids->type.array);
        }
        
        ids->type.array = arr;
    }

    ecs_os_memcpy_n(&ids->type.array[ids->type.count], 
        add->array, ecs_id_t, add->count);
    ids->type.count += add->count;
}

#define ECS_DIFF_INIT {\
    .added = { .type = { .count = 0, .array = (ecs_id_t[ECS_ID_CACHE_SIZE]){0}}, .size = ECS_ID_CACHE_SIZE },\
    .removed = { .type = { .count = 0, .array = (ecs_id_t[ECS_ID_CACHE_SIZE]){0}}, .size = ECS_ID_CACHE_SIZE },\
    .on_set = { .type = { .count = 0, .array = (ecs_id_t[ECS_ID_CACHE_SIZE]){0}}, .size = ECS_ID_CACHE_SIZE },\
    .un_set = { .type = { .count = 0, .array = (ecs_id_t[ECS_ID_CACHE_SIZE]){0}}, .size = ECS_ID_CACHE_SIZE },\
}

static
void diff_append(
    ecs_diff_buffer_t *dst,
    ecs_table_diff_t *src)
{
    ids_merge(&dst->added, &src->added);
    ids_merge(&dst->removed, &src->removed);
    ids_merge(&dst->on_set, &src->on_set);
    ids_merge(&dst->un_set, &src->un_set);
}

static
void diff_free(
    ecs_diff_buffer_t *diff)
{
    if (diff->added.type.count > ECS_ID_CACHE_SIZE) {
        ecs_os_free(diff->added.type.array);
    }
    if (diff->removed.type.count > ECS_ID_CACHE_SIZE) {
        ecs_os_free(diff->removed.type.array);
    }
    if (diff->on_set.type.count > ECS_ID_CACHE_SIZE) {
        ecs_os_free(diff->on_set.type.array);
    }
    if (diff->un_set.type.count > ECS_ID_CACHE_SIZE) {
        ecs_os_free(diff->un_set.type.array);
    }
}

static
ecs_table_diff_t diff_to_table_diff(
    ecs_diff_buffer_t *diff)
{
    return (ecs_table_diff_t){
        .added = diff->added.type,
        .removed = diff->removed.type,
        .on_set = diff->on_set.type,
        .un_set = diff->un_set.type
    };
}

static
ecs_table_t* table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_diff_buffer_t *diff)
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
void flecs_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    ecs_entity_t event,
    ecs_type_t *ids,
    ecs_entity_t relation)
{
    flecs_emit(world, world, &(ecs_event_desc_t){
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
    int32_t row,
    int32_t count);

static
void instantiate_children(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_table_t *child_table)
{
    if (!ecs_table_count(child_table)) {
        return;
    }

    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = &child_table->data;

    ecs_entity_t *ids = type.array;
    int32_t type_count = type.count;

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_type_t components = {
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
        if (ECS_HAS_RELATION(id, EcsChildOf) && (ECS_PAIR_SECOND(id) == base)) {
            childof_base_index = pos;
        }

        int32_t storage_index = ecs_table_type_to_storage_index(child_table, i);
        if (storage_index != -1) {
            ecs_column_t *column = &child_data->columns[storage_index];
            component_data[pos] = ecs_storage_first(column);
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
    ecs_entity_t *entities = ecs_storage_first(&table->data.entities);
    int32_t child_count = ecs_storage_count(&child_data->entities);

    for (i = row; i < count + row; i ++) {
        ecs_entity_t instance = entities[i];
        ecs_diff_buffer_t diff = ECS_DIFF_INIT;
        ecs_table_t *i_table = NULL;
 
        /* Replace ChildOf element in the component array with instance id */
        components.array[childof_base_index] = ecs_pair(EcsChildOf, instance);

        /* Find or create table */
        for (j = 0; j < components.count; j ++) {
            i_table = table_append(world, i_table, components.array[j], &diff);
        }

        ecs_assert(i_table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(i_table->type.count == components.count,
            ECS_INTERNAL_ERROR, NULL);

        /* The instance is trying to instantiate from a base that is also
         * its parent. This would cause the hierarchy to instantiate itself
         * which would cause infinite recursion. */
        ecs_entity_t *children = ecs_storage_first(&child_data->entities);

#ifdef FLECS_DEBUG
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
        ecs_table_diff_t table_diff = diff_to_table_diff(&diff);
        new_w_data(world, i_table, NULL, &components, child_count, 
            component_data, false, &child_row, &table_diff);
        diff_free(&diff);

        /* If prefab child table has children itself, recursively instantiate */
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, child, i_table, child_row + j, 1);
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
    int32_t row,
    int32_t count)
{
    ecs_table_t *base_table = ecs_get_table(world, ecs_get_alive(world, base));
    if (!base_table || !(base_table->flags & EcsTableIsPrefab)) {
        /* Don't instantiate children from base entities that aren't prefabs */
        return;
    }

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);
    
    ecs_id_record_t *idr = flecs_id_record_get(world, ecs_childof(base));
    ecs_table_cache_iter_t it;
    if (idr && flecs_table_cache_iter((ecs_table_cache_t*)idr, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            instantiate_children(
                world, base, table, row, count, tr->hdr.table);
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
    const ecs_type_info_t *ti,
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
    const ecs_type_info_t *ti,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool notify_on_set)
{
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *r = flecs_entities_get(world, base);
    ecs_table_t *base_table;
    if (!r || !(base_table = r->table)) {
        return false;
    }

    void *base_ptr = get_component(
        world, base_table, ECS_RECORD_TO_ROW(r->row), component);
    if (base_ptr) {
        int32_t index, data_size = ti->size;
        void *data_ptr = ecs_storage_get(column, data_size, row);

        ecs_copy_t copy = ti->hooks.copy;
        if (copy) {
            for (index = 0; index < count; index ++) {
                copy(data_ptr, base_ptr, 1, ti);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }
        } else {
            for (index = 0; index < count; index ++) {
                ecs_os_memcpy(data_ptr, base_ptr, data_size);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }                    
        }

        ecs_type_t ids = {
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
                EcsIsA, 1, 0, 0, 0, 0, 0);
            if (c == -1) {
                flecs_notify(
                    world, table, other_table, row, count, EcsOnSet, &ids, 0);
            }
        }

        return true;
    } else {
        /* If component not found on base, check if base itself inherits */
        ecs_type_t base_type = base_table->type;
        return override_component(world, component, base_type, table, 
            other_table, ti, column, row, count, notify_on_set);
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_table_t *other_table,
    const ecs_type_info_t *ti,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool notify_on_set)
{
    ecs_entity_t *type_array = type.array;
    int32_t i, type_count = type.count;

    /* Walk prefabs */
    i = type_count - 1;
    do {
        ecs_entity_t e = type_array[i];

        if (!(e & ECS_ROLE_MASK)) {
            break;
        }

        if (ECS_HAS_RELATION(e, EcsIsA)) {
            if (override_from_base(world, ecs_pair_second(world, e), component,
                table, other_table, ti, column, row, count, notify_on_set))
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
    int32_t row,
    int32_t count,
    ecs_type_t *added,
    bool notify_on_set)
{
    ecs_column_t *columns = table->data.columns;
    ecs_type_t type = table->type;
    ecs_table_t *storage_table = table->storage_table;

    int i;
    for (i = 0; i < added->count; i ++) {
        ecs_entity_t id = added->array[i];

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            ecs_entity_t base = ECS_PAIR_SECOND(id);

            /* Cannot inherit from base if base is final */
            ecs_check(!ecs_has_id(world, ecs_get_alive(world, base), EcsFinal),
                ECS_CONSTRAINT_VIOLATED, NULL);
            ecs_check(base != 0, ECS_INVALID_PARAMETER, NULL);

            if (!world->stages[0].base) {
                /* Setting base prevents instantiating the hierarchy multiple
                 * times. The instantiate function recursively iterates the
                 * hierarchy to instantiate children. While this is happening,
                 * new tables are created which end up calling this function,
                 * which would call instantiate multiple times for the same
                 * level in the hierarchy. */
                world->stages[0].base = base;
                instantiate(world, base, table, row, count);
                world->stages[0].base = 0;
            }
        }

        if (!storage_table) {
            continue;
        }

        ecs_table_record_t *tr = flecs_table_record_get(
            world, storage_table, id);
        if (!tr) {
            continue;
        }

        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        if (idr->flags & EcsIdDontInherit) {
            continue;
        }

        const ecs_type_info_t *ti = idr->type_info;
        if (!ti->size) {
            continue;
        }

        ecs_column_t *column = &columns[tr->column];
        override_component(world, id, type, table, other_table, ti, 
            column, row, count, notify_on_set);
    }
error:
    return;
}

static
void flecs_set_union(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,    
    ecs_type_t *ids,
    bool reset)
{
    ecs_id_t *array = ids->array;
    int32_t i, id_count = ids->count;

    for (i = 0; i < id_count; i ++) {
        ecs_id_t id = array[i];
        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_id_record_t *idr = flecs_id_record_get(world, 
                ecs_pair(EcsUnion, ECS_PAIR_FIRST(id)));
            if (!idr) {
                continue;
            }

            const ecs_table_record_t *tr = flecs_id_record_get_table(
                idr, table);
            ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
            int32_t column = tr->column - table->sw_offset;
            ecs_switch_t *sw = &table->data.sw_columns[column];
            ecs_entity_t union_case = 0;
            if (!reset) {
                union_case = ECS_PAIR_SECOND(id);
            }

            int32_t r;
            for (r = 0; r < count; r ++) {
                flecs_switch_set(sw, row + r, union_case);
            }
        }
    }
}

static
void flecs_add_remove_union(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_type_t *added,
    ecs_type_t *removed)
{
    if (added) {
        flecs_set_union(world, table, row, count, added, false);
    }
    if (removed) {
        flecs_set_union(world, table, row, count, removed, true);
    } 
}

static
ecs_record_t* new_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *new_table,
    ecs_table_diff_t *diff,
    bool construct,
    bool notify_on_set)
{
    int32_t new_row;

    if (!record) {
        record = flecs_entities_ensure(world, entity);
    }

    new_row = flecs_table_append(world, new_table, entity, record, construct);

    record->table = new_table;
    record->row = ECS_ROW_TO_RECORD(new_row, record->row & ECS_ROW_FLAGS_MASK);

    ecs_data_t *new_data = &new_table->data;
    ecs_assert(ecs_storage_count(&new_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);
    (void)new_data;

    ecs_flags32_t flags = new_table->flags;

    if (flags & EcsTableHasAddActions) {
        flecs_notify_on_add(
            world, new_table, NULL, new_row, 1, diff, notify_on_set);       
    }

    return record;
}

static
void move_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *dst_table,
    ecs_table_diff_t *diff,
    bool construct,
    bool notify_on_set)
{
    ecs_table_t *src_table = record->table;
    int32_t src_row = ECS_RECORD_TO_ROW(record->row);
    
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_table != dst_table, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_storage_count(&src_table->data.entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record == flecs_entities_get(world, entity), ECS_INTERNAL_ERROR, NULL);

    int32_t dst_row = flecs_table_append(world, dst_table, entity, 
        record, false);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type.count) {
        flecs_notify_on_remove(world, src_table, dst_table, 
            ECS_RECORD_TO_ROW(src_row), 1, diff);

        flecs_table_move(world, entity, entity, dst_table, dst_row, 
            src_table, src_row, construct);                
    }

    /* Update entity index & delete old data after running remove actions */
    record->table = dst_table;
    record->row = ECS_ROW_TO_RECORD(dst_row, record->row & ECS_ROW_FLAGS_MASK);
    
    flecs_table_delete(world, src_table, src_row, false);

    /* If components were added, invoke add actions */
    ecs_flags32_t dst_flags = dst_table->flags;
    if (src_table != dst_table || diff->added.count) {
        if (diff->added.count && (dst_flags & EcsTableHasAddActions)) {
            flecs_notify_on_add(world, dst_table, src_table, dst_row, 1, diff, 
                notify_on_set);
        }
    }

error:
    return;
}

static
void delete_entity(
    ecs_world_t *world,
    ecs_record_t *record,
    ecs_table_diff_t *diff)
{    
    ecs_table_t *table = record->table;
    int32_t row = ECS_RECORD_TO_ROW(record->row);

    /* Invoke remove actions before deleting */
    if (table->flags & EcsTableHasRemoveActions) {   
        flecs_notify_on_remove(world, table, NULL, row, 1, diff);
    }

    flecs_table_delete(world, table, row, true);
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
    ecs_type_t *ids)
{
    if (!ids) {
        return;
    }

    int i;
    for (i = 0; i < ids->count; i ++) {
        ecs_entity_t id = ids->array[i];
        if (ECS_HAS_ROLE(id, PAIR)) {
            flecs_monitor_mark_dirty(world, 
                ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard));
        }

        flecs_monitor_mark_dirty(world, id);
    }
}

static
void update_component_monitors(
    ecs_world_t *world,
    ecs_type_t *added,
    ecs_type_t *removed)
{
    update_component_monitor_w_array(world, added);
    update_component_monitor_w_array(world, removed);
}

static
void flecs_commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *dst_table,   
    ecs_table_diff_t *diff,
    bool construct,
    bool notify_on_set)
{
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *src_table = record ? record->table : NULL;
    if (src_table == dst_table) {
        /* If source and destination table are the same no action is needed *
         * However, if a component was added in the process of traversing a
         * table, this suggests that a case switch could have occured. */
        if (((diff->added.count) || (diff->removed.count)) && 
             src_table && src_table->flags & EcsTableHasUnion) 
        {
            flecs_add_remove_union(world, src_table, 
                ECS_RECORD_TO_ROW(record->row), 1,
                &diff->added, &diff->removed);
        }

        return;
    }

    if (src_table) {
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (dst_table->type.count) { 
            move_entity(world, entity, record, dst_table, diff, 
                construct, notify_on_set);
        } else {
            delete_entity(world, record, diff);
            record->table = NULL;
        }      
    } else {        
        if (dst_table->type.count) {
            record = new_entity(world, entity, record, dst_table, diff, 
                construct, notify_on_set);
        }        
    }

    /* If the entity is being watched, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (record->row & ECS_ROW_FLAGS_MASK) {
        update_component_monitors(world, &diff->added, &diff->removed);
    }

    if ((!src_table || !src_table->type.count) && world->range_check_enabled) {
        ecs_check(!world->info.max_id || entity <= world->info.max_id, 
            ECS_OUT_OF_RANGE, 0);
        ecs_check(entity >= world->info.min_id, 
            ECS_OUT_OF_RANGE, 0);
    } 
error:
    return;
}

static
void new(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t *to_add)
{
    int32_t i, count = to_add->count;
    ecs_table_t *table = &world->store.root;
    
    ecs_diff_buffer_t diff = ECS_DIFF_INIT;
    for (i = 0; i < count; i ++) {
        table = table_append(world, table, to_add->array[i], &diff);
    }

    ecs_table_diff_t table_diff = diff_to_table_diff(&diff);
    new_entity(world, entity, NULL, table, &table_diff, true, true);
    diff_free(&diff);
}

static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_entity_t *entities,
    ecs_type_t *component_ids,
    int32_t count,
    void **component_data,
    bool is_move,
    int32_t *row_out,
    ecs_table_diff_t *diff)
{
    int32_t sparse_count = 0;
    if (!entities) {
        sparse_count = flecs_entities_count(world);
        entities = flecs_sparse_new_ids(ecs_eis(world), count);
    }

    if (!table) {
        return entities;
    }

    ecs_type_t type = table->type;   
    if (!type.count) {
        return entities;        
    }

    ecs_type_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = type.array;
        component_array.count = type.count;
    }

    ecs_data_t *data = &table->data;
    int32_t row = flecs_table_appendn(world, table, data, count, entities);
    
    /* Update entity index. */
    int i;
    ecs_record_t **records = ecs_storage_first(&data->records);
    for (i = 0; i < count; i ++) { 
        records[row + i] = flecs_entities_set(world, entities[i], 
            &(ecs_record_t){
                .table = table,
                .row = ECS_ROW_TO_RECORD(row + i, 0)
            });
    }

    flecs_defer_begin(world, &world->stages[0]);

    flecs_notify_on_add(world, table, NULL, row, count, diff, 
        component_data == NULL);

    if (component_data) {
        /* Set components that we're setting in the component mask so the init
         * actions won't call OnSet triggers for them. This ensures we won't
         * call OnSet triggers multiple times for the same component */
        int32_t c_i;
        ecs_table_t *storage_table = table->storage_table;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            /* Find component in storage type */
            ecs_entity_t id = component_ids->array[c_i];
            const ecs_table_record_t *tr = flecs_table_record_get(
                world, storage_table, id);
            ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);

            int32_t index = tr->column;
            ecs_type_info_t *ti = table->type_info[index];
            ecs_column_t *column = &table->data.columns[index];
            int32_t size = ti->size;
            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
            void *ptr = ecs_storage_get(column, size, row);

            ecs_copy_t copy;
            ecs_move_t move;
            if (is_move && (move = ti->hooks.move)) {
                move(ptr, src_ptr, count, ti);
            } else if (!is_move && (copy = ti->hooks.copy)) {
                copy(ptr, src_ptr, count, ti);
            } else {
                ecs_os_memcpy(ptr, src_ptr, size * count);
            } 
        };

        flecs_notify_on_set(world, table, row, count, NULL, true);
        flecs_notify_on_set(world, table, row, count, &diff->on_set, false);
    }

    flecs_defer_end(world, &world->stages[0]);

    if (row_out) {
        *row_out = row;
    }

    if (sparse_count) {
        entities = flecs_sparse_ids(ecs_eis(world));
        return &entities[sparse_count];
    } else {
        return entities;
    }
}

static
void add_id_w_record(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_id_t id,
    bool construct)
{
    ecs_table_diff_t diff;

    ecs_table_t *src_table = NULL;
    if (record) {
        src_table = record->table;
    }

    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &id, &diff);

    flecs_commit(world, entity, record, dst_table, &diff, construct, 
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

    ecs_record_t *r = flecs_entities_ensure(world, entity);
    ecs_table_diff_t diff;
    ecs_table_t *src_table = r->table;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &id, &diff);

    flecs_commit(world, entity, r, dst_table, &diff, true, true);

    flecs_defer_end(world, stage);
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

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *src_table = NULL;
    if (!r || !(src_table = r->table)) {
        goto done; /* Nothing to remove */
    }

    ecs_table_diff_t diff;
    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, &id, &diff);

    flecs_commit(world, entity, r, dst_table, &diff, true, true);

done:
    flecs_defer_end(world, stage);
}

static
void *get_mutable(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t id,
    ecs_record_t *r)
{
    void *dst = NULL;

    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check((id & ECS_COMPONENT_MASK) == id || 
        ECS_HAS_ROLE(id, PAIR), ECS_INVALID_PARAMETER, NULL);

    if (r->table) {
        dst = get_component(world, r->table, ECS_RECORD_TO_ROW(r->row), id);
    }

    if (!dst) {
        /* If entity didn't have component yet, add it */
        add_id_w_record(world, entity, r, id, true);

        /* Flush commands so the pointer we're fetching is stable */
        ecs_defer_end(world);
        ecs_defer_begin(world);

        ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(r->table->storage_table != NULL, ECS_INTERNAL_ERROR, NULL);
        dst = get_component(world, r->table, ECS_RECORD_TO_ROW(r->row), id);

        return dst;
    }

error:
    return dst;
}

/* -- Private functions -- */
static
void flecs_notify_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    ecs_table_diff_t *diff,
    bool run_on_set)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);

    if (diff->added.count) {
        if (table->flags & EcsTableHasIsA) {
            components_override(world, table, other_table, row, count, 
                &diff->added, run_on_set);
        }

        if (table->flags & EcsTableHasUnion) {
            flecs_add_remove_union(world, table, row, count, &diff->added, NULL);
        }

        if (table->flags & EcsTableHasOnAdd) {
            flecs_notify(world, table, other_table, row, count, EcsOnAdd, 
                &diff->added, 0);
        }
    }

    /* When a IsA relation is added to an entity, that entity inherits the
     * components from the base. Send OnSet notifications so that an application
     * can respond to these new components. */
    if (run_on_set && diff->on_set.count) {
        flecs_notify(world, table, other_table, row, count, EcsOnSet, &diff->on_set, 
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
            flecs_notify(world, table, other_table, row, count, EcsUnSet, &diff->un_set, 0);
        }

        if (table->flags & EcsTableHasOnRemove && diff->removed.count) {
            flecs_notify(world, table, other_table, row, count, EcsOnRemove, 
                &diff->removed, 0);
        }

        if (table->flags & EcsTableHasIsA && diff->on_set.count) {
            flecs_notify(world, table, other_table, row, count, EcsOnSet, 
                &diff->on_set, 0);
        }
    }
}

void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_type_t *ids,
    bool owned)
{
    ecs_data_t *data = &table->data;

    ecs_entity_t *entities = ecs_storage_get_t(
        &data->entities, ecs_entity_t, row);
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_storage_count(&data->entities), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_type_t local_ids;
    if (!ids) {
        local_ids.array = table->storage_ids;
        local_ids.count = table->storage_count;
        ids = &local_ids;
    }

    if (owned) {
        ecs_table_t *storage_table = table->storage_table;
        int i;
        for (i = 0; i < ids->count; i ++) {
            ecs_id_t id = ids->array[i];
            const ecs_table_record_t *tr = flecs_table_record_get(world, 
                storage_table, id);
            ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);
            int32_t column = tr->column;
            const ecs_type_info_t *ti = table->type_info[column];
            ecs_iter_action_t on_set = ti->hooks.on_set;
            if (on_set) {
                ecs_column_t *c = &table->data.columns[column];
                ecs_size_t size = ti->size;
                void *ptr = ecs_storage_get(c, size, row);
                ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

                ecs_iter_t it = {.term_count = 1};
                it.entities = entities;
                
                flecs_iter_init(&it, flecs_iter_cache_all);
                it.world = world;
                it.real_world = world;
                it.table = table;
                it.ptrs[0] = ptr;
                it.sizes[0] = size;
                it.ids[0] = id;
                it.event = EcsOnSet;
                it.event_id = id;
                it.ctx = ti->hooks.ctx;
                it.binding_ctx = ti->hooks.binding_ctx;
                it.count = count;
                flecs_iter_validate(&it);
                on_set(&it);
            }
        }
    }

    /* Run OnSet notifications */
    if (table->flags & EcsTableHasOnSet && ids->count) {
        flecs_notify(world, table, NULL, row, count, EcsOnSet, ids, 0);
    }
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
    return row | (EcsEntityObserved * is_watched);
}

void flecs_add_flag(
    ecs_world_t *world,
    ecs_entity_t entity,
    uint32_t flag)
{
    ecs_record_t *record = flecs_entities_get(world, entity);
    if (!record) {
        ecs_record_t new_record = {.row = flag, .table = NULL};
        flecs_entities_set(world, entity, &new_record);
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
    const ecs_type_t *added,
    const ecs_type_t *removed)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *src_table = NULL;
    if (!record) {
        record = flecs_entities_get(world, entity);
        src_table = record->table;
    }

    ecs_table_diff_t diff;
    ecs_os_zeromem(&diff);

    if (added) {
        diff.added = *added;
    }
    if (removed) {
        diff.added = *removed;
    }
    
    flecs_commit(world, entity, record, table, &diff, true, true);

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
        ecs_assert(unsafe_world->info.last_id < UINT_MAX, 
            ECS_INVALID_OPERATION, NULL);
        entity = (ecs_entity_t)ecs_os_ainc(
            (int32_t*)&unsafe_world->info.last_id);
    } else {
        entity = flecs_entities_recycle(unsafe_world);
    }

    ecs_assert(!unsafe_world->info.max_id || 
        ecs_entity_t_lo(entity) <= unsafe_world->info.max_id, 
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
    if (unsafe_world->flags & EcsWorldReadonly) {
        /* Can't issue new comp id while iterating when in multithreaded mode */
        ecs_check(ecs_get_stage_count(world) <= 1,
            ECS_INVALID_WHILE_READONLY, NULL);
    }

    ecs_entity_t id = 0;
    if (unsafe_world->info.last_component_id < ECS_HI_COMPONENT_ID) {
        do {
            id = unsafe_world->info.last_component_id ++;
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
    ecs_check(!id || ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    ecs_id_t ids[3];
    ecs_type_t to_add = { .array = ids, .count = 0 };

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

        flecs_entities_set(world, entity, &(ecs_record_t){ 0 });
    }
    flecs_defer_end(world, stage);

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
    ecs_diff_buffer_t *diff,
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

            if (term.oper == EcsAnd || !replace_and) {
                /* Regular AND expression */
                table = table_append(world, table, term.id, diff);
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
    const char *root_sep = desc->root_sep;

    /* Find existing table */
    ecs_table_t *src_table = NULL, *table = NULL;
    ecs_record_t *r = NULL;
    if (!new_entity) {
        r = flecs_entities_get(world, result);
        if (r) {
            table = r->table;
        }
    }

    /* Find destination table */
    ecs_diff_buffer_t diff = ECS_DIFF_INIT;

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
    while ((i < ECS_ID_CACHE_SIZE) && (id = ids[i ++])) {
        bool should_add = true;
        if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_FIRST(id) == EcsChildOf) {
            scope = ECS_PAIR_SECOND(id);
            if ((!desc->entity && desc->name) || (name && !name_assigned)) {
                /* If name is added to entity, pass scope to add_path instead
                 * of adding it to the table. The provided name may have nested
                 * elements, in which case the parent provided here is not the
                 * parent the entity will end up with. */
                should_add = false;
            }
        }
        if (should_add) {
            table = table_append(world, table, id, &diff);
        }
    }

    /* Add components from the 'add_expr' expression */
    if (desc->add_expr && ecs_os_strcmp(desc->add_expr, "0")) {
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
        ecs_table_diff_t table_diff = diff_to_table_diff(&diff);
        flecs_commit(world, result, r, table, &table_diff, true, true);
        ecs_defer_end(world);
    }

    /* Set name */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, result, scope, name, sep, root_sep);
        ecs_assert(ecs_get_name(world, result) != NULL,
            ECS_INTERNAL_ERROR, NULL);
    }

    if (desc->symbol && desc->symbol[0]) {
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
    const char *root_sep = desc->root_sep;

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
    while ((i < ECS_ID_CACHE_SIZE) && (id = ids[i ++])) {
        bool defer = true;
        if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_FIRST(id) == EcsChildOf) {
            scope = ECS_PAIR_SECOND(id);
            if (!desc->entity || (name && !name_assigned)) {
                /* New named entities are created by temporarily going out of
                 * readonly mode to ensure no duplicates are created. */
                defer = false;
            }
        }
        if (defer) {
            ecs_add_id(world, entity, id);
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

    int32_t thread_count = ecs_get_stage_count(world);

    /* Set name */
    if (name && !name_assigned) {
        /* To prevent creating two entities with the same name, temporarily go
         * out of readonly mode if it's safe to do so. */
        ecs_suspend_readonly_state_t state;
        if (thread_count <= 1) {
            /* When not running on multiple threads we can temporarily leave
             * readonly mode which ensures that we don't accidentally create
             * two entities with the same name. */
            ecs_world_t *real_world = flecs_suspend_readonly(world, &state);
            ecs_add_path_w_sep(real_world, entity, scope, name, sep, root_sep);
            flecs_resume_readonly(real_world, &state);
        } else {
            /* In multithreaded mode we can't leave readonly mode, which means
             * there is a risk of creating two entities with the same name. 
             * Future improvements will be able to detect this. */
            ecs_add_path_w_sep(world, entity, scope, name, sep, root_sep);
        }
    }

    /* Set symbol */
    if (desc->symbol) {
        const char *sym = ecs_get_symbol(world, entity);
        if (!sym || ecs_os_strcmp(sym, desc->symbol)) {
            if (thread_count <= 1) { /* See above */
                ecs_suspend_readonly_state_t state;
                ecs_world_t *real_world = flecs_suspend_readonly(world, &state);
                ecs_set_symbol(world, entity, desc->symbol);
                flecs_resume_readonly(real_world, &state);
            } else {
                ecs_set_symbol(world, entity, desc->symbol);
            }
        }
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

    if (name && !name[0]) {
        name = NULL;
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
            /* If add array contains a ChildOf pair, use it as scope instead */
            const ecs_id_t *ids = desc->add;
            ecs_id_t id;
            int32_t i = 0;
            while ((i < ECS_ID_CACHE_SIZE) && (id = ids[i ++])) {
                if (ECS_HAS_ROLE(id, PAIR) && 
                    (ECS_PAIR_FIRST(id) == EcsChildOf))
                {
                    scope = ECS_PAIR_SECOND(id);
                }
            }

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
        sparse_count = flecs_entities_count(world);
        entities = flecs_sparse_new_ids(ecs_eis(world), count);
        ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    } else {
        int i;
        for (i = 0; i < count; i ++) {
            ecs_ensure(world, entities[i]);
        }
    }

    ecs_type_t ids;
    ecs_table_t *table = desc->table;
    ecs_diff_buffer_t diff = ECS_DIFF_INIT;
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
        diff.added.type.array = table->type.array;
        diff.added.type.count = table->type.count;
        ids = diff.added.type;
    }

    ecs_table_diff_t table_diff = diff_to_table_diff(&diff);
    new_w_data(world, table, entities, &ids, count, desc->data, true, NULL, 
        &table_diff);

    if (!sparse_count) {
        return entities;
    } else {
        /* Refetch entity ids, in case the underlying array was reallocated */
        entities = flecs_sparse_ids(ecs_eis(world));
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

    EcsComponent *ptr = ecs_get_mut(world, result, EcsComponent);
    if (!ptr->size) {
        ecs_assert(ptr->alignment == 0, ECS_INTERNAL_ERROR, NULL);
        ptr->size = desc->type.size;
        ptr->alignment = desc->type.alignment;
        if (!ptr->size) {
            ecs_trace("#[green]tag#[reset] %s created", 
                ecs_get_name(world, result));
        } else {
            ecs_trace("#[green]component#[reset] %s created", 
                ecs_get_name(world, result));
        }
    } else {
        if (ptr->size != desc->type.size) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, desc->entity.name);
        }
        if (ptr->alignment != desc->type.alignment) {
            ecs_abort(ECS_INVALID_COMPONENT_ALIGNMENT, desc->entity.name);
        }
    }

    ecs_modified(world, result, EcsComponent);

    if (desc->type.size && 
        !ecs_id_in_use(world, result) && 
        !ecs_id_in_use(world, ecs_pair(result, EcsWildcard)))
    {
        ecs_set_hooks_id(world, result, &desc->type.hooks);
    }

    if (e >= world->info.last_component_id && e < ECS_HI_COMPONENT_ID) {
        world->info.last_component_id = e + 1;
    }

    /* Ensure components cannot be deleted */
    ecs_add_pair(world, result, EcsOnDelete, EcsPanic);

    flecs_resume_readonly(world, &readonly_state);
    
    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_has(world, result, EcsComponent), ECS_INTERNAL_ERROR, NULL);

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
    ecs_diff_buffer_t diff = ECS_DIFF_INIT;
    
    if (id) {
        table = table_append(world, table, id, &diff);
    }

    ecs_table_diff_t td = diff_to_table_diff(&diff);
    ids = new_w_data(world, table, NULL, NULL, count, NULL, false, NULL, &td);
    flecs_defer_end(world, stage);

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

    ecs_record_t *r = flecs_entities_get(world, entity);
    if (!r) {
        return; /* Nothing to clear */
    }

    ecs_table_t *table = r->table;
    if (table) {
        ecs_table_diff_t diff = {
            .removed = table->type,
            .un_set = { table->storage_ids, table->storage_count }
        };

        delete_entity(world, r, &diff);
        r->table = NULL;
    }    

    flecs_defer_end(world, stage);
error:
    return;
}

static
void flecs_throw_invalid_delete(
    ecs_world_t *world,
    ecs_id_t id)
{
    char *id_str = NULL;
    if (!(world->flags & EcsWorldQuit)) {
        id_str = ecs_id_str(world, id);
        ecs_throw(ECS_CONSTRAINT_VIOLATED, id_str);
    }
error:
    ecs_os_free(id_str);
}

static
void flecs_marked_id_push(
    ecs_world_t *world,
    ecs_id_record_t* idr,
    ecs_entity_t action)
{
    ecs_marked_id_t *m = ecs_vector_add(&world->store.marked_ids, 
        ecs_marked_id_t);

    m->idr = idr;
    m->id = idr->id;
    m->action = action;

    flecs_id_record_claim(world, idr);
}

static
void flecs_id_mark_for_delete(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_entity_t action);

static
void flecs_targets_mark_for_delete(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_id_record_t *idr;
    ecs_entity_t *entities = ecs_storage_first(&table->data.entities);
    ecs_record_t **records = ecs_storage_first(&table->data.records);
    int32_t i, count = ecs_storage_count(&table->data.entities);
    for (i = 0; i < count; i ++) {
        ecs_record_t *r = records[i];
        if (!r) {
            continue;
        }

        /* If entity is not used as id or as relationship target, there won't
         * be any tables with a reference to it. */
        ecs_flags32_t flags = r->row & ECS_ROW_FLAGS_MASK;
        if (!(flags & (EcsEntityObservedId|EcsEntityObservedObject))) {
            continue;
        }

        ecs_entity_t e = entities[i];
        if (flags & EcsEntityObservedId) {
            if ((idr = flecs_id_record_get(world, e))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE(idr->flags));
            }
            if ((idr = flecs_id_record_get(world, ecs_pair(e, EcsWildcard)))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE(idr->flags));
            }
        }
        if (flags & EcsEntityObservedObject) {
            if ((idr = flecs_id_record_get(world, ecs_pair(EcsWildcard, e)))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE_OBJECT(idr->flags));
            }
        }
    }
}

static
bool flecs_id_is_delete_target(
    ecs_id_t id,
    ecs_entity_t action)
{
    if (!action && ecs_id_is_pair(id) && ECS_PAIR_FIRST(id) == EcsWildcard) {
        /* If no explicit delete action is provided, and the id we're deleting
         * has the form (*, Target), use OnDeleteObject action */
        return true;
    }
    return false;
}

static
ecs_entity_t flecs_get_delete_action(
    ecs_table_t *table,
    ecs_table_record_t *tr,
    ecs_entity_t action,
    bool delete_target)
{
    ecs_entity_t result = action;
    if (!result && delete_target) {
        /* If action is not specified and we're deleting a relation target,
         * derive the action from the current record */
        ecs_table_record_t *trr = &table->records[tr->column];
        ecs_id_record_t *idrr = (ecs_id_record_t*)trr->hdr.cache;
        result = ECS_ID_ON_DELETE_OBJECT(idrr->flags);
    }
    return result;
}

static
void flecs_update_monitors_for_delete(
    ecs_world_t *world,
    ecs_id_t id)
{
    update_component_monitors(world, NULL, &(ecs_type_t){
        .array = (ecs_id_t[]){id},
        .count = 1
    });
}

static
void flecs_id_mark_for_delete(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_entity_t action)
{
    if (idr->flags & EcsIdMarkedForDelete) {
        return;
    }

    idr->flags |= EcsIdMarkedForDelete;
    flecs_marked_id_push(world, idr, action);

    ecs_id_t id = idr->id;

    bool delete_target = flecs_id_is_delete_target(id, action);

    /* Mark all tables with the id for delete */
    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&idr->cache, &it)) {
        ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            if (table->flags & EcsTableMarkedForDelete) {
                continue;
            }

            ecs_entity_t cur_action = flecs_get_delete_action(table, tr, action,
                delete_target);

            /* If this is a Delete action, recursively mark ids & tables */
            if (cur_action == EcsDelete) {
                table->flags |= EcsTableMarkedForDelete;
                ecs_log_push_2();
                flecs_targets_mark_for_delete(world, table);
                ecs_log_pop_2();
            } else if (cur_action == EcsPanic) {
                flecs_throw_invalid_delete(world, id);
            }
        }
    }

    /* Same for empty tables */
    if (flecs_table_cache_empty_iter(&idr->cache, &it)) {
        ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            tr->hdr.table->flags |= EcsTableMarkedForDelete;
        }
    }

    /* Signal query cache monitors */
    flecs_update_monitors_for_delete(world, id);

    /* If id is a wildcard pair, update cache monitors for non-wildcard ids */
    if (ecs_id_is_wildcard(id)) {
        ecs_assert(ECS_HAS_ROLE(id, PAIR), ECS_INTERNAL_ERROR, NULL);
        ecs_id_record_t *cur = idr;
        if (ECS_PAIR_SECOND(id) == EcsWildcard) {
            while ((cur = cur->first.next)) {
                flecs_update_monitors_for_delete(world, cur->id);
            }
        } else {
            ecs_assert(ECS_PAIR_FIRST(id) == EcsWildcard, 
                ECS_INTERNAL_ERROR, NULL);
            while ((cur = cur->second.next)) {
                flecs_update_monitors_for_delete(world, cur->id);
            }
        }
    }
}

static
bool flecs_on_delete_mark(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        /* If there's no id record, there's nothing to delete */
        return false;
    }

    if (!action) {
        /* If no explicit action is provided, derive it */
        if (!ecs_id_is_pair(id) || ECS_PAIR_SECOND(id) == EcsWildcard) {
            /* Delete actions are determined by the component, or in the case
             * of a pair by the relationship. */
            action = ECS_ID_ON_DELETE(idr->flags);
        }
    }

    if (action == EcsPanic) {
        /* This id is protected from deletion */
        flecs_throw_invalid_delete(world, id);
        return false;
    }

    flecs_id_mark_for_delete(world, idr, action);

    return true;
}

static
void flecs_remove_from_table(
    ecs_world_t *world, 
    ecs_table_t *table) 
{
    ecs_table_diff_t temp_diff;
    ecs_diff_buffer_t diff = ECS_DIFF_INIT;
    ecs_table_t *dst_table = table; 

    /* To find the dst table, remove all ids that are marked for deletion */
    int32_t i, t, count = ecs_vector_count(world->store.marked_ids);
    ecs_marked_id_t *ids = ecs_vector_first(world->store.marked_ids,
        ecs_marked_id_t);
    const ecs_table_record_t *tr;
    for (i = 0; i < count; i ++) {
        const ecs_id_record_t *idr = ids[i].idr;

        if (!(tr = flecs_id_record_get_table(idr, dst_table))) {
            continue;
        }

        t = tr->column;

        do {
            ecs_id_t id = dst_table->type.array[t];
            dst_table = flecs_table_traverse_remove(
                world, dst_table, &id, &temp_diff);
            diff_append(&diff, &temp_diff);
        } while (dst_table->type.count && (t = ecs_search_offset(
            world, dst_table, t, idr->id, NULL)) != -1);
    }

    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!dst_table->type.count) {
        /* If this removes all components, clear table */
        ecs_dbg_3("#[red]clear#[reset] entities from table %u", 
            (uint32_t)table->id);
        flecs_table_clear_entities(world, table);
    } else {
        /* Otherwise, merge table into dst_table */
        ecs_dbg_3("#[red]move#[reset] entities from table %u to %u", 
            (uint32_t)table->id, (uint32_t)dst_table->id);

        if (dst_table != table) {
            if (diff.removed.type.count) {
                ecs_log_push_3();
                ecs_table_diff_t td = diff_to_table_diff(&diff);
                flecs_notify_on_remove(world, table, NULL, 
                    0, ecs_table_count(table), &td);
                ecs_log_pop_3();
            }
            flecs_table_merge(world, dst_table, table, 
                &dst_table->data, &table->data);
        }
    }

    diff_free(&diff);
}

static
bool flecs_on_delete_clear_tables(
    ecs_world_t *world)
{
    int32_t i, last = ecs_vector_count(world->store.marked_ids), first = 0;
    ecs_marked_id_t *ids = ecs_vector_first(world->store.marked_ids,
        ecs_marked_id_t);

    /* Iterate in reverse order so that DAGs get deleted bottom to top */
    do {
        for (i = last - 1; i >= first; i --) {
            ecs_id_record_t *idr = ids[i].idr;
            ecs_entity_t action = ids[i].action;
 
            /* Empty all tables for id */
            ecs_table_cache_iter_t it;
            if (flecs_table_cache_iter(&idr->cache, &it)) {
                ecs_table_record_t *tr;
                while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
                    ecs_table_t *table = tr->hdr.table;

                    if ((action == EcsRemove) || 
                        !(table->flags & EcsTableMarkedForDelete))
                    {
                        flecs_remove_from_table(world, table);
                    } else {
                        ecs_dbg_3(
                            "#[red]delete#[reset] entities from table %u", 
                            (uint32_t)table->id);
                        flecs_table_delete_entities(world, table);
                    }
                }
            }

            /* Run commands so children get notified before parent is deleted */
            ecs_defer_end(world);
            ecs_defer_begin(world);

            /* User code (from triggers) could have enqueued more ids to delete,
             * reobtain the array in case it got reallocated */
            ids = ecs_vector_first(world->store.marked_ids, ecs_marked_id_t);
        }

        /* Check if new ids were marked since we started */
        int32_t new_last = ecs_vector_count(world->store.marked_ids);
        if (new_last != last) {
            /* Iterate remaining ids */
            ecs_assert(new_last > last, ECS_INTERNAL_ERROR, NULL);
            first = last;
            last = new_last;
        } else {
            break;
        }
    } while (true);

    return true;
}

static
bool flecs_on_delete_clear_ids(
    ecs_world_t *world)
{
    int32_t i, count = ecs_vector_count(world->store.marked_ids);
    ecs_marked_id_t *ids = ecs_vector_first(world->store.marked_ids,
        ecs_marked_id_t);

    for (i = 0; i < count; i ++) {
        ecs_id_record_t *idr = ids[i].idr;

        flecs_id_record_release_tables(world, idr);

        /* Release the claim taken by flecs_marked_id_push. This may delete the
         * id record as all other claims may have been released. */
        if (flecs_id_record_release(world, idr)) {
            /* If the id record is still alive, release the initial claim */
            flecs_id_record_release(world, idr);
        }
    }

    return true;
}

static
void flecs_on_delete(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action)
{
    /* Cleanup can happen recursively. If a cleanup action is already in 
     * progress, only append ids to the marked_ids. The topmost cleanup
     * frame will handle the actual cleanup. */
    int32_t count = ecs_vector_count(world->store.marked_ids);

    /* Make sure we're evaluating a consistent list of non-empty tables */
    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    /* Collect all ids that need to be deleted */
    flecs_on_delete_mark(world, id, action);

    /* Only perform cleanup if we're the first stack frame doing it */
    if (!count && ecs_vector_count(world->store.marked_ids)) {
        ecs_dbg_2("#[red]delete#[reset]");
        ecs_log_push_2();

        /* Empty tables with all the to be deleted ids */
        flecs_on_delete_clear_tables(world);

        /* All marked tables are empty, ensure they're in the right list */
        ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

        /* Release remaining references to the ids */
        flecs_on_delete_clear_ids(world);

        /* Verify deleted ids are no longer in use */
#ifdef FLECS_DEBUG
        ecs_marked_id_t *ids = ecs_vector_first(world->store.marked_ids,
            ecs_marked_id_t);
        int32_t i;
        count = ecs_vector_count(world->store.marked_ids);
        for (i = 0; i < count; i ++) {
            ecs_assert(!ecs_id_in_use(world, ids[i].id), 
                ECS_INTERNAL_ERROR, NULL);
        }
#endif
        ecs_assert(!ecs_id_in_use(world, id), ECS_INTERNAL_ERROR, NULL);

        /* Ids are deleted, clear stack */
        ecs_vector_clear(world->store.marked_ids);

        ecs_log_pop_2();
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

    flecs_on_delete(world, id, EcsDelete);
    flecs_defer_end(world, stage);
}

void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(world, stage, id, EcsRemove)) {
        return;
    }

    flecs_on_delete(world, id, EcsRemove);
    flecs_defer_end(world, stage);
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

    ecs_record_t *r = flecs_entities_get(world, entity);
    if (r) {
        ecs_flags32_t row_flags = ECS_RECORD_TO_ROW_FLAGS(r->row);
        if (row_flags) {
            if (row_flags & EcsEntityObservedId) {
                flecs_on_delete(world, entity, 0);
                flecs_on_delete(world, ecs_pair(entity, EcsWildcard), 0);
            }
            if (row_flags & EcsEntityObservedObject) {
                flecs_on_delete(world, ecs_pair(EcsWildcard, entity), 0);
            }

            /* Merge operations before deleting entity */
            ecs_defer_end(world);
            ecs_defer_begin(world);
        }

        ecs_table_t *table = r->table;

        /* If entity has components, remove them. Check if table is still alive,
         * as delete actions could have deleted the table already. */
        if (table) {
            ecs_table_diff_t diff = {
                .removed = table->type,
                .un_set = { table->storage_ids, table->storage_count }
            };

            delete_entity(world, r, &diff);

            r->row = 0;
            r->table = NULL;
        }
        
        flecs_entities_remove(world, entity);
    }

    flecs_defer_end(world, stage);
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
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
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
    ecs_check(ecs_id_is_valid(world, id) || ecs_id_is_wildcard(id), 
        ECS_INVALID_PARAMETER, NULL);
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
    ecs_check(!dst || !ecs_get_table(world, dst), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (!dst) {
        dst = ecs_new_id(world);
    }

    if (flecs_defer_clone(world, stage, dst, src, copy_value)) {
        return dst;
    }

    ecs_record_t *src_r = flecs_entities_get(world, src);
    ecs_table_t *src_table;
    if (!src_r || !(src_table = src_r->table)) {
        goto done;
    }

    ecs_type_t src_type = src_table->type;
    ecs_table_diff_t diff = { .added = src_type };
    ecs_record_t *dst_r = new_entity(world, dst, NULL, src_table, &diff, true, true);
    int32_t row = ECS_RECORD_TO_ROW(dst_r->row);

    if (copy_value) {
        flecs_table_move(world, dst, src, src_table,
            row, src_table, ECS_RECORD_TO_ROW(src_r->row), true);

        flecs_notify_on_set(world, src_table, row, 1, NULL, true);
    }

done:
    flecs_defer_end(world, stage);
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

    ecs_record_t *r = flecs_entities_get(world, entity);
    if (!r) {
        return NULL;
    }

    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return NULL;
    }

    const ecs_table_record_t *tr = NULL;
    ecs_table_t *storage_table = table->storage_table;
    if (storage_table) {
        tr = flecs_id_record_get_table(idr, storage_table);
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

void* ecs_get_mut_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    void *result;

    if (flecs_defer_set(
        world, stage, EcsOpMut, entity, id, 0, NULL, &result))
    {
        return result;
    }

    ecs_record_t *r = flecs_entities_ensure(world, entity);
    result = get_mutable(world, entity, id, r);
    ecs_check(result != NULL, ECS_INVALID_PARAMETER, NULL);
    
    flecs_defer_end(world, stage);

    return result;
error:
    return NULL;
}

static
ecs_record_t* flecs_access_begin(
    ecs_world_t *stage,
    ecs_entity_t entity,
    bool write)
{
    ecs_check(ecs_os_has_threading(), ECS_MISSING_OS_API, NULL);

    const ecs_world_t *world = ecs_get_world(stage);
    ecs_record_t *r = flecs_entities_get(world, entity);
    if (!r) {
        return NULL;
    }

    ecs_table_t *table;
    if (!(table = r->table)) {
        return NULL;
    }

    int32_t count = ecs_os_ainc(&table->lock);
    (void)count;
    if (write) {
        ecs_check(count == 1, ECS_ACCESS_VIOLATION, NULL);
    }

    return r;
error:
    return NULL;
}

static
void flecs_access_end(
    const ecs_record_t *r,
    bool write)
{
    ecs_check(ecs_os_has_threading(), ECS_MISSING_OS_API, NULL);
    ecs_check(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(r->table != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t count = ecs_os_adec(&r->table->lock);
    (void)count;
    if (write) {
        ecs_check(count == 0, ECS_ACCESS_VIOLATION, NULL);
    }
    ecs_check(count >= 0, ECS_ACCESS_VIOLATION, NULL);

error:
    return;
}

ecs_record_t* ecs_write_begin(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    return flecs_access_begin(world, entity, true);
}

void ecs_write_end(
    ecs_record_t *r)
{
    flecs_access_end(r, true);
}

const ecs_record_t* ecs_read_begin(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    return flecs_access_begin(world, entity, false);
}

void ecs_read_end(
    const ecs_record_t *r)
{
    flecs_access_end(r, false);
}

const void* ecs_record_get_id(
    ecs_world_t *stage,
    const ecs_record_t *r,
    ecs_id_t id)
{
    const ecs_world_t *world = ecs_get_world(stage);
    return get_component(world, r->table, ECS_RECORD_TO_ROW(r->row), id);
}

void* ecs_record_get_mut_id(
    ecs_world_t *stage,
    ecs_record_t *r,
    ecs_id_t id)
{
    const ecs_world_t *world = ecs_get_world(stage);
    return get_component(world, r->table, ECS_RECORD_TO_ROW(r->row), id);
}

ecs_ref_t ecs_ref_init_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    
    world = ecs_get_world(world);

    ecs_record_t *record = flecs_entities_get(world, entity);
    ecs_check(record != NULL, ECS_INVALID_PARAMETER,
        "cannot create ref for empty entity");

    ecs_ref_t result = {
        .entity = entity,
        .id = id,
        .record = record
    };

    ecs_table_t *table = record->table;
    if (table) {
        result.tr = flecs_table_record_get(world, table->storage_table, id);
    }

    return result;
error:
    return (ecs_ref_t){0};
}

void* ecs_ref_get_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->record != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id == ref->id, ECS_INVALID_PARAMETER, NULL);

    ecs_record_t *r = ref->record;
    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    int32_t row = ECS_RECORD_TO_ROW(r->row);
    ecs_check(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);

    ecs_table_record_t *tr = ref->tr;
    if (!tr || tr->hdr.table != table) {
        tr = ref->tr = flecs_table_record_get(world, table->storage_table, id);
        if (!tr) {
            return NULL;
        }
    }

    return get_component_w_index(table, tr->column, row);
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
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_check(!ecs_has_id(world, entity, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    void *result;

    if (flecs_defer_set(world, stage, EcsOpMut, entity, id, 0, NULL, &result)) {
        return result;
    }

    ecs_record_t *r = flecs_entities_ensure(world, entity);
    add_id_w_record(world, entity, r, id, false /* Add without ctor */);

    void *ptr = get_component(world, r->table, ECS_RECORD_TO_ROW(r->row), id);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_defer_end(world, stage);

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
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_modified(world, stage, entity, id)) {
        return;
    }

    /* If the entity does not have the component, calling ecs_modified is 
     * invalid. The assert needs to happen after the defer statement, as the
     * entity may not have the component when this function is called while
     * operations are being deferred. */
    ecs_check(ecs_has_id(world, entity, id), ECS_INVALID_PARAMETER, NULL);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = r->table;
    ecs_type_t ids = { .array = &id, .count = 1 };
    flecs_notify_on_set(world, table, ECS_RECORD_TO_ROW(r->row), 1, &ids, true);

    flecs_table_mark_dirty(world, table, id);
    flecs_defer_end(world, stage);
error:
    return;
}

static
ecs_entity_t set_ptr_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    void *ptr,
    bool is_move,
    bool flecs_notify)
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
        flecs_utosize(size), ptr, NULL))
    {
        return entity;
    }

    ecs_record_t *r = flecs_entities_ensure(world, entity);
    void *dst = get_mutable(world, entity, id, r);
    ecs_check(dst != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_entity_t real_id = ecs_get_typeid(world, id);
        const ecs_type_info_t *ti = get_c_info(world, real_id);
        if (ti) {
            if (is_move) {
                ecs_move_t move = ti->hooks.move;
                if (move) {
                    move(dst, ptr, 1, ti);
                } else {
                    ecs_os_memcpy(dst, ptr, flecs_utosize(size));
                }
            } else {
                ecs_copy_t copy = ti->hooks.copy;
                if (copy) {
                    copy(dst, ptr, 1, ti);
                } else {
                    ecs_os_memcpy(dst, ptr, flecs_utosize(size));
                }
            }
        } else {
            ecs_os_memcpy(dst, ptr, flecs_utosize(size));
        }
    } else {
        ecs_os_memset(dst, 0, size);
    }

    flecs_table_mark_dirty(world, r->table, id);

    if (flecs_notify) {
        ecs_type_t ids = { .array = &id, .count = 1 };
        flecs_notify_on_set(
            world, r->table, ECS_RECORD_TO_ROW(r->row), 1, &ids, true);
    }

    flecs_defer_end(world, stage);

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
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Safe to cast away const: function won't modify if move arg is false */
    return set_ptr_w_id(
        world, entity, id, size, (void*)ptr, false, true);
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
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_enable(
        world, stage, entity, id, enable))
    {
        return;
    } else {
        /* Operations invoked by enable/disable should not be deferred */
        stage->defer --;
    }

    ecs_record_t *r = flecs_entities_ensure(world, entity);
    ecs_entity_t bs_id = (id & ECS_COMPONENT_MASK) | ECS_DISABLED;
    
    ecs_table_t *table = r->table;
    int32_t index = -1;
    if (table) {
        index = ecs_search(world, table, bs_id, 0);
    }

    if (index == -1) {
        ecs_add_id(world, entity, bs_id);
        ecs_enable_component_w_id(world, entity, id, enable);
        return;
    }

    index -= table->bs_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_bitset_t *bs = &table->data.bs_columns[index];
    ecs_assert(bs != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_bitset_set(bs, ECS_RECORD_TO_ROW(r->row), enable);
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
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table;
    if (!r || !(table = r->table)) {
        return false;
    }

    ecs_entity_t bs_id = (id & ECS_COMPONENT_MASK) | ECS_DISABLED;
    int32_t index = ecs_search(world, table, bs_id, 0);
    if (index == -1) {
        /* If table does not have DISABLED column for component, component is
         * always enabled, if the entity has it */
        return ecs_has_id(world, entity, id);
    }

    index -= table->bs_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &table->data.bs_columns[index];

    return flecs_bitset_get(bs, ECS_RECORD_TO_ROW(r->row));
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

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table;
    if (!r || !(table = r->table)) {
        return false;
    }

    ecs_table_record_t *tr = NULL;
    int32_t column = ecs_search_relation(
        world, table, 0, id, EcsIsA, 0, 0, 0, 0, 0, &tr);
    if (column == -1) {
        return false;
    }

    table = tr->hdr.table;
    if ((table->flags & EcsTableHasUnion) && ECS_HAS_ROLE(id, PAIR) &&
        ECS_PAIR_SECOND(id) != EcsWildcard) 
    {
        if (ECS_PAIR_FIRST(table->type.array[column]) == EcsUnion) {
            ecs_switch_t *sw = &table->data.sw_columns[
                column - table->sw_offset];
            int32_t row = ECS_RECORD_TO_ROW(r->row);
            uint64_t value = flecs_switch_get(sw, row);
            return value == ECS_PAIR_SECOND(id);
        }
    }
    
    return true;
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

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table;
    if (!r || !(table = r->table)) {
        return 0;
    }

    ecs_id_t wc = ecs_pair(rel, EcsWildcard);
    ecs_table_record_t *tr = flecs_table_record_get(world, table, wc);
    if (!tr) {
        if (table->flags & EcsTableHasUnion) {
            wc = ecs_pair(EcsUnion, rel);
            tr = flecs_table_record_get(world, table, wc);
            if (tr) {
                ecs_switch_t *sw = &table->data.sw_columns[
                    tr->column - table->sw_offset];
                int32_t row = ECS_RECORD_TO_ROW(r->row);
                return flecs_switch_get(sw, row);
                
            }
        }
        return 0;
    }

    if (index >= tr->count) {
        return 0;
    }

    ecs_id_t *ids = table->type.array;
    return ecs_pair_second(world, ids[tr->column + index]);
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
            world, table, 0, id, rel, 0, 0, &subject, 0, 0, 0);
        if (column == -1) {
            return 0;
        }
    } else {
        ecs_id_t *ids = table->type.array;
        int32_t i, count = table->type.count;

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

    EcsIdentifier *ptr = ecs_get_mut_pair(world, entity, EcsIdentifier, tag);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_os_strset(&ptr->value, name);
    ecs_modified_pair(world, entity, ecs_id(EcsIdentifier), tag);
    
    return entity;
error:
    return 0;
}


ecs_entity_t ecs_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    if (!entity) {
        return ecs_entity_init(world, &(ecs_entity_desc_t){
            .name = name
        });
    }
    return set_identifier(world, entity, EcsName, name);
}

ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    return set_identifier(world, entity, EcsSymbol, name);
}

void ecs_set_alias(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    set_identifier(world, entity, EcsAlias, name);
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
    
    /* Entity identifiers should not contain flag bits */
    if (entity & ECS_ROLE_MASK) {
        return false;
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

    return flecs_entities_is_alive(world, entity);
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

    ecs_entity_t current = flecs_entities_get_current(world, entity);
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
    flecs_entities_set_generation(world, entity);

    /* Ensure id exists. The underlying datastructure will verify that the
     * generation count matches the provided one. */
    flecs_entities_ensure(world, entity);
error:
    return;
}

void ecs_ensure_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t r = ECS_PAIR_FIRST(id);
        ecs_entity_t o = ECS_PAIR_SECOND(id);

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

    return flecs_entities_exists(world, entity);
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

    ecs_record_t *record = flecs_entities_get(world, entity);
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

const ecs_type_t* ecs_get_type(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
        return &table->type;
    }

    return NULL;
}

const ecs_type_info_t* ecs_get_type_info(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr && ECS_HAS_ROLE(id, PAIR)) {
        idr = flecs_id_record_get(world, 
            ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard));
        if (!idr || !idr->type_info) {
            idr = NULL;
        }
        if (!idr) {
            idr = flecs_id_record_get(world, 
                ecs_pair(EcsWildcard, ECS_PAIR_SECOND(id)));
            if (!idr || !idr->type_info) {
                idr = NULL;
            }
        }
    }

    if (idr) {
        return idr->type_info;
    } else if (!(id & ECS_ROLE_MASK)) {
        return flecs_sparse_get(world->type_info, ecs_type_info_t, id);
    }
error:
    return NULL;
}

ecs_entity_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    if (ti) {
        return ti->component;
    }
error:
    return 0;
}

ecs_entity_t ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (ecs_id_is_wildcard(id)) {
        /* If id is a wildcard, we can't tell if it's a tag or not, except
         * when the relation part of a pair has the Tag property */
        if (ECS_HAS_ROLE(id, PAIR)) {
            if (ECS_PAIR_FIRST(id) != EcsWildcard) {
                ecs_entity_t rel = ecs_pair_first(world, id);
                if (ecs_is_valid(world, rel)) {
                    if (ecs_has_id(world, rel, EcsTag)) {
                        return true;
                    }
                } else {
                    /* During bootstrap it's possible that not all ids are valid
                    * yet. Using ecs_get_typeid will ensure correct values are
                    * returned for only those components initialized during
                    * bootstrap, while still asserting if another invalid id
                    * is provided. */
                    if (ecs_get_typeid(world, id) == 0) {
                        return true;
                    }
                }
            } else {
                /* If relation is * id is not guaranteed to be a tag */
            }
        }
    } else {
        if (ecs_get_typeid(world, id) == 0) {
            return true;
        }
    }

    return false;
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
    if (ecs_has_id(world, entity, EcsPrefab)) {
        /* If entity is a type, enable/disable all entities in the type */
        const ecs_type_t *type = ecs_get_type(world, entity);
        ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_id_t *ids = type->array;
        int32_t i, count = type->count;
        for (i = 0; i < count; i ++) {
            ecs_id_t id = ids[i];
            if (ecs_id_get_flags(world, id) & EcsIdDontInherit) {
                continue;
            }
            ecs_enable(world, id, enabled);
        }
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
    return flecs_defer_begin(world, stage);
error:
    return false;
}

bool ecs_defer_end(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_end(world, stage);
error:
    return false;
}

void ecs_defer_suspend(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_deferred(world), ECS_INVALID_OPERATION, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(stage->defer_suspend == false, ECS_INVALID_OPERATION, NULL);
    stage->defer_suspend = true;
error:
    return;
}

void ecs_defer_resume(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_deferred(world), ECS_INVALID_OPERATION, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(stage->defer_suspend == true, ECS_INVALID_OPERATION, NULL);
    stage->defer_suspend = false;
error:
    return;
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
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        ecs_entity_t obj = ECS_PAIR_SECOND(id);

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
    const ecs_type_t *type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_entity_t *ids = type->array;
    int32_t i, count = type->count;

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

char* ecs_table_str(
    const ecs_world_t *world,
    const ecs_table_t *table)
{
    if (table) {
        return ecs_type_str(world, &table->type);
    } else {
        return NULL;
    }
}

static
void flecs_flush_bulk_new(
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
void flecs_dtor_value(
    ecs_world_t *world,
    ecs_id_t id,
    void *value,
    int32_t count)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        ecs_size_t size = ti->size;
        void *ptr;
        int i;
        for (i = 0, ptr = value; i < count; i ++, ptr = ECS_OFFSET(ptr, size)) {
            dtor(ptr, 1, ti);
        }
    }
}

static
void flecs_discard_op(
    ecs_world_t *world,
    ecs_defer_op_t *op)
{
    if (op->kind != EcsOpBulkNew) {
        void *value = op->is._1.value;
        if (value) {
            flecs_dtor_value(world, op->id, value, 1);
            flecs_stack_free(value, op->is._1.size);
        }
    } else {
        ecs_os_free(op->is._n.entities);
    }
}

static 
bool flecs_is_entity_valid(
    ecs_world_t *world,
    ecs_entity_t e)
{
    if (ecs_exists(world, e) && !ecs_is_alive(world, e)) {
        return false;
    }
    return true;
}

static
bool flecs_remove_invalid(
    ecs_world_t *world,
    ecs_id_t *id_out)
{
    ecs_id_t id = *id_out;

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t rel = ecs_pair_first(world, id);
        if (!rel || !flecs_is_entity_valid(world, rel)) {
            /* After relation is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        } else {
            ecs_entity_t obj = ecs_pair_second(world, id);
            if (!obj || !flecs_is_entity_valid(world, obj)) {
                /* Check the relation's policy for deleted objects */
                ecs_id_record_t *idr = flecs_id_record_get(world, 
                    ecs_pair(rel, EcsWildcard));
                if (idr) {
                    ecs_entity_t action = ECS_ID_ON_DELETE_OBJECT(idr->flags);
                    if (action == EcsDelete) {
                        /* Entity should be deleted, don't bother checking
                         * other ids */
                        return false;
                    } else if (action == EcsPanic) {
                        /* If policy is throw this object should not have
                         * been deleted */
                        flecs_throw_invalid_delete(world, id);
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
        if (!flecs_is_entity_valid(world, id)) {
            /* After relation is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        }
    }

    return true;
}

/* Leave safe section. Run all deferred commands. */
bool flecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    if (stage->defer_suspend) {
        return false;
    }

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

            ecs_stack_t stack = stage->defer_stack;
            flecs_stack_init(&stage->defer_stack);

            for (i = 0; i < count; i ++) {
                ecs_defer_op_t *op = &ops[i];
                ecs_entity_t e = op->is._1.entity;
                if (op->kind == EcsOpBulkNew) {
                    e = 0;
                }

                /* If entity is no longer alive, this could be because the queue
                 * contained both a delete and a subsequent add/remove/set which
                 * should be ignored. */
                if (e && !ecs_is_alive(world, e) && flecs_entities_exists(world, e)) {
                    ecs_assert(op->kind != EcsOpNew && op->kind != EcsOpClone, 
                        ECS_INTERNAL_ERROR, NULL);
                    world->info.discard_count ++;
                    flecs_discard_op(world, op);
                    continue;
                }

                switch(op->kind) {
                case EcsOpNew:
                case EcsOpAdd:
                    ecs_assert(op->id != 0, ECS_INTERNAL_ERROR, NULL);
                    if (flecs_remove_invalid(world, &op->id)) {
                        if (op->id) {
                            world->info.add_count ++;
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
                    set_ptr_w_id(world, e, 
                        op->id, flecs_itosize(op->is._1.size), 
                        op->is._1.value, true, true);
                    break;
                case EcsOpMut:
                    set_ptr_w_id(world, e, 
                        op->id, flecs_itosize(op->is._1.size), 
                        op->is._1.value, true, false);
                    break;
                case EcsOpModified:
                    if (ecs_has_id(world, e, op->id)) {
                        ecs_modified_id(world, e, op->id);
                    }
                    break;
                case EcsOpDelete: {
                    ecs_delete(world, e);
                    break;
                }
                case EcsOpClear:
                    ecs_clear(world, e);
                    break;
                case EcsOpOnDeleteAction:
                    flecs_on_delete(world, op->id, e);
                    break;
                case EcsOpEnable:
                    ecs_enable_component_w_id(world, e, op->id, true);
                    break;
                case EcsOpDisable:
                    ecs_enable_component_w_id(world, e, op->id, false);
                    break;
                case EcsOpBulkNew:
                    flecs_flush_bulk_new(world, op);
                    continue;
                }

                if (op->is._1.value) {
                    flecs_stack_free(op->is._1.value, op->is._1.size);
                }
            }

            if (stage->defer_queue) {
                ecs_vector_free(stage->defer_queue);
            }

            /* Restore defer queue */
            ecs_vector_clear(defer_queue);
            stage->defer_queue = defer_queue;

            /* Restore stack */
            flecs_stack_fini(&stage->defer_stack);
            stage->defer_stack = stack;
            flecs_stack_reset(&stage->defer_stack);
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
                flecs_discard_op(world, &ops[i]);
            }

            if (stage->defer_queue) {
                ecs_vector_free(stage->defer_queue);
            }

            /* Restore defer queue */
            ecs_vector_clear(defer_queue);
            stage->defer_queue = defer_queue;
            flecs_stack_reset(&stage->defer_stack);
        }

        return true;
    }

error:
    return false;
}
