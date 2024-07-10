/**
 * @file entity.c
 * @brief Entity API.
 * 
 * This file contains the implementation for the entity API, which includes 
 * creating/deleting entities, adding/removing/setting components, instantiating
 * prefabs, and several other APIs for retrieving entity data.
 * 
 * The file also contains the implementation of the command buffer, which is 
 * located here so it can call functions private to the compilation unit.
 */

#include "private_api.h"
#include <ctype.h>

#ifdef FLECS_SCRIPT
#include "addons/script/script.h"
#endif

static
const ecs_entity_t* flecs_bulk_new(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_entity_t *entities,
    ecs_type_t *component_ids,
    int32_t count,
    void **c_info,
    bool move,
    int32_t *row_out,
    ecs_table_diff_t *diff);

typedef struct {
    const ecs_type_info_t *ti;
    void *ptr;
} flecs_component_ptr_t;

static
flecs_component_ptr_t flecs_table_get_component(
    ecs_table_t *table,
    int32_t column_index,
    int32_t row)
{
    ecs_check(column_index < table->column_count, ECS_NOT_A_COMPONENT, NULL);
    ecs_column_t *column = &table->data.columns[column_index];
    return (flecs_component_ptr_t){
        .ti = column->ti,
        .ptr = ecs_vec_get(&column->data, column->size, row)
    };
error:
    return (flecs_component_ptr_t){0};
}

static
flecs_component_ptr_t flecs_get_component_ptr(
    ecs_table_t *table,
    int32_t row,
    ecs_id_record_t *idr)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!idr) {
        return (flecs_component_ptr_t){0};
    }

    if (idr->flags & EcsIdIsSparse) {
        ecs_entity_t entity = flecs_table_entities_array(table)[row];
        return (flecs_component_ptr_t){
            .ti = idr->type_info,
            .ptr = flecs_sparse_get_any(idr->sparse, 0, entity)
        };
    }

    ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr || (tr->column == -1)) {
        return (flecs_component_ptr_t){0};
    }

    return flecs_table_get_component(table, tr->column, row);
}

static
void* flecs_get_component(
    ecs_table_t *table,
    int32_t row,
    ecs_id_record_t *idr)
{
    return flecs_get_component_ptr(table, row, idr).ptr;
}

void* flecs_get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_id_record_t *idr,
    int32_t recur_depth)
{
    ecs_check(recur_depth < ECS_MAX_RECURSION, ECS_INVALID_PARAMETER,
        "cycle detected in IsA relationship");

    /* Table (and thus entity) does not have component, look for base */
    if (!(table->flags & EcsTableHasIsA)) {
        return NULL;
    }

    if (!(idr->flags & EcsIdOnInstantiateInherit)) {
        return NULL;
    }

    /* Exclude Name */
    if (id == ecs_pair(ecs_id(EcsIdentifier), EcsName)) {
        return NULL;
    }

    /* Table should always be in the table index for (IsA, *), otherwise the
     * HasBase flag should not have been set */
    ecs_table_record_t *tr_isa = flecs_id_record_get_table(
        world->idr_isa_wildcard, table);
    ecs_check(tr_isa != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t i = tr_isa->index, end = tr_isa->count + tr_isa->index;
    void *ptr = NULL;

    do {
        ecs_id_t pair = ids[i ++];
        ecs_entity_t base = ecs_pair_second(world, pair);

        ecs_record_t *r = flecs_entities_get(world, base);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

        table = r->table;
        if (!table) {
            continue;
        }

        const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
        if (!tr) {
            ptr = flecs_get_base_component(world, table, id, idr, 
                recur_depth + 1);
        } else {
            if (idr->flags & EcsIdIsSparse) {
                return flecs_sparse_get_any(idr->sparse, 0, base);
            } else {
                int32_t row = ECS_RECORD_TO_ROW(r->row);
                return flecs_table_get_component(table, tr->column, row).ptr;
            }
        }
    } while (!ptr && (i < end));

    return ptr;
error:
    return NULL;
}

static
void flecs_instantiate_slot(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    ecs_entity_t slot_of,
    ecs_entity_t slot,
    ecs_entity_t child)
{
    if (base == slot_of) {
        /* Instance inherits from slot_of, add slot to instance */
        ecs_add_pair(world, instance, slot, child);
    } else {
        /* Slot is registered for other prefab, travel hierarchy
         * upwards to find instance that inherits from slot_of */
        ecs_entity_t parent = instance;
        int32_t depth = 0;
        do {
            if (ecs_has_pair(world, parent, EcsIsA, slot_of)) {
                const char *name = ecs_get_name(world, slot);
                if (name == NULL) {
                    char *slot_of_str = ecs_get_path(world, slot_of);
                    ecs_throw(ECS_INVALID_OPERATION, "prefab '%s' has unnamed "
                        "slot (slots must be named)", slot_of_str);
                    ecs_os_free(slot_of_str);
                    return;
                }

                /* The 'slot' variable is currently pointing to a child (or 
                 * grandchild) of the current base. Find the original slot by
                 * looking it up under the prefab it was registered. */
                if (depth == 0) {
                    /* If the current instance is an instance of slot_of, just
                     * lookup the slot by name, which is faster than having to
                     * create a relative path. */
                    slot = ecs_lookup_child(world, slot_of, name);
                } else {
                    /* If the slot is more than one level away from the slot_of
                     * parent, use a relative path to find the slot */
                    char *path = ecs_get_path_w_sep(world, parent, child, ".",
                        NULL);
                    slot = ecs_lookup_path_w_sep(world, slot_of, path, ".", 
                        NULL, false);
                    ecs_os_free(path);
                }

                if (slot == 0) {
                    char *slot_of_str = ecs_get_path(world, slot_of);
                    char *slot_str = ecs_get_path(world, slot);
                    ecs_throw(ECS_INVALID_OPERATION,
                        "'%s' is not in hierarchy for slot '%s'",
                            slot_of_str, slot_str);
                    ecs_os_free(slot_of_str);
                    ecs_os_free(slot_str);
                }

                ecs_add_pair(world, parent, slot, child);
                break;
            }

            depth ++;
        } while ((parent = ecs_get_target(world, parent, EcsChildOf, 0)));
        
        if (parent == 0) {
            char *slot_of_str = ecs_get_path(world, slot_of);
            char *slot_str = ecs_get_path(world, slot);
            ecs_throw(ECS_INVALID_OPERATION,
                "'%s' is not in hierarchy for slot '%s'",
                    slot_of_str, slot_str);
            ecs_os_free(slot_of_str);
            ecs_os_free(slot_str);
        }
    }

error:
    return;
}

static
ecs_table_t* flecs_find_table_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_diff_builder_t *diff)
{
    ecs_table_diff_t temp_diff = ECS_TABLE_DIFF_INIT;
    table = flecs_table_traverse_add(world, table, &id, &temp_diff);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_table_diff_build_append_table(world, diff, &temp_diff);
    return table;
error:
    return NULL;
}

static
ecs_table_t* flecs_find_table_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_diff_builder_t *diff)
{
    ecs_table_diff_t temp_diff = ECS_TABLE_DIFF_INIT;
    table = flecs_table_traverse_remove(world, table, &id, &temp_diff);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_table_diff_build_append_table(world, diff, &temp_diff);
    return table;
error:
    return NULL;
}

static
int32_t flecs_child_type_insert(
    ecs_type_t *type,
    void **component_data,
    ecs_id_t id)
{
    int32_t i, count = type->count;
    for (i = 0; i < count; i ++) {
        ecs_id_t cur = type->array[i];
        if (cur == id) {
            /* Id is already part of type */
            return -1;
        }

        if (cur > id) {
            /* A larger id was found so id can't be part of the type. */
            break;
        }
    }

    /* Assumes that the array has enough memory to store the new element. */
    int32_t to_move = type->count - i;
    if (to_move) {
        ecs_os_memmove(&type->array[i + 1],
            &type->array[i], to_move * ECS_SIZEOF(ecs_id_t));
        ecs_os_memmove(&component_data[i + 1],
            &component_data[i], to_move * ECS_SIZEOF(void*));
    }

    component_data[i] = NULL;
    type->array[i] = id;
    type->count ++;

    return i;
}

static
void flecs_instantiate_children(
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

    ecs_entity_t slot_of = 0;
    ecs_entity_t *ids = type.array;
    int32_t type_count = type.count;

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_table_diff_t diff = { .added = {0}};
    diff.added.array = ecs_os_alloca_n(ecs_entity_t, type_count + 1);
    void **component_data = ecs_os_alloca_n(void*, type_count + 1);

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int j, i, childof_base_index = -1;
    for (i = 0; i < type_count; i ++) {
        ecs_id_t id = ids[i];

        /* If id has DontInherit flag don't inherit it, except for the name
         * and ChildOf pairs. The name is preserved so applications can lookup
         * the instantiated children by name. The ChildOf pair is replaced later
         * with the instance parent. */
        if ((id != ecs_pair(ecs_id(EcsIdentifier), EcsName)) &&
            ECS_PAIR_FIRST(id) != EcsChildOf) 
        {
            ecs_table_record_t *tr = &child_table->_->records[i];
            ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
            if (idr->flags & EcsIdOnInstantiateDontInherit) {
                continue;
            }
        }

        /* If child is a slot, keep track of which parent to add it to, but
         * don't add slot relationship to child of instance. If this is a child
         * of a prefab, keep the SlotOf relationship intact. */
        if (!(table->flags & EcsTableIsPrefab)) {
            if (ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsSlotOf) {
                ecs_assert(slot_of == 0, ECS_INTERNAL_ERROR, NULL);
                slot_of = ecs_pair_second(world, id);
                continue;
            }
        }

        /* Keep track of the element that creates the ChildOf relationship with
         * the prefab parent. We need to replace this element to make sure the
         * created children point to the instance and not the prefab */ 
        if (ECS_HAS_RELATION(id, EcsChildOf) && 
           (ECS_PAIR_SECOND(id) == (uint32_t)base)) {
            childof_base_index = diff.added.count;
        }

        int32_t storage_index = ecs_table_type_to_column_index(child_table, i);
        if (storage_index != -1) {
            ecs_vec_t *column = &child_data->columns[storage_index].data;
            component_data[diff.added.count] = ecs_vec_first(column);
        } else {
            component_data[diff.added.count] = NULL;
        }

        diff.added.array[diff.added.count] = id;
        diff.added.count ++;
    }

    /* Table must contain children of base */
    ecs_assert(childof_base_index != -1, ECS_INTERNAL_ERROR, NULL);

    /* If children are added to a prefab, make sure they are prefabs too */
    if (table->flags & EcsTableIsPrefab) {
        if (flecs_child_type_insert(
            &diff.added, component_data, EcsPrefab) != -1) 
        {
            childof_base_index ++;
        }
    }

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *instances = ecs_vec_first(&table->data.entities);
    int32_t child_count = ecs_vec_count(&child_data->entities);

    for (i = row; i < count + row; i ++) {
        ecs_entity_t instance = instances[i];
        ecs_table_t *i_table = NULL;
 
        /* Replace ChildOf element in the component array with instance id */
        diff.added.array[childof_base_index] = ecs_pair(EcsChildOf, instance);

        /* Find or create table */
        i_table = flecs_table_find_or_create(world, &diff.added);

        ecs_assert(i_table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(i_table->type.count == diff.added.count,
            ECS_INTERNAL_ERROR, NULL);

        /* The instance is trying to instantiate from a base that is also
         * its parent. This would cause the hierarchy to instantiate itself
         * which would cause infinite recursion. */
        ecs_entity_t *children = ecs_vec_first(&child_data->entities);

#ifdef FLECS_DEBUG
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];        
            ecs_check(child != instance, ECS_INVALID_PARAMETER, 
                "cycle detected in IsA relationship");
        }
#else
        /* Bit of boilerplate to ensure that we don't get warnings about the
         * error label not being used. */
        ecs_check(true, ECS_INVALID_OPERATION, NULL);
#endif

        /* Create children */
        int32_t child_row;
        const ecs_entity_t *i_children = flecs_bulk_new(world, i_table, NULL, 
            &diff.added, child_count, component_data, false, &child_row, &diff);

        /* If children are slots, add slot relationships to parent */
        if (slot_of) {
            for (j = 0; j < child_count; j ++) {
                ecs_entity_t child = children[j];
                ecs_entity_t i_child = i_children[j];
                flecs_instantiate_slot(world, base, instance, slot_of,
                    child, i_child);
            }
        }

        /* If prefab child table has children itself, recursively instantiate */
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            flecs_instantiate(world, child, i_table, child_row + j, 1);
        }
    }   
error:
    return;    
}

void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    int32_t row,
    int32_t count)
{
    ecs_record_t *record = flecs_entities_get_any(world, base);
    ecs_table_t *base_table = record->table;
    if (!base_table || !(base_table->flags & EcsTableIsPrefab)) {
        /* Don't instantiate children from base entities that aren't prefabs */
        return;
    }

    ecs_id_record_t *idr = flecs_id_record_get(world, ecs_childof(base));
    ecs_table_cache_iter_t it;
    if (idr && flecs_table_cache_all_iter((ecs_table_cache_t*)idr, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            flecs_instantiate_children(
                world, base, table, row, count, tr->hdr.table);
        }
    }
}

static
void flecs_sparse_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *added,
    bool construct)
{
    int32_t i, j;
    for (i = 0; i < added->count; i ++) {
        ecs_id_t id = added->array[i];
        ecs_id_record_t *idr = flecs_id_record_get(world, id);
        if (idr && idr->flags & EcsIdIsSparse) {
            const ecs_type_info_t *ti = idr->type_info;
            ecs_xtor_t ctor = ti->hooks.ctor;
            ecs_iter_action_t on_add = ti->hooks.on_add;
            ecs_entity_t *entities = flecs_table_entities_array(table);
            for (j = 0; j < count; j ++) {
                ecs_entity_t e = entities[row + j];
                void *ptr = flecs_sparse_ensure(idr->sparse, 0, e);
                ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                if (construct && ctor) {
                    ctor(ptr, 1, ti);
                }

                if (on_add) {
                    flecs_invoke_hook(world, table, count, row, &entities[row],
                        ptr, id, ti, EcsOnAdd, on_add);
                }
            }
        }
    }
}

static
void flecs_sparse_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *removed)
{
    int32_t i, j;
    for (i = 0; i < removed->count; i ++) {
        ecs_id_t id = removed->array[i];
        ecs_id_record_t *idr = flecs_id_record_get(world, id);
        if (idr && idr->flags & EcsIdIsSparse) {
            const ecs_type_info_t *ti = idr->type_info;
            ecs_xtor_t dtor = ti->hooks.dtor;
            ecs_iter_action_t on_remove = ti->hooks.on_remove;
            ecs_entity_t *entities = flecs_table_entities_array(table);
            for (j = 0; j < count; j ++) {
                ecs_entity_t e = entities[row + j];
                void *ptr = flecs_sparse_remove_fast(idr->sparse, 0, e);
                ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                if (on_remove) {
                    flecs_invoke_hook(world, table, count, row, &entities[row],
                        ptr, id, ti, EcsOnRemove, on_remove);
                }
                if (dtor) {
                    dtor(ptr, 1, ti);
                }
            }
        }
    }
}

static
void flecs_union_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *added)
{
    int32_t i, j;
    for (i = 0; i < added->count; i ++) {
        ecs_id_t id = added->array[i];
        if (ECS_IS_PAIR(id)) {
            ecs_id_t wc = ecs_pair(ECS_PAIR_FIRST(id), EcsUnion);
            ecs_id_record_t *idr = flecs_id_record_get(world, wc);
            if (idr && idr->flags & EcsIdIsUnion) {
                ecs_entity_t *entities = flecs_table_entities_array(table);
                for (j = 0; j < count; j ++) {
                    ecs_entity_t e = entities[row + j];
                    flecs_switch_set(
                        idr->sparse, (uint32_t)e, ecs_pair_second(world, id));
                }
            }
        }
    }
}

static
void flecs_union_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *removed)
{
    int32_t i, j;
    for (i = 0; i < removed->count; i ++) {
        ecs_id_t id = removed->array[i];
        if (ECS_IS_PAIR(id)) {
            ecs_id_t wc = ecs_pair(ECS_PAIR_FIRST(id), EcsUnion);
            ecs_id_record_t *idr = flecs_id_record_get(world, wc);
            if (idr && idr->flags & EcsIdIsUnion) {
                ecs_entity_t *entities = flecs_table_entities_array(table);
                for (j = 0; j < count; j ++) {
                    ecs_entity_t e = entities[row + j];
                    flecs_switch_reset(idr->sparse, (uint32_t)e);
                }
            }
        }
    }
}

static
void flecs_notify_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_type_t *added,
    ecs_flags32_t flags,
    ecs_flags64_t set_mask,
    bool construct)
{
    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);

    if (added->count) {
        ecs_flags32_t table_flags = table->flags;

        if (table_flags & EcsTableHasSparse) {
            flecs_sparse_on_add(world, table, row, count, added, construct);
        }

        if (table_flags & EcsTableHasUnion) {
            flecs_union_on_add(world, table, row, count, added);
        }

        if (table_flags & (EcsTableHasOnAdd|EcsTableHasIsA|EcsTableHasTraversable)) {
            flecs_emit(world, world, set_mask, &(ecs_event_desc_t){
                .event = EcsOnAdd,
                .ids = added,
                .table = table,
                .other_table = other_table,
                .offset = row,
                .count = count,
                .observable = world,
                .flags = flags
            });
        }
    }
}

void flecs_notify_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_type_t *removed)
{
    ecs_assert(removed != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    if (removed->count) {
        ecs_flags32_t table_flags = table->flags;

        if (table_flags & EcsTableHasSparse) {
            flecs_sparse_on_remove(world, table, row, count, removed);
        }
    
        if (table_flags & EcsTableHasUnion) {
            flecs_union_on_remove(world, table, row, count, removed);
        }

        if (table_flags & (EcsTableHasOnRemove|EcsTableHasIsA|
            EcsTableHasTraversable))
        {
            flecs_emit(world, world, 0, &(ecs_event_desc_t) {
                .event = EcsOnRemove,
                .ids = removed,
                .table = table,
                .other_table = other_table,
                .offset = row,
                .count = count,
                .observable = world
            });
        }
    }
}

static
void flecs_update_name_index(
    ecs_world_t *world,
    ecs_table_t *src, 
    ecs_table_t *dst, 
    int32_t offset, 
    int32_t count) 
{
    ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!(dst->flags & EcsTableHasName)) {
        /* If destination table doesn't have a name, we don't need to update the
         * name index. Even if the src table had a name, the on_remove hook for
         * EcsIdentifier will remove the entity from the index. */
        return;
    }

    ecs_hashmap_t *src_index = src->_->name_index;
    ecs_hashmap_t *dst_index = dst->_->name_index;
    if ((src_index == dst_index) || (!src_index && !dst_index)) {
        /* If the name index didn't change, the entity still has the same parent
         * so nothing needs to be done. */
        return;
    }

    EcsIdentifier *names = ecs_table_get_pair(world, 
        dst, EcsIdentifier, EcsName, offset);
    ecs_assert(names != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    ecs_entity_t *entities = ecs_vec_get_t(
        &dst->data.entities, ecs_entity_t, offset);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        EcsIdentifier *name = &names[i];

        uint64_t index_hash = name->index_hash;
        if (index_hash) {
            flecs_name_index_remove(src_index, e, index_hash);
        }
        const char *name_str = name->value;
        if (name_str) {
            ecs_assert(name->hash != 0, ECS_INTERNAL_ERROR, NULL);

            flecs_name_index_ensure(
                dst_index, e, name_str, name->length, name->hash);
            name->index = dst_index;
        }
    }
}

static
ecs_record_t* flecs_new_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    bool ctor,
    ecs_flags32_t evt_flags)
{
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t row = flecs_table_append(world, table, entity, ctor, true);
    record->table = table;
    record->row = ECS_ROW_TO_RECORD(row, record->row & ECS_ROW_FLAGS_MASK);

    ecs_assert(ecs_vec_count(&table->data.entities) > row, 
        ECS_INTERNAL_ERROR, NULL);
    flecs_notify_on_add(
        world, table, NULL, row, 1, &diff->added, evt_flags, 0, ctor);

    return record;
}

static
void flecs_move_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *dst_table,
    ecs_table_diff_t *diff,
    bool ctor,
    ecs_flags32_t evt_flags)
{
    ecs_table_t *src_table = record->table;
    int32_t src_row = ECS_RECORD_TO_ROW(record->row);

    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_table != dst_table, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_table->type.count > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vec_count(&src_table->data.entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record == flecs_entities_get(world, entity), 
        ECS_INTERNAL_ERROR, NULL);

    /* Append new row to destination table */
    int32_t dst_row = flecs_table_append(world, dst_table, entity, 
        false, false);

    /* Invoke remove actions for removed components */
    flecs_notify_on_remove(
        world, src_table, dst_table, src_row, 1, &diff->removed);

    /* Copy entity & components from src_table to dst_table */
    flecs_table_move(world, entity, entity, dst_table, dst_row, 
        src_table, src_row, ctor);

    /* Update entity index & delete old data after running remove actions */
    record->table = dst_table;
    record->row = ECS_ROW_TO_RECORD(dst_row, record->row & ECS_ROW_FLAGS_MASK);
    
    flecs_table_delete(world, src_table, src_row, false);
    flecs_notify_on_add(
        world, dst_table, src_table, dst_row, 1, &diff->added, evt_flags, 0, ctor);

    flecs_update_name_index(world, src_table, dst_table, dst_row, 1);

error:
    return;
}

static
void flecs_delete_entity(
    ecs_world_t *world,
    ecs_record_t *record,
    ecs_table_diff_t *diff)
{    
    ecs_table_t *table = record->table;
    int32_t row = ECS_RECORD_TO_ROW(record->row);

    /* Invoke remove actions before deleting */
    flecs_notify_on_remove(world, table, NULL, row, 1, &diff->removed);
    flecs_table_delete(world, table, row, true);
}

/* Updating component monitors is a relatively expensive operation that only
 * happens for entities that are monitored. The approach balances the amount of
 * processing between the operation on the entity vs the amount of work that
 * needs to be done to rematch queries, as a simple brute force approach does
 * not scale when there are many tables / queries. Therefore we need to do a bit
 * of bookkeeping that is more intelligent than simply flipping a flag */
static
void flecs_update_component_monitor_w_array(
    ecs_world_t *world,
    ecs_type_t *ids)
{
    if (!ids) {
        return;
    }

    int i;
    for (i = 0; i < ids->count; i ++) {
        ecs_entity_t id = ids->array[i];
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            flecs_monitor_mark_dirty(world, 
                ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard));
        }

        flecs_monitor_mark_dirty(world, id);
    }
}

static
void flecs_update_component_monitors(
    ecs_world_t *world,
    ecs_type_t *added,
    ecs_type_t *removed)
{
    flecs_update_component_monitor_w_array(world, added);
    flecs_update_component_monitor_w_array(world, removed);
}

static
void flecs_commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *dst_table,   
    ecs_table_diff_t *diff,
    bool construct,
    ecs_flags32_t evt_flags)
{
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);
    flecs_journal_begin(world, EcsJournalMove, entity, 
        &diff->added, &diff->removed);
    
    ecs_table_t *src_table = NULL;
    int is_trav = 0;
    if (record) {
        src_table = record->table;
        is_trav = (record->row & EcsEntityIsTraversable) != 0;
    }

    if (src_table == dst_table) {
        /* If source and destination table are the same no action is needed *
         * However, if a component was added in the process of traversing a
         * table, this suggests that a union relationship could have changed. */
        if (src_table) {
            flecs_notify_on_add(world, src_table, src_table, 
                ECS_RECORD_TO_ROW(record->row), 1, &diff->added, evt_flags, 0, 
                    construct);
        }
        flecs_journal_end();
        return;
    }

    if (src_table) {
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_table_traversable_add(dst_table, is_trav);

        if (dst_table->type.count) { 
            flecs_move_entity(world, entity, record, dst_table, diff, 
                construct, evt_flags);
        } else {
            flecs_delete_entity(world, record, diff);
            record->table = NULL;
        }

        flecs_table_traversable_add(src_table, -is_trav);
    } else {        
        flecs_table_traversable_add(dst_table, is_trav);
        if (dst_table->type.count) {
            flecs_new_entity(world, entity, record, dst_table, diff, 
                construct, evt_flags);
        }
    }

    /* If the entity is being watched, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (is_trav) {
        flecs_update_component_monitors(world, &diff->added, &diff->removed);
    }

    if ((!src_table || !src_table->type.count) && world->range_check_enabled) {
        ecs_check(!world->info.max_id || entity <= world->info.max_id, 
            ECS_OUT_OF_RANGE, 0);
        ecs_check(entity >= world->info.min_id, 
            ECS_OUT_OF_RANGE, 0);
    } 

error:
    flecs_journal_end();
    return;
}

static
const ecs_entity_t* flecs_bulk_new(
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
        entities = flecs_entities_new_ids(world, count);
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
    for (i = 0; i < count; i ++) {
        ecs_record_t *r = flecs_entities_get(world, entities[i]);
        r->table = table;
        r->row = ECS_ROW_TO_RECORD(row + i, 0);
    }

    flecs_defer_begin(world, world->stages[0]);

    flecs_notify_on_add(world, table, NULL, row, count, &diff->added, 
        (component_data == NULL) ? 0 : EcsEventNoOnSet, 0, true);

    if (component_data) {
        int32_t c_i;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            /* Find component in storage type */
            ecs_entity_t id = component_ids->array[c_i];
            const ecs_table_record_t *tr = flecs_table_record_get(
                world, table, id);
            ecs_assert(tr != NULL, ECS_INVALID_PARAMETER, 
                "id is not a component");
            ecs_assert(tr->column != -1, ECS_INVALID_PARAMETER, 
                "id is not a component");
            ecs_assert(tr->count == 1, ECS_INVALID_PARAMETER,
                "ids cannot be wildcards");

            int32_t index = tr->column;
            ecs_column_t *column = &table->data.columns[index];
            ecs_type_info_t *ti = column->ti;
            int32_t size = column->size;
            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
            void *ptr = ecs_vec_get(&column->data, size, row);

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

        int32_t j, storage_count = table->column_count;
        for (j = 0; j < storage_count; j ++) {
            ecs_type_t set_type = {
                .array = &table->data.columns[j].id,
                .count = 1
            };

            flecs_notify_on_set(world, table, row, count, &set_type, true);
        }
    }

    flecs_defer_end(world, world->stages[0]);

    if (row_out) {
        *row_out = row;
    }

    if (sparse_count) {
        entities = flecs_entities_ids(world);
        return &entities[sparse_count];
    } else {
        return entities;
    }
}

static
void flecs_add_id_w_record(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_id_t id,
    bool construct)
{
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *src_table = record->table;
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &id, &diff);
    flecs_commit(world, entity, record, dst_table, &diff, construct, 
        EcsEventNoOnSet); /* No OnSet, this function is only called from
                           * functions that are about to set the component. */
}

static
void flecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_add(stage, entity, id)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *src_table = r->table;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &id, &diff);

    flecs_commit(world, entity, r, dst_table, &diff, true, 0);

    flecs_defer_end(world, stage);
}

static
void flecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_remove(stage, entity, id)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *src_table = r->table;
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, &id, &diff);

    flecs_commit(world, entity, r, dst_table, &diff, true, 0);

    flecs_defer_end(world, stage);
}

static
flecs_component_ptr_t flecs_ensure(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t id,
    ecs_record_t *r)
{
    flecs_component_ptr_t dst = {0};

    flecs_poly_assert(world, ecs_world_t);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check((id & ECS_COMPONENT_MASK) == id || 
        ECS_HAS_ID_FLAG(id, PAIR), ECS_INVALID_PARAMETER,
            "invalid component id specified for ensure");

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (r->table) {
        dst = flecs_get_component_ptr(r->table, ECS_RECORD_TO_ROW(r->row), idr);
        if (dst.ptr) {
            return dst;
        }
    }

    /* If entity didn't have component yet, add it */
    flecs_add_id_w_record(world, entity, r, id, true);

    /* Flush commands so the pointer we're fetching is stable */
    flecs_defer_end(world, world->stages[0]);
    flecs_defer_begin(world, world->stages[0]);

    if (!idr) {
        idr = flecs_id_record_get(world, id);
        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
    dst = flecs_get_component_ptr(r->table, ECS_RECORD_TO_ROW(r->row), idr);
error:
    return dst;
}

void flecs_invoke_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t count,
    int32_t row,
    ecs_entity_t *entities,
    void *ptr,
    ecs_id_t id,
    const ecs_type_info_t *ti,
    ecs_entity_t event,
    ecs_iter_action_t hook)
{
    int32_t defer = world->stages[0]->defer;
    if (defer < 0) {
        world->stages[0]->defer *= -1;
    }

    ecs_iter_t it = { .field_count = 1};
    it.entities = entities;
    
    flecs_iter_init(world, &it, flecs_iter_cache_all);
    it.world = world;
    it.real_world = world;
    it.table = table;
    it.ptrs[0] = ptr;
    it.sizes = ECS_CONST_CAST(ecs_size_t*, &ti->size);
    it.ids[0] = id;
    it.event = event;
    it.event_id = id;
    it.ctx = ti->hooks.ctx;
    it.callback_ctx = ti->hooks.binding_ctx;
    it.count = count;
    it.offset = row;
    flecs_iter_validate(&it);
    hook(&it);
    ecs_iter_fini(&it);

    world->stages[0]->defer = defer;
}

void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_type_t *ids,
    bool owned)
{
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *data = &table->data;

    ecs_entity_t *entities = ecs_vec_get_t(
        &data->entities, ecs_entity_t, row);
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vec_count(&data->entities), 
        ECS_INTERNAL_ERROR, NULL);

    if (owned) {
        int i;
        for (i = 0; i < ids->count; i ++) {
            ecs_id_t id = ids->array[i];
            ecs_id_record_t *idr = flecs_id_record_get(world, id);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *ti = idr->type_info;
            ecs_iter_action_t on_set = ti->hooks.on_set;
            if (!on_set) {
                continue;
            }

            if (idr->flags & EcsIdIsSparse) {
                int32_t j;
                for (j = 0; j < count; j ++) {
                    void *ptr = flecs_sparse_get_any(
                        idr->sparse, 0, entities[j]);
                    flecs_invoke_hook(world, table, 1, row, &entities[j], ptr,
                        id, ti, EcsOnSet, on_set);
                }
            } else {
                const ecs_table_record_t *tr = 
                    flecs_id_record_get_table(idr, table);
                ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(tr->column != -1, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);

                int32_t index = tr->column;
                ecs_column_t *column = &table->data.columns[index];
                if (on_set) {
                    ecs_vec_t *c = &column->data;
                    void *ptr = ecs_vec_get(c, column->size, row);
                    flecs_invoke_hook(world, table, count, row, entities, ptr, 
                        id, ti, EcsOnSet, on_set);
                }
            }
        }
    }

    /* Run OnSet notifications */
    if (table->flags & EcsTableHasOnSet && ids->count) {
        flecs_emit(world, world, 0, &(ecs_event_desc_t) {
            .event = EcsOnSet,
            .ids = ids,
            .table = table,
            .offset = row,
            .count = count,
            .observable = world
        });
    }
}

void flecs_record_add_flag(
    ecs_record_t *record,
    uint32_t flag)
{
    if (flag == EcsEntityIsTraversable) {
        if (!(record->row & flag)) {
            ecs_table_t *table = record->table;
            if (table) {
                flecs_table_traversable_add(table, 1);
            }
        }
    }
    record->row |= flag;
}

void flecs_add_flag(
    ecs_world_t *world,
    ecs_entity_t entity,
    uint32_t flag)
{
    ecs_record_t *record = flecs_entities_get_any(world, entity);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_record_add_flag(record, flag);
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
    ecs_check(!ecs_is_deferred(world), ECS_INVALID_OPERATION, 
        "commit cannot be called on stage or while world is deferred");

    ecs_table_t *src_table = NULL;
    if (!record) {
        record = flecs_entities_get(world, entity);
        src_table = record->table;
    }

    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;

    if (added) {
        diff.added = *added;
    }
    if (removed) {
        diff.removed = *removed;
    }

    ecs_defer_begin(world);
    flecs_commit(world, entity, record, table, &diff, true, 0);
    ecs_defer_end(world);

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

ecs_entity_t ecs_new(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    /* It is possible that the world passed to this function is a stage, so
     * make sure we have the actual world. Cast away const since this is one of
     * the few functions that may modify the world while it is in readonly mode,
     * since it is thread safe (uses atomic inc when in threading mode) */
    ecs_world_t *unsafe_world = 
        ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    ecs_entity_t entity;
    if (unsafe_world->flags & EcsWorldMultiThreaded) {
        /* When world is in multithreading mode, make sure OS API has threading 
         * functions initialized */
        ecs_assert(ecs_os_has_threading(), ECS_INVALID_OPERATION, 
            "thread safe id creation unavailable: threading API not available");

        /* Can't atomically increase number above max int */
        ecs_assert(flecs_entities_max_id(unsafe_world) < UINT_MAX, 
            ECS_INVALID_OPERATION, "thread safe ids exhausted");
        entity = (ecs_entity_t)ecs_os_ainc(
            (int32_t*)&flecs_entities_max_id(unsafe_world));
    } else {
        entity = flecs_entities_new_id(unsafe_world);
    }

    ecs_assert(!unsafe_world->info.max_id || 
        ecs_entity_t_lo(entity) <= unsafe_world->info.max_id, 
        ECS_OUT_OF_RANGE, NULL);

    flecs_journal(world, EcsJournalNew, entity, 0, 0);

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
    ecs_world_t *unsafe_world = 
        ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));
    if (unsafe_world->flags & EcsWorldReadonly) {
        /* Can't issue new comp id while iterating when in multithreaded mode */
        ecs_check(ecs_get_stage_count(world) <= 1,
            ECS_INVALID_WHILE_READONLY, NULL);
    }

    ecs_entity_t id = 0;
    if (unsafe_world->info.last_component_id < FLECS_HI_COMPONENT_ID) {
        do {
            id = unsafe_world->info.last_component_id ++;
        } while (ecs_exists(unsafe_world, id) && id <= FLECS_HI_COMPONENT_ID);        
    }

    if (!id || id >= FLECS_HI_COMPONENT_ID) {
        /* If the low component ids are depleted, return a regular entity id */
        id = ecs_new(unsafe_world);
    } else {
        flecs_entities_ensure(world, id);
    }

    ecs_assert(ecs_get_type(world, id) == NULL, ECS_INTERNAL_ERROR, NULL);

    return id;
error: 
    return 0;
}

ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);    
    ecs_entity_t entity = ecs_new(world);

    if (flecs_defer_add(stage, entity, id)) {
        return entity;
    }

    ecs_table_diff_builder_t diff_builder = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff_builder);
    ecs_table_t *table = flecs_find_table_add(
        world, &world->store.root, id, &diff_builder);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_diff_t diff;
    flecs_table_diff_build_noalloc(&diff_builder, &diff);
    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_new_entity(world, entity, r, table, &diff, true, 0);
    flecs_table_diff_builder_fini(world, &diff_builder);

    flecs_defer_end(world, stage);

    return entity;
error:
    return 0;
}

ecs_entity_t ecs_new_w_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_stage_from_world(&world);    
    ecs_entity_t entity = ecs_new(world);
    ecs_record_t *r = flecs_entities_get(world, entity);

    ecs_table_diff_t table_diff = { .added = table->type };
    flecs_new_entity(world, entity, r, table, &table_diff, true, 0);
    return entity;
error:
    return 0;
}

static
void flecs_copy_id(
    ecs_world_t *world,
    ecs_record_t *r,
    ecs_id_t id,
    size_t size,
    void *dst_ptr,
    void *src_ptr,
    const ecs_type_info_t *ti)
{
    ecs_check(dst_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(src_ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_copy_t copy = ti->hooks.copy;
    if (copy) {
        copy(dst_ptr, src_ptr, 1, ti);
    } else {
        ecs_os_memcpy(dst_ptr, src_ptr, flecs_utosize(size));
    }

    flecs_table_mark_dirty(world, r->table, id);

    ecs_table_t *table = r->table;
    if (table->flags & EcsTableHasOnSet || ti->hooks.on_set) {
        ecs_type_t ids = { .array = &id, .count = 1 };
        flecs_notify_on_set(
            world, table, ECS_RECORD_TO_ROW(r->row), 1, &ids, true);
    }
error:
    return;
}

/* Traverse table graph by either adding or removing identifiers parsed from the
 * passed in expression. */
static
int flecs_traverse_from_expr(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_vec_t *ids)
{
#ifdef FLECS_SCRIPT
    const char *ptr = expr;
    if (ptr) {
        ecs_id_t id = 0;
        while (ptr[0] && (ptr = flecs_id_parse(world, name, ptr, &id))) {
            if (!id) {
                break;
            }
            ecs_vec_append_t(&world->allocator, ids, ecs_id_t)[0] = id;
        }

        if (!ptr) {
            goto error;
        }
    }
    return 0;
#else
    (void)world;
    (void)name;
    (void)expr;
    (void)ids;
    ecs_err("cannot parse component expression: script addon required");
    goto error;
#endif
error:
    return -1;
}

/* Add/remove components based on the parsed expression. This operation is 
 * slower than flecs_traverse_from_expr, but safe to use from a deferred context. */
static
void flecs_defer_from_expr(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const char *expr)
{
#ifdef FLECS_SCRIPT
    const char *ptr = expr;
    if (ptr) {
        ecs_id_t id = 0;
        while (ptr[0] && (ptr = flecs_id_parse(world, name, ptr, &id))) {
            if (!id) {
                break;
            }
            ecs_add_id(world, entity, id);
        }
    }
#else
    (void)world;
    (void)entity;
    (void)name;
    (void)expr;
    ecs_err("cannot parse component expression: script addon required");
#endif
}

/* If operation is not deferred, add components by finding the target
 * table and moving the entity towards it. */
static 
int flecs_traverse_add(
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
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);
    ecs_vec_t ids;

    /* Add components from the 'add_expr' expression. Look up before naming 
     * entity, so that expression can't resolve to self. */
    ecs_vec_init_t(&world->allocator, &ids, ecs_id_t, 0);
    if (desc->add_expr && ecs_os_strcmp(desc->add_expr, "0")) {
        if (flecs_traverse_from_expr(world, name, desc->add_expr, &ids)) {
            goto error;
        }
    }

    /* Set symbol */
    if (desc->symbol && desc->symbol[0]) {
        const char *sym = ecs_get_symbol(world, result);
        if (sym) {
            ecs_assert(!ecs_os_strcmp(desc->symbol, sym),
                ECS_INCONSISTENT_NAME, desc->symbol);
        } else {
            ecs_set_symbol(world, result, desc->symbol);
        }
    }

    /* If a name is provided but not yet assigned, add the Name component */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, result, scope, name, sep, root_sep);
    } else if (new_entity && scope) {
        ecs_add_pair(world, result, EcsChildOf, scope);
    }

    /* Find existing table */
    ecs_table_t *src_table = NULL, *table = NULL;
    ecs_record_t *r = flecs_entities_get(world, result);
    table = r->table;

    /* Add components from the 'add' array */
    if (desc->add) {
        int32_t i = 0;
        ecs_id_t id;

        while ((id = desc->add[i ++])) {
            table = flecs_find_table_add(world, table, id, &diff);
        }
    }

    /* Add components from the 'set' array */
    if (desc->set) {
        int32_t i = 0;
        ecs_id_t id;

        while ((id = desc->set[i ++].type)) {
            table = flecs_find_table_add(world, table, id, &diff);
        }
    }

    /* Add ids from .expr */
    {
        int32_t i, count = ecs_vec_count(&ids);
        ecs_id_t *expr_ids = ecs_vec_first(&ids);
        for (i = 0; i < count; i ++) {
            table = flecs_find_table_add(world, table, expr_ids[i], &diff);
        }
    }

    /* Find destination table */
    /* If this is a new entity without a name, add the scope. If a name is
     * provided, the scope will be added by the add_path_w_sep function */
    if (new_entity) {
        if (new_entity && scope && !name && !name_assigned) {
            table = flecs_find_table_add(
                world, table, ecs_pair(EcsChildOf, scope), &diff);
        }
        if (with) {
            table = flecs_find_table_add(world, table, with, &diff);
        }
    }

    /* Commit entity to destination table */
    if (src_table != table) {
        flecs_defer_begin(world, world->stages[0]);
        ecs_table_diff_t table_diff;
        flecs_table_diff_build_noalloc(&diff, &table_diff);
        flecs_commit(world, result, r, table, &table_diff, true, 0);
        flecs_table_diff_builder_fini(world, &diff);
        flecs_defer_end(world, world->stages[0]);
    }

    /* Set component values */
    if (desc->set) {
        table = r->table;
        ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t i = 0, row = ECS_RECORD_TO_ROW(r->row);
        const ecs_value_t *v;
        
        flecs_defer_begin(world, world->stages[0]);

        while ((void)(v = &desc->set[i ++]), v->type) {
            if (!v->ptr) {
                continue;
            }
            ecs_assert(ECS_RECORD_TO_ROW(r->row) == row, ECS_INTERNAL_ERROR, NULL);
            ecs_id_record_t *idr = flecs_id_record_get(world, v->type);
            flecs_component_ptr_t ptr = flecs_get_component_ptr(table, row, idr);
            ecs_check(ptr.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *ti = idr->type_info;
            flecs_copy_id(world, r, v->type, 
                flecs_itosize(ti->size), ptr.ptr, v->ptr, ti);
        }

        flecs_defer_end(world, world->stages[0]);
    }

    flecs_table_diff_builder_fini(world, &diff);
    ecs_vec_fini_t(&world->allocator, &ids, ecs_id_t);
    return 0;
error:
    ecs_vec_fini_t(&world->allocator, &ids, ecs_id_t);
    return -1;
}

/* When in deferred mode, we need to add/remove components one by one using
 * the regular operations. */
static 
void flecs_deferred_add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const ecs_entity_desc_t *desc,
    ecs_entity_t scope,
    ecs_id_t with,
    bool flecs_new_entity,
    bool name_assigned)
{
    const char *sep = desc->sep;
    const char *root_sep = desc->root_sep;

    /* If this is a new entity without a name, add the scope. If a name is
     * provided, the scope will be added by the add_path_w_sep function */
    if (flecs_new_entity) {
        if (flecs_new_entity && scope && !name && !name_assigned) {
            ecs_add_id(world, entity, ecs_pair(EcsChildOf, scope));
        }

        if (with) {
            ecs_add_id(world, entity, with);
        }
    }

    /* Add components from the 'add' id array */
    if (desc->add) {
        int32_t i = 0;
        ecs_id_t id;

        while ((id = desc->add[i ++])) {
            bool defer = true;
            if (ECS_HAS_ID_FLAG(id, PAIR) && ECS_PAIR_FIRST(id) == EcsChildOf) {
                scope = ECS_PAIR_SECOND(id);
                if (name && (!desc->id || !name_assigned)) {
                    /* New named entities are created by temporarily going out of
                     * readonly mode to ensure no duplicates are created. */
                    defer = false;
                }
            }
            if (defer) {
                ecs_add_id(world, entity, id);
            }
        }
    }

    /* Set component values */
    if (desc->set) {
        int32_t i = 0;
        const ecs_value_t *v;
        while ((void)(v = &desc->set[i ++]), v->type) {
            if (v->ptr) {
                ecs_set_id(world, entity, v->type, 0, v->ptr);
            } else {
                ecs_add_id(world, entity, v->type);
            }
        }
    }

    /* Add components from the 'add_expr' expression */
    if (desc->add_expr) {
        flecs_defer_from_expr(world, entity, name, desc->add_expr);
    }

    int32_t thread_count = ecs_get_stage_count(world);

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

    /* Set name */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, entity, scope, name, sep, root_sep);
    }
}

ecs_entity_t ecs_entity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_entity_desc_t was not initialized to zero");

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_entity_t scope = stage->scope;
    ecs_id_t with = ecs_get_with(world);
    ecs_entity_t result = desc->id;

#ifdef FLECS_DEBUG
    if (desc->add) {
        ecs_id_t id;
        int32_t i = 0;
        while ((id = desc->add[i ++])) {
            if (ECS_HAS_ID_FLAG(id, PAIR) && 
                (ECS_PAIR_FIRST(id) == EcsChildOf))
            {
                if (desc->name) {
                    ecs_check(false, ECS_INVALID_PARAMETER, "%s: cannot set parent in "
                        "ecs_entity_desc_t::add, use ecs_entity_desc_t::parent",
                            desc->name);
                } else {
                    ecs_check(false, ECS_INVALID_PARAMETER, "cannot set parent in "
                        "ecs_entity_desc_t::add, use ecs_entity_desc_t::parent");
                }
            }
        }
    }
#endif

    const char *name = desc->name;
    const char *sep = desc->sep;
    if (!sep) {
        sep = ".";
    }

    if (name) {
        if (!name[0]) {
            name = NULL;
        } else if (flecs_name_is_id(name)){
            ecs_entity_t id = flecs_name_to_id(name);
            if (!id) {
                return 0;
            }
            if (result && (id != result)) {
                ecs_err("name id conflicts with provided id");
                return 0;
            }
            name = NULL;
            result = id;
        }
    }

    const char *root_sep = desc->root_sep;
    bool flecs_new_entity = false;
    bool name_assigned = false;

    /* Remove optional prefix from name. Entity names can be derived from 
     * language identifiers, such as components (typenames) and systems
     * function names). Because C does not have namespaces, such identifiers
     * often encode the namespace as a prefix.
     * To ensure interoperability between C and C++ (and potentially other 
     * languages with namespacing) the entity must be stored without this prefix
     * and with the proper namespace, which is what the name_prefix is for */
    const char *prefix = world->info.name_prefix;
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

    /* Parent field takes precedence over scope */
    if (desc->parent) {
        scope = desc->parent;
    }

    /* Find or create entity */
    if (!result) {
        if (name) {
            /* If add array contains a ChildOf pair, use it as scope instead */
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
                result = ecs_new(world);
            }
            flecs_new_entity = true;
            ecs_assert(ecs_get_type(world, result) == NULL,
                ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        /* Make sure provided id is either alive or revivable */
        ecs_make_alive(world, result);

        name_assigned = ecs_has_pair(
            world, result, ecs_id(EcsIdentifier), EcsName);
        if (name && name_assigned) {
            /* If entity has name, verify that name matches. The name provided
             * to the function could either have been relative to the current
             * scope, or fully qualified. */
            char *path;
            ecs_size_t root_sep_len = root_sep ? ecs_os_strlen(root_sep) : 0;
            if (root_sep && !ecs_os_strncmp(name, root_sep, root_sep_len)) {
                /* Fully qualified name was provided, so make sure to
                 * compare with fully qualified name */
                path = ecs_get_path_w_sep(world, 0, result, sep, root_sep);
            } else {
                /* Relative name was provided, so make sure to compare with
                 * relative name */
                if (!sep || sep[0]) {
                    path = ecs_get_path_w_sep(world, scope, result, sep, "");
                } else {
                    /* Safe, only freed when sep is valid */
                    path = ECS_CONST_CAST(char*, ecs_get_name(world, result));
                }
            }
            if (path) {
                if (ecs_os_strcmp(path, name)) {
                    /* Mismatching name */
                    ecs_err("existing entity '%s' is initialized with "
                        "conflicting name '%s'", path, name);
                    if (!sep || sep[0]) {
                        ecs_os_free(path);
                    }
                    return 0;
                }
                if (!sep || sep[0]) {
                    ecs_os_free(path);
                }
            }
        }
    }

    ecs_assert(name_assigned == ecs_has_pair(
        world, result, ecs_id(EcsIdentifier), EcsName),
            ECS_INTERNAL_ERROR, NULL);

    if (stage->defer) {
        flecs_deferred_add_remove((ecs_world_t*)stage, result, name, desc, 
            scope, with, flecs_new_entity, name_assigned);
    } else {
        if (flecs_traverse_add(world, result, name, desc,
            scope, with, flecs_new_entity, name_assigned)) 
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
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_bulk_desc_t was not initialized to zero");

    const ecs_entity_t *entities = desc->entities;
    int32_t count = desc->count;

    int32_t sparse_count = 0;
    if (!entities) {
        sparse_count = flecs_entities_count(world);
        entities = flecs_entities_new_ids(world, count);
        ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    } else {
        int i;
        for (i = 0; i < count; i ++) {
            ecs_make_alive(world, entities[i]);
        }
    }

    ecs_type_t ids;
    ecs_table_t *table = desc->table;
    if (!table) {
        ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
        flecs_table_diff_builder_init(world, &diff);

        int32_t i = 0;
        ecs_id_t id;
        while ((id = desc->ids[i])) {
            table = flecs_find_table_add(world, table, id, &diff);
            i ++;
        }

        ids.array = ECS_CONST_CAST(ecs_id_t*, desc->ids);
        ids.count = i;

        ecs_table_diff_t table_diff;
        flecs_table_diff_build_noalloc(&diff, &table_diff);
        flecs_bulk_new(world, table, entities, &ids, count, desc->data, true, NULL, 
            &table_diff);
        flecs_table_diff_builder_fini(world, &diff);
    } else {
        ecs_table_diff_t diff = {
            .added.array = table->type.array,
            .added.count = table->type.count
        };
        ids = (ecs_type_t){.array = diff.added.array, .count = diff.added.count};
        flecs_bulk_new(world, table, entities, &ids, count, desc->data, true, NULL, 
            &diff);
    }

    if (!sparse_count) {
        return entities;
    } else {
        /* Refetch entity ids, in case the underlying array was reallocated */
        entities = flecs_entities_ids(world);
        return &entities[sparse_count];
    }
error:
    return NULL;
}

static
void flecs_check_component(
    ecs_world_t *world,
    ecs_entity_t result,
    const EcsComponent *ptr,
    ecs_size_t size,
    ecs_size_t alignment)
{
    if (ptr->size != size) {
        char *path = ecs_get_path(world, result);
        ecs_abort(ECS_INVALID_COMPONENT_SIZE, path);
        ecs_os_free(path);
    }
    if (ptr->alignment != alignment) {
        char *path = ecs_get_path(world, result);
        ecs_abort(ECS_INVALID_COMPONENT_ALIGNMENT, path);
        ecs_os_free(path);
    }
}

ecs_entity_t ecs_component_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_component_desc_t was not initialized to 0");

    /* If existing entity is provided, check if it is already registered as a
     * component and matches the size/alignment. This can prevent having to
     * suspend readonly mode, and increases the number of scenarios in which
     * this function can be called in multithreaded mode. */
    ecs_entity_t result = desc->entity;
    if (result && ecs_is_alive(world, result)) {
        const EcsComponent *const_ptr = ecs_get(world, result, EcsComponent);
        if (const_ptr) {
            flecs_check_component(world, result, const_ptr,
                desc->type.size, desc->type.alignment);
            return result;
        }
    }

    ecs_suspend_readonly_state_t readonly_state;
    world = flecs_suspend_readonly(world, &readonly_state);

    bool new_component = true;
    if (!result) {
        result = ecs_new_low_id(world);
    } else {
        ecs_make_alive(world, result);
        new_component = ecs_has(world, result, EcsComponent);
    }

    EcsComponent *ptr = ecs_ensure(world, result, EcsComponent);
    if (!ptr->size) {
        ecs_assert(ptr->alignment == 0, ECS_INTERNAL_ERROR, NULL);
        ptr->size = desc->type.size;
        ptr->alignment = desc->type.alignment;
        if (!new_component || ptr->size != desc->type.size) {
            if (!ptr->size) {
                ecs_trace("#[green]tag#[reset] %s created", 
                    ecs_get_name(world, result));
            } else {
                ecs_trace("#[green]component#[reset] %s created", 
                    ecs_get_name(world, result));
            }
        }
    } else {
        flecs_check_component(world, result, ptr,
            desc->type.size, desc->type.alignment);
    }

    if (desc->type.name && new_component) {
        ecs_entity(world, { .id = result, .name = desc->type.name });
    }

    ecs_modified(world, result, EcsComponent);

    if (desc->type.size && 
        !ecs_id_in_use(world, result) && 
        !ecs_id_in_use(world, ecs_pair(result, EcsWildcard)))
    {
        ecs_set_hooks_id(world, result, &desc->type.hooks);
    }

    if (result >= world->info.last_component_id && result < FLECS_HI_COMPONENT_ID) {
        world->info.last_component_id = result + 1;
    }

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
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);
    
    if (id) {
        table = flecs_find_table_add(world, table, id, &diff);
    }

    ecs_table_diff_t td;
    flecs_table_diff_build_noalloc(&diff, &td);
    ids = flecs_bulk_new(world, table, NULL, NULL, count, NULL, false, NULL, &td);
    flecs_table_diff_builder_fini(world, &diff);
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
    if (flecs_defer_clear(stage, entity)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = r->table;
    if (table) {
        ecs_table_diff_t diff = {
            .removed = table->type
        };

        flecs_delete_entity(world, r, &diff);
        r->table = NULL;

        if (r->row & EcsEntityIsTraversable) {
            flecs_table_traversable_add(table, -1);
        }
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
        ecs_err("(OnDelete, Panic) constraint violated while deleting %s", 
            id_str);
        ecs_os_free(id_str);
        #ifndef FLECS_SOFT_ASSERT
        ecs_abort(ECS_CONSTRAINT_VIOLATED, NULL);
        #endif
    }    
}

static
void flecs_marked_id_push(
    ecs_world_t *world,
    ecs_id_record_t* idr,
    ecs_entity_t action,
    bool delete_id)
{
    ecs_marked_id_t *m = ecs_vec_append_t(&world->allocator,
        &world->store.marked_ids, ecs_marked_id_t);

    m->idr = idr;
    m->id = idr->id;
    m->action = action;
    m->delete_id = delete_id;

    flecs_id_record_claim(world, idr);
}

static
void flecs_id_mark_for_delete(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_entity_t action,
    bool delete_id);

static
void flecs_targets_mark_for_delete(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_id_record_t *idr;
    ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
    int32_t i, count = ecs_vec_count(&table->data.entities);
    for (i = 0; i < count; i ++) {
        ecs_record_t *r = flecs_entities_get(world, entities[i]);
        if (!r) {
            continue;
        }

        /* If entity is not used as id or as relationship target, there won't
         * be any tables with a reference to it. */
        ecs_flags32_t flags = r->row & ECS_ROW_FLAGS_MASK;
        if (!(flags & (EcsEntityIsId|EcsEntityIsTarget))) {
            continue;
        }

        ecs_entity_t e = entities[i];
        if (flags & EcsEntityIsId) {
            if ((idr = flecs_id_record_get(world, e))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE(idr->flags), true);
            }
            if ((idr = flecs_id_record_get(world, ecs_pair(e, EcsWildcard)))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE(idr->flags), true);
            }
        }
        if (flags & EcsEntityIsTarget) {
            if ((idr = flecs_id_record_get(world, ecs_pair(EcsWildcard, e)))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE_TARGET(idr->flags), true);
            }
            if ((idr = flecs_id_record_get(world, ecs_pair(EcsFlag, e)))) {
                flecs_id_mark_for_delete(world, idr, 
                    ECS_ID_ON_DELETE_TARGET(idr->flags), true);
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
         * has the form (*, Target), use OnDeleteTarget action */
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
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        ecs_id_t id = idr->id;

        /* If action is not specified and we're deleting a relationship target,
         * derive the action from the current record */
        int32_t i = tr->index, count = tr->count;
        do {
            ecs_type_t *type = &table->type;
            ecs_table_record_t *trr = &table->_->records[i];
            ecs_id_record_t *idrr = (ecs_id_record_t*)trr->hdr.cache;
            result = ECS_ID_ON_DELETE_TARGET(idrr->flags);
            if (result == EcsDelete) {
                /* Delete takes precedence over Remove */
                break;
            }

            if (count > 1) {
                /* If table contains multiple pairs for target they are not
                 * guaranteed to occupy consecutive elements in the table's type
                 * vector, so a linear search is needed to find matches. */
                for (++ i; i < type->count; i ++) {
                    if (ecs_id_match(type->array[i], id)) {
                        break;
                    }
                }

                /* We should always have as many matching ids as tr->count */
                ecs_assert(i < type->count, ECS_INTERNAL_ERROR, NULL);
            }
        } while (--count);
    }

    return result;
}

static
void flecs_update_monitors_for_delete(
    ecs_world_t *world,
    ecs_id_t id)
{
    flecs_update_component_monitors(world, NULL, &(ecs_type_t){
        .array = (ecs_id_t[]){id},
        .count = 1
    });
}

static
void flecs_id_mark_for_delete(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_entity_t action,
    bool delete_id)
{
    if (idr->flags & EcsIdMarkedForDelete) {
        return;
    }

    idr->flags |= EcsIdMarkedForDelete;
    flecs_marked_id_push(world, idr, action, delete_id);

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
        ecs_assert(ECS_HAS_ID_FLAG(id, PAIR), ECS_INTERNAL_ERROR, NULL);
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
    ecs_entity_t action,
    bool delete_id)
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

    flecs_id_mark_for_delete(world, idr, action, delete_id);

    return true;
}

static
void flecs_remove_from_table(
    ecs_world_t *world, 
    ecs_table_t *table) 
{
    ecs_table_diff_t temp_diff = { .added = {0} };
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);
    ecs_table_t *dst_table = table; 

    /* To find the dst table, remove all ids that are marked for deletion */
    int32_t i, t, count = ecs_vec_count(&world->store.marked_ids);
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    const ecs_table_record_t *tr;
    for (i = 0; i < count; i ++) {
        const ecs_id_record_t *idr = ids[i].idr;

        if (!(tr = flecs_id_record_get_table(idr, dst_table))) {
            continue;
        }

        t = tr->index;

        do {
            ecs_id_t id = dst_table->type.array[t];
            ecs_table_t *tgt_table = flecs_table_traverse_remove(
                world, dst_table, &id, &temp_diff);
            ecs_assert(tgt_table != dst_table, ECS_INTERNAL_ERROR, NULL);
            dst_table = tgt_table;
            flecs_table_diff_build_append_table(world, &diff, &temp_diff);
        } while (dst_table->type.count && (t = ecs_search_offset(
            world, dst_table, t, idr->id, NULL)) != -1);
    }

    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!dst_table->type.count) {
        /* If this removes all components, clear table */
        flecs_table_clear_entities(world, table);
    } else {
        /* Otherwise, merge table into dst_table */
        if (dst_table != table) {
            int32_t table_count = ecs_table_count(table);
            if (diff.removed.count && table_count) {
                ecs_log_push_3();
                ecs_table_diff_t td;
                flecs_table_diff_build_noalloc(&diff, &td);
                flecs_notify_on_remove(world, table, NULL, 0, table_count, 
                    &td.removed);
                ecs_log_pop_3();
            }

            flecs_table_merge(world, dst_table, table);
        }
    }

    flecs_table_diff_builder_fini(world, &diff);
}

static
bool flecs_on_delete_clear_tables(
    ecs_world_t *world)
{
    /* Iterate in reverse order so that DAGs get deleted bottom to top */
    int32_t i, last = ecs_vec_count(&world->store.marked_ids), first = 0;
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    do {
        for (i = last - 1; i >= first; i --) {
            ecs_id_record_t *idr = ids[i].idr;
            ecs_entity_t action = ids[i].action;

            /* Empty all tables for id */
            {
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
            }

            /* Run commands so children get notified before parent is deleted */
            if (world->stages[0]->defer) {
                flecs_defer_end(world, world->stages[0]);
                flecs_defer_begin(world, world->stages[0]);
            }

            /* User code (from triggers) could have enqueued more ids to delete,
             * reobtain the array in case it got reallocated */
            ids = ecs_vec_first(&world->store.marked_ids);
        }

        /* Check if new ids were marked since we started */
        int32_t new_last = ecs_vec_count(&world->store.marked_ids);
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
    int32_t i, count = ecs_vec_count(&world->store.marked_ids);
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    int twice = 2;

    do {
        for (i = 0; i < count; i ++) {
            /* Release normal ids before wildcard ids */
            if (ecs_id_is_wildcard(ids[i].id)) {
                if (twice == 2) {
                    continue;
                }
            } else {
                if (twice == 1) {
                    continue;
                }
            }

            ecs_id_record_t *idr = ids[i].idr;
            bool delete_id = ids[i].delete_id;

            flecs_id_record_release_tables(world, idr);

            /* Release the claim taken by flecs_marked_id_push. This may delete the
             * id record as all other claims may have been released. */
            int32_t rc = flecs_id_record_release(world, idr);
            ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, NULL);
            (void)rc;

            /* If rc is 0, the id was likely deleted by a nested delete_with call
             * made by an on_remove handler/OnRemove observer */
            if (rc) {
                if (delete_id) {
                    /* If id should be deleted, release initial claim. This happens when
                     * a component, tag, or part of a pair is deleted. */
                    flecs_id_record_release(world, idr);
                } else {
                    /* If id should not be deleted, unmark id record for deletion. This
                     * happens when all instances *of* an id are deleted, for example
                     * when calling ecs_remove_all or ecs_delete_with. */
                    idr->flags &= ~EcsIdMarkedForDelete;
                }
            }
        }
    } while (-- twice);

    return true;
}

static
void flecs_on_delete(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action,
    bool delete_id)
{
    /* Cleanup can happen recursively. If a cleanup action is already in 
     * progress, only append ids to the marked_ids. The topmost cleanup
     * frame will handle the actual cleanup. */
    int32_t count = ecs_vec_count(&world->store.marked_ids);

    /* Make sure we're evaluating a consistent list of non-empty tables */
    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    /* Collect all ids that need to be deleted */
    flecs_on_delete_mark(world, id, action, delete_id);

    /* Only perform cleanup if we're the first stack frame doing it */
    if (!count && ecs_vec_count(&world->store.marked_ids)) {
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
        ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
        int32_t i; count = ecs_vec_count(&world->store.marked_ids);
        for (i = 0; i < count; i ++) {
            ecs_assert(!ecs_id_in_use(world, ids[i].id), 
                ECS_INTERNAL_ERROR, NULL);
        }
#endif
        ecs_assert(!ecs_id_in_use(world, id), ECS_INTERNAL_ERROR, NULL);

        /* Ids are deleted, clear stack */
        ecs_vec_clear(&world->store.marked_ids);

        ecs_log_pop_2();
    }
}

void ecs_delete_with(
    ecs_world_t *world,
    ecs_id_t id)
{
    flecs_journal_begin(world, EcsJournalDeleteWith, id, NULL, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(stage, id, EcsDelete)) {
        return;
    }

    flecs_on_delete(world, id, EcsDelete, false);
    flecs_defer_end(world, stage);

    flecs_journal_end();
}

void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t id)
{
    flecs_journal_begin(world, EcsJournalRemoveAll, id, NULL, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(stage, id, EcsRemove)) {
        return;
    }

    flecs_on_delete(world, id, EcsRemove, false);
    flecs_defer_end(world, stage);

    flecs_journal_end();
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_delete(stage, entity)) {
        return;
    }

    ecs_record_t *r = flecs_entities_try(world, entity);
    if (r) {
        flecs_journal_begin(world, EcsJournalDelete, entity, NULL, NULL);

        ecs_flags32_t row_flags = ECS_RECORD_TO_ROW_FLAGS(r->row);
        ecs_table_t *table;
        if (row_flags) {
            if (row_flags & EcsEntityIsTarget) {
                flecs_on_delete(world, ecs_pair(EcsFlag, entity), 0, true);
                flecs_on_delete(world, ecs_pair(EcsWildcard, entity), 0, true);
                r->idr = NULL;
            }
            if (row_flags & EcsEntityIsId) {
                flecs_on_delete(world, entity, 0, true);
                flecs_on_delete(world, ecs_pair(entity, EcsWildcard), 0, true);
            }
            if (row_flags & EcsEntityIsTraversable) {
                table = r->table;
                if (table) {
                    flecs_table_traversable_add(table, -1);
                }
            }
            /* Merge operations before deleting entity */
            flecs_defer_end(world, stage);
            flecs_defer_begin(world, stage);
        }

        table = r->table;

        if (table) {
            ecs_table_diff_t diff = {
                .removed = table->type
            };

            flecs_delete_entity(world, r, &diff);

            r->row = 0;
            r->table = NULL;
        }
        
        flecs_entities_remove(world, entity);

        flecs_journal_end();
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
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    flecs_add_id(world, entity, id);
error:
    return;
}

void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id) || ecs_id_is_wildcard(id), 
        ECS_INVALID_PARAMETER, NULL);
    flecs_remove_id(world, entity, id);
error:
    return;
}

void ecs_auto_override_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_add_id(world, entity, ECS_AUTO_OVERRIDE | id);
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
    ecs_check(ecs_is_alive(world, src), ECS_INVALID_PARAMETER, NULL);
    ecs_check(!dst || !ecs_get_table(world, dst), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (!dst) {
        dst = ecs_new(world);
    }

    if (flecs_defer_clone(stage, dst, src, copy_value)) {
        return dst;
    }

    ecs_record_t *src_r = flecs_entities_get(world, src);
    ecs_assert(src_r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *src_table = src_r->table;
    if (!src_table) {
        goto done;
    }

    ecs_table_t *dst_table = src_table;
    if (src_table->flags & EcsTableHasName) {
        dst_table = ecs_table_remove_id(world, src_table, 
            ecs_pair_t(EcsIdentifier, EcsName));
    }

    ecs_type_t dst_type = dst_table->type;
    ecs_table_diff_t diff = { .added = dst_type };
    ecs_record_t *dst_r = flecs_entities_get(world, dst);
    flecs_new_entity(world, dst, dst_r, dst_table, &diff, true, 0);
    int32_t row = ECS_RECORD_TO_ROW(dst_r->row);

    if (copy_value) {
        flecs_table_move(world, dst, src, dst_table,
            row, src_table, ECS_RECORD_TO_ROW(src_r->row), true);
        int32_t i, count = dst_table->column_count;
        for (i = 0; i < count; i ++) {
            ecs_type_t type = {
                .array = &dst_table->data.columns[i].id,
                .count = 1
            };
            flecs_notify_on_set(world, dst_table, row, 1, &type, true);
        }
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
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return NULL;
    }

    const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr) {
        return flecs_get_base_component(world, table, id, idr, 0);
    } else {
        if (idr->flags & EcsIdIsSparse) {
            return flecs_sparse_get_any(idr->sparse, 0, entity);
        }
        ecs_check(tr->column != -1, ECS_NOT_A_COMPONENT, NULL);
    }

    int32_t row = ECS_RECORD_TO_ROW(r->row);
    return flecs_table_get_component(table, tr->column, row).ptr;
error:
    return NULL;
}

void* ecs_get_mut_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    int32_t row = ECS_RECORD_TO_ROW(r->row);
    return flecs_get_component_ptr(table, row, idr).ptr;
error:
    return NULL;
}

void* ecs_ensure_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_cmd(stage)) {
        return flecs_defer_set(
            world, stage, EcsCmdEnsure, entity, id, 0, NULL, NULL);
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    void *result = flecs_ensure(world, entity, id, r).ptr;
    ecs_check(result != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_defer_end(world, stage);
    return result;
error:
    return NULL;
}

void* ecs_ensure_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(flecs_defer_cmd(stage), ECS_INVALID_PARAMETER, NULL);

    return flecs_defer_set(world, stage, EcsCmdSet, entity, id, 0, NULL, NULL);
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
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table;
    if (!(table = r->table)) {
        return NULL;
    }

    int32_t count = ecs_os_ainc(&table->_->lock);
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
    int32_t count = ecs_os_adec(&r->table->_->lock);
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

ecs_entity_t ecs_record_get_entity(
    const ecs_record_t *record)
{
    ecs_check(record != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_table_t *table = record->table;
    if (!table) {
        return 0;
    }

    return ecs_vec_get_t(&table->data.entities, ecs_entity_t, 
        ECS_RECORD_TO_ROW(record->row))[0];
error:
    return 0;
}

const void* ecs_record_get_id(
    const ecs_world_t *stage,
    const ecs_record_t *r,
    ecs_id_t id)
{
    const ecs_world_t *world = ecs_get_world(stage);
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    return flecs_get_component(r->table, ECS_RECORD_TO_ROW(r->row), idr);
}

bool ecs_record_has_id(
    ecs_world_t *stage,
    const ecs_record_t *r,
    ecs_id_t id)
{
    const ecs_world_t *world = ecs_get_world(stage);
    if (r->table) {
        return ecs_table_has_id(world, r->table, id);
    }
    return false;
}

void* ecs_record_ensure_id(
    ecs_world_t *stage,
    ecs_record_t *r,
    ecs_id_t id)
{
    const ecs_world_t *world = ecs_get_world(stage);
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    return flecs_get_component(r->table, ECS_RECORD_TO_ROW(r->row), idr);
}

ecs_ref_t ecs_ref_init_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
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
        result.tr = flecs_table_record_get(world, table, id);
        result.table_id = table->id;
    }

    return result;
error:
    return (ecs_ref_t){0};
}

static
bool flecs_ref_needs_sync(
    ecs_ref_t *ref,
    ecs_table_record_t *tr,
    const ecs_table_t *table)
{
    ecs_assert(ref != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return !tr || ref->table_id != table->id || tr->hdr.table != table;
}

void ecs_ref_update(
    const ecs_world_t *world,
    ecs_ref_t *ref)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->record != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_record_t *r = ref->record;
    ecs_table_t *table = r->table;
    if (!table) {
        return;
    }

    ecs_table_record_t *tr = ref->tr;
    if (flecs_ref_needs_sync(ref, tr, table)) {
        tr = ref->tr = flecs_table_record_get(world, table, ref->id);
        if (!tr) {
            return;
        }

        ecs_assert(tr->hdr.table == r->table, ECS_INTERNAL_ERROR, NULL);
        ref->table_id = table->id;
    }
error:
    return;
}

void* ecs_ref_get_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(ref->id != 0, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(ref->record != NULL, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(id == ref->id, ECS_INVALID_PARAMETER, "ref not initialized");

    ecs_record_t *r = ref->record;
    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    int32_t row = ECS_RECORD_TO_ROW(r->row);
    ecs_check(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);

    ecs_table_record_t *tr = ref->tr;
    if (flecs_ref_needs_sync(ref, tr, table)) {
        tr = ref->tr = flecs_table_record_get(world, table, id);
        if (!tr) {
            return NULL;
        }

        ref->table_id = table->id;
        ecs_assert(tr->hdr.table == r->table, ECS_INTERNAL_ERROR, NULL);
    }

    int32_t column = tr->column;
    if (column == -1) {
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        if (idr->flags & EcsIdIsSparse) {
            return flecs_sparse_get_any(idr->sparse, 0, ref->entity);
        }
    }
    return flecs_table_get_component(table, column, row).ptr;
error:
    return NULL;
}

void* ecs_emplace_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_new)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_check(is_new || !ecs_has_id(world, entity, id), ECS_INVALID_PARAMETER, 
        "cannot emplace a component the entity already has");

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_cmd(stage)) {
        return flecs_defer_set(
            world, stage, EcsCmdEmplace, entity, id, 0, NULL, is_new);
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = r->table;
    flecs_add_id_w_record(world, entity, r, id, false /* Add without ctor */);
    flecs_defer_end(world, stage);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    void *ptr = flecs_get_component(r->table, ECS_RECORD_TO_ROW(r->row), idr);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, 
        "emplaced component was removed during operation, make sure to not "
        "remove component T in on_add(T) hook/OnAdd(T) observer");

    if (is_new) {
        *is_new = table != r->table;
    }

    return ptr;
error:
    return NULL;
}

static
void flecs_modified_id_if(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool owned)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_modified(stage, entity, id)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = r->table;
    if (!flecs_table_record_get(world, table, id)) {
        flecs_defer_end(world, stage);
        return;
    }

    ecs_type_t ids = { .array = &id, .count = 1 };
    flecs_notify_on_set(world, table, ECS_RECORD_TO_ROW(r->row), 1, &ids, owned);

    flecs_table_mark_dirty(world, table, id);
    flecs_defer_end(world, stage);
error:
    return;
}

void ecs_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_modified(stage, entity, id)) {
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
void flecs_set_id_copy(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    void *ptr)
{
    if (flecs_defer_cmd(stage)) {
        flecs_defer_set(world, stage, EcsCmdSet, entity, id, 
            flecs_utosize(size), ptr, NULL);
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_ensure(world, entity, id, r);

    flecs_copy_id(world, r, id, size, dst.ptr, ptr, dst.ti);

    flecs_defer_end(world, stage);
}

static
void flecs_set_id_move(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    void *ptr,
    ecs_cmd_kind_t cmd_kind)
{
    if (flecs_defer_cmd(stage)) {
        flecs_defer_set(world, stage, cmd_kind, entity, id, 
            flecs_utosize(size), ptr, NULL);
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_ensure(world, entity, id, r);
    ecs_check(dst.ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_type_info_t *ti = dst.ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_move_t move;
    if (cmd_kind != EcsCmdEmplace) {
        /* ctor will have happened by ensure */
        move = ti->hooks.move_dtor;
    } else {
        move = ti->hooks.ctor_move_dtor;
    }
    if (move) {
        move(dst.ptr, ptr, 1, ti);
    } else {
        ecs_os_memcpy(dst.ptr, ptr, flecs_utosize(size));
    }

    flecs_table_mark_dirty(world, r->table, id);

    if (cmd_kind == EcsCmdSet) {
        ecs_table_t *table = r->table;
        if (table->flags & EcsTableHasOnSet || ti->hooks.on_set) {
            ecs_type_t ids = { .array = &id, .count = 1 };
            flecs_notify_on_set(
                world, table, ECS_RECORD_TO_ROW(r->row), 1, &ids, true);
        }
    }

    flecs_defer_end(world, stage);
error:
    return;
}

void ecs_set_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    /* Safe to cast away const: function won't modify if move arg is false */
    flecs_set_id_copy(world, stage, entity, id, size, 
        ECS_CONST_CAST(void*, ptr));
error:
    return;
}

#if defined(FLECS_DEBUG) || defined(FLECS_KEEP_ASSERT)
static
bool flecs_can_toggle(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return false;
    }

    return (idr->flags & EcsIdCanToggle) != 0;
}
#endif

void ecs_enable_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_check(flecs_can_toggle(world, id), ECS_INVALID_OPERATION, 
        "add CanToggle trait to component");

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_enable(stage, entity, id, enable)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_entity_t bs_id = id | ECS_TOGGLE;
    
    ecs_table_t *table = r->table;
    int32_t index = -1;
    if (table) {
        index = ecs_table_get_type_index(world, table, bs_id);
    }

    if (index == -1) {
        ecs_add_id(world, entity, bs_id);
        flecs_defer_end(world, stage);
        ecs_enable_id(world, entity, id, enable);
        return;
    }
    
    ecs_assert(table->_ != NULL, ECS_INTERNAL_ERROR, NULL);
    index -= table->_->bs_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULL, since entity is stored in the table */
    ecs_bitset_t *bs = &table->_->bs_columns[index];
    ecs_assert(bs != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_bitset_set(bs, ECS_RECORD_TO_ROW(r->row), enable);

    flecs_defer_end(world, stage);
error:
    return;
}

bool ecs_is_enabled_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    if (!table) {
        return false;
    }

    ecs_entity_t bs_id = id | ECS_TOGGLE;
    int32_t index = ecs_table_get_type_index(world, table, bs_id);
    if (index == -1) {
        /* If table does not have TOGGLE column for component, component is
         * always enabled, if the entity has it */
        return ecs_has_id(world, entity, id);
    }

    ecs_assert(table->_ != NULL, ECS_INTERNAL_ERROR, NULL);
    index -= table->_->bs_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &table->_->bs_columns[index];

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
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get_any(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    if (!table) {
        return false;
    }

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    int32_t column;
    if (idr) {
        ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
        if (tr) {
            return true;
        }
    }

    if (!(table->flags & (EcsTableHasUnion|EcsTableHasIsA))) {
        return false;
    }

    if (ECS_IS_PAIR(id) && (table->flags & EcsTableHasUnion)) {
        ecs_id_record_t *u_idr = flecs_id_record_get(world, 
            ecs_pair(ECS_PAIR_FIRST(id), EcsUnion));
        if (u_idr && u_idr->flags & EcsIdIsUnion) {
            uint64_t cur = flecs_switch_get(u_idr->sparse, (uint32_t)entity);
            return (uint32_t)cur == ECS_PAIR_SECOND(id);
        }
    }

    ecs_table_record_t *tr;
    column = ecs_search_relation(world, table, 0, id, 
        EcsIsA, 0, 0, 0, &tr);
    if (column == -1) {
        return false;
    }

    return true;
error:
    return false;
}

bool ecs_owns_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return (ecs_search(world, ecs_get_table(world, entity), id, 0) != -1);
}

ecs_entity_t ecs_get_target(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    int32_t index)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(rel != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    if (!table) {
        goto not_found;
    }

    ecs_id_t wc = ecs_pair(rel, EcsWildcard);
    ecs_id_record_t *idr = flecs_id_record_get(world, wc);
    const ecs_table_record_t *tr = NULL;
    if (idr) {
        tr = flecs_id_record_get_table(idr, table);
    }
    if (!tr) {
        if (!idr || (idr->flags & EcsIdOnInstantiateInherit)) {
            goto look_in_base;
        } else {
            return 0;
        }
    }

    if (index >= tr->count) {
        index -= tr->count;
        goto look_in_base;
    }

    ecs_entity_t result = 
        ecs_pair_second(world, table->type.array[tr->index + index]);

    if (result == EcsUnion) {
        wc = ecs_pair(rel, EcsUnion);
        ecs_id_record_t *wc_idr = flecs_id_record_get(world, wc);
        ecs_assert(wc_idr != NULL, ECS_INTERNAL_ERROR, NULL);
        result = flecs_switch_get(wc_idr->sparse, (uint32_t)entity);
    }

    return result;
look_in_base:
    if (table->flags & EcsTableHasIsA) {
        ecs_table_record_t *tr_isa = flecs_id_record_get_table(
            world->idr_isa_wildcard, table);
        ecs_assert(tr_isa != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_id_t *ids = table->type.array;
        int32_t i = tr_isa->index, end = (i + tr_isa->count);
        for (; i < end; i ++) {
            ecs_id_t isa_pair = ids[i];
            ecs_entity_t base = ecs_pair_second(world, isa_pair);
            ecs_assert(base != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_entity_t t = ecs_get_target(world, base, rel, index);
            if (t) {
                return t;
            }
        }
    }

not_found:
error:
    return 0;
}

ecs_entity_t ecs_get_parent(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    return ecs_get_target(world, entity, EcsChildOf, 0);
}

ecs_entity_t ecs_get_target_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    if (!id) {
        return ecs_get_target(world, entity, rel, 0);
    }

    world = ecs_get_world(world);

    ecs_table_t *table = ecs_get_table(world, entity);
    ecs_entity_t subject = 0;

    if (rel) {
        int32_t column = ecs_search_relation(
            world, table, 0, id, rel, 0, &subject, 0, 0);
        if (column == -1) {
            return 0;
        }
    } else {
        entity = 0; /* Don't return entity if id was not found */

        if (table) {
            ecs_id_t *ids = table->type.array;
            int32_t i, count = table->type.count;

            for (i = 0; i < count; i ++) {
                ecs_id_t ent = ids[i];
                if (ent & ECS_ID_FLAGS_MASK) {
                    /* Skip ids with pairs, roles since 0 was provided for rel */
                    break;
                }

                if (ecs_has_id(world, ent, id)) {
                    subject = ent;
                    break;
                }
            }
        }
    }

    if (subject == 0) {
        return entity;
    } else {
        return subject;
    }
error:
    return 0;
}

int32_t ecs_get_depth(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel)
{
    ecs_check(ecs_is_valid(world, rel), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_has_id(world, rel, EcsAcyclic), ECS_INVALID_PARAMETER, 
        "cannot safely determine depth for relationship that is not acyclic "
            "(add Acyclic property to relationship)");

    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
        return ecs_table_get_depth(world, table, rel);
    }

    return 0;
error:
    return -1;
}

static
const char* flecs_get_identifier(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

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
    return flecs_get_identifier(world, entity, EcsName);
}

const char* ecs_get_symbol(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    world = ecs_get_world(world);
    if (ecs_owns_pair(world, entity, ecs_id(EcsIdentifier), EcsSymbol)) {
        return flecs_get_identifier(world, entity, EcsSymbol);
    } else {
        return NULL;
    }
}

static
ecs_entity_t flecs_set_identifier(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t tag,
    const char *name)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0 || name != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        entity = ecs_new(world);
    }

    if (!name) {
        ecs_remove_pair(world, entity, ecs_id(EcsIdentifier), tag);
        return entity;
    }

    EcsIdentifier *ptr = ecs_ensure_pair(world, entity, EcsIdentifier, tag);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (tag == EcsName) {
        /* Insert command after ensure, but before the name is potentially 
         * freed. Even though the name is a const char*, it is possible that the
         * application passed in the existing name of the entity which could 
         * still cause it to be freed. */
        flecs_defer_path(stage, 0, entity, name);
    }

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
        return ecs_entity(world, {
            .name = name
        });
    }

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    flecs_set_identifier(world, stage, entity, EcsName, name);

    return entity;
}

ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    return flecs_set_identifier(world, NULL, entity, EcsSymbol, name);
}

void ecs_set_alias(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    flecs_set_identifier(world, NULL, entity, EcsAlias, name);
}

ecs_id_t ecs_make_pair(
    ecs_entity_t relationship,
    ecs_entity_t target)
{
    ecs_assert(!ECS_IS_PAIR(relationship) && !ECS_IS_PAIR(target), 
        ECS_INVALID_PARAMETER, "cannot create nested pairs");
    return ecs_pair(relationship, target);
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
    
    /* Entity identifiers should not contain flag bits */
    if (entity & ECS_ID_FLAGS_MASK) {
        return false;
    }

    /* Entities should not contain data in dead zone bits */
    if (entity & ~0xFF00FFFFFFFFFFFF) {
        return false;
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
    if (!(e & ECS_ID_FLAGS_MASK)) {
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

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    if (flecs_entities_is_alive(world, entity)) {
        return entity;
    }

    /* Make sure id does not have generation. This guards against accidentally
     * "upcasting" a not alive identifier to an alive one. */
    if ((uint32_t)entity != entity) {
        return 0;
    }

    ecs_entity_t current = flecs_entities_get_alive(world, entity);
    if (!current || !flecs_entities_is_alive(world, current)) {
        return 0;
    }

    return current;
error:
    return 0;
}

void ecs_make_alive(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    /* Const cast is safe, function checks for threading */
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    /* The entity index can be mutated while in staged/readonly mode, as long as
     * the world is not multithreaded. */
    ecs_assert(!(world->flags & EcsWorldMultiThreaded), 
        ECS_INVALID_OPERATION, 
            "cannot make entity alive while world is in multithreaded mode");

    /* Check if a version of the provided id is alive */
    ecs_entity_t any = ecs_get_alive(world, (uint32_t)entity);
    if (any == entity) {
        /* If alive and equal to the argument, there's nothing left to do */
        return;
    }

    /* If the id is currently alive but did not match the argument, fail */
    ecs_check(!any, ECS_INVALID_PARAMETER, 
        "entity is alive with different generation");

    /* Set generation if not alive. The sparse set checks if the provided
     * id matches its own generation which is necessary for alive ids. This
     * check would cause ecs_ensure to fail if the generation of the 'entity'
     * argument doesn't match with its generation.
     * 
     * While this could've been addressed in the sparse set, this is a rare
     * scenario that can only be triggered by ecs_ensure. Implementing it here
     * allows the sparse set to not do this check, which is more efficient. */
    flecs_entities_make_alive(world, entity);

    /* Ensure id exists. The underlying data structure will verify that the
     * generation count matches the provided one. */
    flecs_entities_ensure(world, entity);
error:
    return;
}

void ecs_make_alive_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    flecs_poly_assert(world, ecs_world_t);
    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_entity_t r = ECS_PAIR_FIRST(id);
        ecs_entity_t o = ECS_PAIR_SECOND(id);

        ecs_check(r != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_check(o != 0, ECS_INVALID_PARAMETER, NULL);

        if (flecs_entities_get_alive(world, r) == 0) {
            ecs_assert(!ecs_exists(world, r), ECS_INVALID_PARAMETER, 
                "first element of pair is not alive");
            flecs_entities_ensure(world, r);
        }
        if (flecs_entities_get_alive(world, o) == 0) {
            ecs_assert(!ecs_exists(world, o), ECS_INVALID_PARAMETER,
                "second element of pair is not alive");
            flecs_entities_ensure(world, o);
        }
    } else {
        flecs_entities_ensure(world, id & ECS_COMPONENT_MASK);
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

void ecs_set_version(
    ecs_world_t *world,
    ecs_entity_t entity_with_generation)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION,
        "cannot change entity generation when world is in readonly mode");
    ecs_assert(!(ecs_is_deferred(world)), ECS_INVALID_OPERATION, 
        "cannot change entity generation while world is deferred");

    flecs_entities_make_alive(world, entity_with_generation);

    ecs_record_t *r = flecs_entities_get(world, entity_with_generation);
    if (r && r->table) {
        int32_t row = ECS_RECORD_TO_ROW(r->row);
        ecs_entity_t *entities = r->table->data.entities.array;
        entities[row] = entity_with_generation;
    }
}

ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    
    world = ecs_get_world(world);

    ecs_record_t *record = flecs_entities_get(world, entity);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    return record->table;
error:
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
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr && ECS_IS_PAIR(id)) {
        idr = flecs_id_record_get(world, 
            ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard));
        if (!idr || !idr->type_info) {
            idr = NULL;
        }
        if (!idr) {
            ecs_entity_t first = ecs_pair_first(world, id);
            if (!first || !ecs_has_id(world, first, EcsPairIsTag)) {
                idr = flecs_id_record_get(world, 
                    ecs_pair(EcsWildcard, ECS_PAIR_SECOND(id)));
                if (!idr || !idr->type_info) {
                    idr = NULL;
                }
            }
        }
    }

    if (idr) {
        return idr->type_info;
    } else if (!(id & ECS_ID_FLAGS_MASK)) {
        return flecs_type_info_get(world, id);
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
        ecs_assert(ti->component != 0, ECS_INTERNAL_ERROR, NULL);
        return ti->component;
    }
error:
    return 0;
}

bool ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (ecs_id_is_wildcard(id)) {
        /* If id is a wildcard, we can't tell if it's a tag or not, except
         * when the relationship part of a pair has the Tag property */
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            if (ECS_PAIR_FIRST(id) != EcsWildcard) {
                ecs_entity_t rel = ecs_pair_first(world, id);
                if (ecs_is_valid(world, rel)) {
                    if (ecs_has_id(world, rel, EcsPairIsTag)) {
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
                /* If relationship is wildcard id is not guaranteed to be a tag */
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
    ecs_iter_t it = ecs_each_id(world, id);
    while (ecs_each_next(&it)) {
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
            if (!(ecs_id_get_flags(world, id) & EcsIdOnInstantiateDontInherit)){
                ecs_enable(world, id, enabled);
            }
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
    ecs_check(ecs_is_deferred(world), ECS_INVALID_OPERATION, 
        "world/stage must be deferred before it can be suspended");
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(stage->defer > 0, ECS_INVALID_OPERATION, 
        "world/stage is already suspended");
    stage->defer = -stage->defer;
error:
    return;
}

void ecs_defer_resume(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(stage->defer < 0, ECS_INVALID_OPERATION,
        "world/stage must be suspended before it can be resumed");
    stage->defer = -stage->defer;
error:
    return;
}

const char* ecs_id_flag_str(
    ecs_entity_t entity)
{
    if (ECS_HAS_ID_FLAG(entity, PAIR)) {
        return "PAIR";
    } else
    if (ECS_HAS_ID_FLAG(entity, TOGGLE)) {
        return "TOGGLE";
    } else
    if (ECS_HAS_ID_FLAG(entity, AUTO_OVERRIDE)) {
        return "AUTO_OVERRIDE";
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

    if (ECS_HAS_ID_FLAG(id, TOGGLE)) {
        ecs_strbuf_appendstr(buf, ecs_id_flag_str(ECS_TOGGLE));
        ecs_strbuf_appendch(buf, '|');
    }

    if (ECS_HAS_ID_FLAG(id, AUTO_OVERRIDE)) {
        ecs_strbuf_appendstr(buf, ecs_id_flag_str(ECS_AUTO_OVERRIDE));
        ecs_strbuf_appendch(buf, '|');
    }

    if (ECS_HAS_ID_FLAG(id, PAIR)) {
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

static
void ecs_type_str_buf(
    const ecs_world_t *world,
    const ecs_type_t *type,
    ecs_strbuf_t *buf)
{
    ecs_entity_t *ids = type->array;
    int32_t i, count = type->count;

    for (i = 0; i < count; i ++) {
        ecs_entity_t id = ids[i];

        if (i) {
            ecs_strbuf_appendch(buf, ',');
            ecs_strbuf_appendch(buf, ' ');
        }

        if (id == 1) {
            ecs_strbuf_appendlit(buf, "Component");
        } else {
            ecs_id_str_buf(world, id, buf);
        }
    }
}

char* ecs_type_str(
    const ecs_world_t *world,
    const ecs_type_t *type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_type_str_buf(world, type, &buf);
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

char* ecs_entity_str(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_get_path_w_sep_buf(world, 0, entity, 0, "", &buf);
    
    ecs_strbuf_appendlit(&buf, " [");
    const ecs_type_t *type = ecs_get_type(world, entity);
    if (type) {
        ecs_type_str_buf(world, type, &buf);
    }
    ecs_strbuf_appendch(&buf, ']');

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

static
void flecs_flush_bulk_new(
    ecs_world_t *world,
    ecs_cmd_t *cmd)
{
    ecs_entity_t *entities = cmd->is._n.entities;

    if (cmd->id) {
        int i, count = cmd->is._n.count;
        for (i = 0; i < count; i ++) {
            flecs_entities_ensure(world, entities[i]);
            flecs_add_id(world, entities[i], cmd->id);
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
void flecs_free_cmd_event(
    ecs_world_t *world,
    ecs_event_desc_t *desc)
{
    if (desc->ids) {
        flecs_stack_free_n(desc->ids->array, ecs_id_t, desc->ids->count);
    }

    /* Safe const cast, command makes a copy of type object */
    flecs_stack_free_t(ECS_CONST_CAST(ecs_type_t*, desc->ids), 
        ecs_type_t);

    if (desc->param) {
        flecs_dtor_value(world, desc->event, 
            /* Safe const cast, command makes copy of value */
            ECS_CONST_CAST(void*, desc->param), 1);
    }
}

static
void flecs_discard_cmd(
    ecs_world_t *world,
    ecs_cmd_t *cmd)
{
    if (cmd->kind == EcsCmdBulkNew) {
        ecs_os_free(cmd->is._n.entities);
    } else if (cmd->kind == EcsCmdEvent) {
        flecs_free_cmd_event(world, cmd->is._1.value);
    } else {
        ecs_assert(cmd->kind != EcsCmdEvent, ECS_INTERNAL_ERROR, NULL);
        void *value = cmd->is._1.value;
        if (value) {
            flecs_dtor_value(world, cmd->id, value, 1);
            flecs_stack_free(value, cmd->is._1.size);
        }
    }
}

static
bool flecs_remove_invalid(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        if (!flecs_entities_is_valid(world, rel)) {
            /* After relationship is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        } else {
            ecs_entity_t obj = ECS_PAIR_SECOND(id);
            if (!flecs_entities_is_valid(world, obj)) {
                /* Check the relationship's policy for deleted objects */
                ecs_id_record_t *idr = flecs_id_record_get(world, 
                    ecs_pair(rel, EcsWildcard));
                if (idr) {
                    ecs_entity_t action = ECS_ID_ON_DELETE_TARGET(idr->flags);
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
        if (!flecs_entities_is_valid(world, id)) {
            /* After relationship is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        }
    }

    return true;
}

static
ecs_table_t* flecs_cmd_batch_add_diff(
    ecs_world_t *world,
    ecs_table_t *dst,
    ecs_table_t *cur,
    ecs_table_t *prev)
{
    int32_t p = 0, p_count = prev->type.count;
    int32_t c = 0, c_count = cur->type.count;
    ecs_id_t *p_ids = prev->type.array;
    ecs_id_t *c_ids = cur->type.array;

    for (; c < c_count;) {
        ecs_id_t c_id = c_ids[c];
        ecs_id_t p_id = p_ids[p];

        if (p_id < c_id) {
            /* Previous id no longer exists in new table, so it was removed */
            dst = ecs_table_remove_id(world, dst, p_id);
        }
        if (c_id < p_id) {
            /* Current id didn't exist in previous table, so it was added */
            dst = ecs_table_add_id(world, dst, c_id);
        }

        c += c_id <= p_id;
        p += p_id <= c_id;
        if (p == p_count) {
            break;
        }
    }

    /* Remainder */
    for (; p < p_count; p ++) {
        ecs_id_t p_id = p_ids[p];
        dst = ecs_table_remove_id(world, dst, p_id);
    }
    for (; c < c_count; c ++) {
        ecs_id_t c_id = c_ids[c];
        dst = ecs_table_add_id(world, dst, c_id);
    }

    return dst;
}

static
void flecs_cmd_batch_for_entity(
    ecs_world_t *world,
    ecs_table_diff_builder_t *diff,
    ecs_entity_t entity,
    ecs_cmd_t *cmds,
    int32_t start)
{
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = NULL;
    if (r) {
        table = r->table;
    }

    world->info.cmd.batched_entity_count ++;

    ecs_table_t *start_table = table;
    ecs_cmd_t *cmd;
    int32_t next_for_entity;
    ecs_table_diff_t table_diff; /* Keep track of diff for observers/hooks */
    int32_t cur = start;
    ecs_id_t id;

    /* Mask to let observable implementation know which components were set.
     * This prevents the code from overwriting components with an override if
     * the batch also contains an IsA pair. */
    ecs_flags64_t set_mask = 0;

    do {
        cmd = &cmds[cur];
        id = cmd->id;
        next_for_entity = cmd->next_for_entity;
        if (next_for_entity < 0) {
            /* First command for an entity has a negative index, flip sign */
            next_for_entity *= -1;
        }

        /* Check if added id is still valid (like is the parent of a ChildOf 
         * pair still alive), if not run cleanup actions for entity */
        if (id) {
            if (flecs_remove_invalid(world, id, &id)) {
                if (!id) {
                    /* Entity should remain alive but id should not be added */
                    cmd->kind = EcsCmdSkip;
                    continue;
                }
                /* Entity should remain alive and id is still valid */
            } else {
                /* Id was no longer valid and had a Delete policy */
                cmd->kind = EcsCmdSkip;
                ecs_delete(world, entity);
                flecs_table_diff_builder_clear(diff);
                return;
            }
        }

        ecs_cmd_kind_t kind = cmd->kind;
        switch(kind) {
        case EcsCmdAddModified:
            /* Add is batched, but keep Modified */
            cmd->kind = EcsCmdModified;

            /* fall through */
        case EcsCmdAdd:
            table = flecs_find_table_add(world, table, id, diff);
            world->info.cmd.batched_command_count ++;
            break;
        case EcsCmdModified:
            if (start_table) {
                /* If a modified was inserted for an existing component, the value
                 * of the component could have been changed. If this is the case,
                 * call on_set hooks before the OnAdd/OnRemove observers are invoked
                 * when moving the entity to a different table.
                 * This ensures that if OnAdd/OnRemove observers access the modified
                 * component value, the on_set hook has had the opportunity to
                 * run first to set any computed values of the component. */
                int32_t row = ECS_RECORD_TO_ROW(r->row);
                ecs_id_record_t *idr = flecs_id_record_get(world, cmd->id);
                flecs_component_ptr_t ptr = flecs_get_component_ptr(
                    start_table, row, idr);
                if (ptr.ptr) {
                    const ecs_type_info_t *ti = ptr.ti;
                    ecs_iter_action_t on_set;
                    if ((on_set = ti->hooks.on_set)) {
                        ecs_table_t *prev_table = r->table;
                        ecs_defer_begin(world);
                        flecs_invoke_hook(world, start_table, 1, row, &entity,
                            ptr.ptr, cmd->id, ptr.ti, EcsOnSet, on_set);
                        ecs_defer_end(world);

                        /* Don't run on_set hook twice, but make sure to still
                         * invoke observers. */
                        cmd->kind = EcsCmdModifiedNoHook;

                        /* If entity changed tables in hook, add difference to
                         * destination table, so we don't lose the side effects
                         * of the hook. */
                        if (r->table != prev_table) {
                            table = flecs_cmd_batch_add_diff(
                                world, table, r->table, prev_table);
                        }
                    }
                }
            }
            break;
        case EcsCmdSet:
        case EcsCmdEnsure: {
            ecs_id_t *ids = diff->added.array;
            uint8_t added_index = flecs_ito(uint8_t, diff->added.count);

            table = flecs_find_table_add(world, table, id, diff);

            if (diff->added.count == (added_index + 1)) {
                /* Single id was added, must be at the end of the array */
                ecs_assert(ids[added_index] == id, ECS_INTERNAL_ERROR, NULL);
                set_mask |= (1llu << added_index);
            } else {
                /* Id was already added or multiple ids got added. Do a linear
                 * search to find the index we need to set the set_mask. */
                int32_t i;
                for (i = 0; i < diff->added.count; i ++) {
                    if (ids[i] == id) {
                        break;
                    }
                }

                ecs_assert(i != diff->added.count, ECS_INTERNAL_ERROR, NULL);
                set_mask |= (1llu << i);
            }

            world->info.cmd.batched_command_count ++;
            break;
        }
        case EcsCmdEmplace:
            /* Don't add for emplace, as this requires a special call to ensure
             * the constructor is not invoked for the component */
            break;
        case EcsCmdRemove:
            table = flecs_find_table_remove(world, table, id, diff);
            world->info.cmd.batched_command_count ++;
            break;
        case EcsCmdClear:
            if (table) {
                ecs_id_t *ids = ecs_vec_grow_t(&world->allocator, 
                    &diff->removed, ecs_id_t, table->type.count);
                ecs_os_memcpy_n(ids, table->type.array, ecs_id_t, 
                    table->type.count);
            }
            table = &world->store.root;
            world->info.cmd.batched_command_count ++;
            break;
        case EcsCmdClone:
        case EcsCmdBulkNew:
        case EcsCmdPath:
        case EcsCmdDelete:
        case EcsCmdOnDeleteAction:
        case EcsCmdEnable:
        case EcsCmdDisable:
        case EcsCmdEvent:
        case EcsCmdSkip:
        case EcsCmdModifiedNoHook:
            break;
        }

        /* Add, remove and clear operations can be skipped since they have no
         * side effects besides adding/removing components */
        if (kind == EcsCmdAdd || kind == EcsCmdRemove || kind == EcsCmdClear) {
            cmd->kind = EcsCmdSkip;
        }
    } while ((cur = next_for_entity));

    /* Invoke OnAdd handlers after commit. This ensures that observers with 
     * mixed OnAdd/OnSet events won't get called with uninitialized values for
     * an OnSet field. */
    ecs_type_t added = { diff->added.array, diff->added.count };
    diff->added.array = NULL;
    diff->added.count = 0;

    /* Move entity to destination table in single operation */
    flecs_table_diff_build_noalloc(diff, &table_diff);
    flecs_defer_begin(world, world->stages[0]);
    flecs_commit(world, entity, r, table, &table_diff, true, 0);
    flecs_defer_end(world, world->stages[0]);

    /* If the batch contains set commands, copy the component value from the 
     * temporary command storage to the actual component storage before OnSet
     * observers are invoked. This ensures that for multi-component OnSet 
     * observers all components are assigned a valid value before the observer
     * is invoked. 
     * This only happens for entities that didn't have the assigned component
     * yet, as for entities that did have the component already the value will
     * have been assigned directly to the component storage. */
    if (set_mask) {
        cur = start;
        do {
            cmd = &cmds[cur];
            next_for_entity = cmd->next_for_entity;
            if (next_for_entity < 0) {
                next_for_entity *= -1;
            }

            switch(cmd->kind) {
            case EcsCmdSet:
            case EcsCmdEnsure: {
                flecs_component_ptr_t ptr = {0};
                if (r->table) {
                    ecs_id_record_t *idr = flecs_id_record_get(world, cmd->id);
                    ptr = flecs_get_component_ptr(
                        r->table, ECS_RECORD_TO_ROW(r->row), idr);
                }

                /* It's possible that even though the component was set, the
                 * command queue also contained a remove command, so before we
                 * do anything ensure the entity actually has the component. */
                if (ptr.ptr) {
                    const ecs_type_info_t *ti = ptr.ti;
                    ecs_move_t move = ti->hooks.move;
                    if (move) {
                        move(ptr.ptr, cmd->is._1.value, 1, ti);
                        ecs_xtor_t dtor = ti->hooks.dtor;
                        if (dtor) {
                            dtor(cmd->is._1.value, 1, ti);
                            cmd->is._1.value = NULL;
                        }
                    } else {
                        ecs_os_memcpy(ptr.ptr, cmd->is._1.value, ti->size);
                    }

                    if (cmd->kind == EcsCmdSet) {
                        /* A set operation is add + copy + modified. We just did
                         * the add the copy, so the only thing that's left is a 
                         * modified command, which will call the OnSet 
                         * observers. */
                        cmd->kind = EcsCmdModified;
                    } else {
                        /* If this was an ensure, nothing's left to be done */
                        cmd->kind = EcsCmdSkip;
                    }
                } else {
                    /* The entity no longer has the component which means that
                     * there was a remove command for the component in the
                     * command queue. In that case skip the command. */
                    cmd->kind = EcsCmdSkip;
                }
                break;
            }
            case EcsCmdClone:
            case EcsCmdBulkNew:
            case EcsCmdAdd:
            case EcsCmdRemove:
            case EcsCmdEmplace:
            case EcsCmdModified:
            case EcsCmdModifiedNoHook:
            case EcsCmdAddModified:
            case EcsCmdPath:
            case EcsCmdDelete:
            case EcsCmdClear:
            case EcsCmdOnDeleteAction:
            case EcsCmdEnable:
            case EcsCmdDisable:
            case EcsCmdEvent:
            case EcsCmdSkip:
                break;
            }
        } while ((cur = next_for_entity));
    }

    if (added.count) {
        flecs_defer_begin(world, world->stages[0]);
        flecs_notify_on_add(world, table, start_table, 
            ECS_RECORD_TO_ROW(r->row), 1, &added, 0, set_mask, true);
        flecs_defer_end(world, world->stages[0]);
    }

    diff->added.array = added.array;
    diff->added.count = added.count;

    flecs_table_diff_builder_clear(diff);
}

/* Leave safe section. Run all deferred commands. */
bool flecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);

    if (stage->defer < 0) {
        /* Defer suspending makes it possible to do operations on the storage
         * without flushing the commands in the queue */
        return false;
    }

    ecs_assert(stage->defer > 0, ECS_INTERNAL_ERROR, NULL);

    if (!--stage->defer) {
        /* Test whether we're flushing to another queue or whether we're 
         * flushing to the storage */
        bool merge_to_world = false;
        if (flecs_poly_is(world, ecs_world_t)) {
            merge_to_world = world->stages[0]->defer == 0;
        }

        ecs_stage_t *dst_stage = flecs_stage_from_world(&world);
        ecs_commands_t *commands = stage->cmd;
        ecs_vec_t *queue = &commands->queue;

        if (ecs_vec_count(queue)) {
            /* Internal callback for capturing commands */
            if (world->on_commands_active) {
                world->on_commands_active(stage, queue, 
                    world->on_commands_ctx_active);
            }

            ecs_cmd_t *cmds = ecs_vec_first(queue);
            int32_t i, count = ecs_vec_count(queue);

            ecs_table_diff_builder_t diff;
            flecs_table_diff_builder_init(world, &diff);
            flecs_commands_push(stage);

            for (i = 0; i < count; i ++) {
                ecs_cmd_t *cmd = &cmds[i];
                ecs_entity_t e = cmd->entity;
                bool is_alive = flecs_entities_is_alive(world, e);

                /* A negative index indicates the first command for an entity */
                if (merge_to_world && (cmd->next_for_entity < 0)) {
                    /* Batch commands for entity to limit archetype moves */
                    if (is_alive) {
                        flecs_cmd_batch_for_entity(world, &diff, e, cmds, i);
                    } else {
                        world->info.cmd.discard_count ++;
                    }
                }

                /* Invalidate entry */
                if (cmd->entry) {
                    cmd->entry->first = -1;
                }

                /* If entity is no longer alive, this could be because the queue
                 * contained both a delete and a subsequent add/remove/set which
                 * should be ignored. */
                ecs_cmd_kind_t kind = cmd->kind;
                if ((kind != EcsCmdPath) && ((kind == EcsCmdSkip) || (e && !is_alive))) {
                    world->info.cmd.discard_count ++;
                    flecs_discard_cmd(world, cmd);
                    continue;
                }

                ecs_id_t id = cmd->id;

                switch(kind) {
                case EcsCmdAdd:
                    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
                    if (flecs_remove_invalid(world, id, &id)) {
                        if (id) {
                            world->info.cmd.add_count ++;
                            flecs_add_id(world, e, id);
                        } else {
                            world->info.cmd.discard_count ++;
                        }
                    } else {
                        world->info.cmd.discard_count ++;
                        ecs_delete(world, e);
                    }
                    break;
                case EcsCmdRemove:
                    flecs_remove_id(world, e, id);
                    world->info.cmd.remove_count ++;
                    break;
                case EcsCmdClone:
                    ecs_clone(world, e, id, cmd->is._1.clone_value);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdSet:
                    flecs_set_id_move(world, dst_stage, e, 
                        cmd->id, flecs_itosize(cmd->is._1.size), 
                        cmd->is._1.value, kind);
                    world->info.cmd.set_count ++;
                    break;
                case EcsCmdEmplace:
                    if (merge_to_world) {
                        bool is_new;
                        ecs_emplace_id(world, e, id, &is_new);
                        if (!is_new) {
                            kind = EcsCmdEnsure;
                        }
                    }
                    flecs_set_id_move(world, dst_stage, e, 
                        cmd->id, flecs_itosize(cmd->is._1.size), 
                        cmd->is._1.value, kind);
                    world->info.cmd.ensure_count ++;
                    break;
                case EcsCmdEnsure:
                    flecs_set_id_move(world, dst_stage, e, 
                        cmd->id, flecs_itosize(cmd->is._1.size), 
                        cmd->is._1.value, kind);
                    world->info.cmd.ensure_count ++;
                    break;
                case EcsCmdModified:
                    flecs_modified_id_if(world, e, id, true);
                    world->info.cmd.modified_count ++;
                    break;
                case EcsCmdModifiedNoHook:
                    flecs_modified_id_if(world, e, id, false);
                    world->info.cmd.modified_count ++;
                    break;
                case EcsCmdAddModified:
                    flecs_add_id(world, e, id);
                    flecs_modified_id_if(world, e, id, true);
                    world->info.cmd.set_count ++;
                    break;
                case EcsCmdDelete: {
                    ecs_delete(world, e);
                    world->info.cmd.delete_count ++;
                    break;
                }
                case EcsCmdClear:
                    ecs_clear(world, e);
                    world->info.cmd.clear_count ++;
                    break;
                case EcsCmdOnDeleteAction:
                    ecs_defer_begin(world);
                    flecs_on_delete(world, id, e, false);
                    ecs_defer_end(world);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdEnable:
                    ecs_enable_id(world, e, id, true);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdDisable:
                    ecs_enable_id(world, e, id, false);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdBulkNew:
                    flecs_flush_bulk_new(world, cmd);
                    world->info.cmd.other_count ++;
                    continue;
                case EcsCmdPath: {
                    bool keep_alive = true;
                    ecs_make_alive(world, e);
                    if (cmd->id) {
                        if (ecs_is_alive(world, cmd->id)) {
                            ecs_add_pair(world, e, EcsChildOf, cmd->id);
                        } else {
                            ecs_delete(world, e);
                            keep_alive = false;
                        }
                    }
                    if (keep_alive) {
                        ecs_set_name(world, e, cmd->is._1.value);
                    }
                    ecs_os_free(cmd->is._1.value);
                    cmd->is._1.value = NULL;
                    world->info.cmd.other_count ++;
                    break;
                }
                case EcsCmdEvent: {
                    ecs_event_desc_t *desc = cmd->is._1.value;
                    ecs_assert(desc != NULL, ECS_INTERNAL_ERROR, NULL);
                    ecs_emit((ecs_world_t*)stage, desc);
                    flecs_free_cmd_event(world, desc);
                    world->info.cmd.event_count ++;
                    break;
                }
                case EcsCmdSkip:
                    break;
                }

                if (cmd->is._1.value) {
                    flecs_stack_free(cmd->is._1.value, cmd->is._1.size);
                }
            }

            flecs_stack_reset(&commands->stack);
            ecs_vec_clear(queue);
            flecs_commands_pop(stage);

            flecs_table_diff_builder_fini(world, &diff);

            /* Internal callback for capturing commands, signal queue is done */
            if (world->on_commands_active) {
                world->on_commands_active(stage, NULL, 
                    world->on_commands_ctx_active);
            }
        }

        return true;
    }

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
        ecs_vec_t commands = stage->cmd->queue;

        if (ecs_vec_count(&commands)) {
            ecs_cmd_t *cmds = ecs_vec_first(&commands);
            int32_t i, count = ecs_vec_count(&commands);
            for (i = 0; i < count; i ++) {
                flecs_discard_cmd(world, &cmds[i]);
            }

            ecs_vec_fini_t(&stage->allocator, &stage->cmd->queue, ecs_cmd_t);

            ecs_vec_clear(&commands);
            flecs_stack_reset(&stage->cmd->stack);
            flecs_sparse_clear(&stage->cmd->entries);
        }

        return true;
    }

error:
    return false;
}
