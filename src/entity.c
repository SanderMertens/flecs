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

#ifdef FLECS_QUERY_DSL
#include "addons/query_dsl/query_dsl.h"
#endif

static
flecs_component_ptr_t flecs_table_get_component(
    ecs_table_t *table,
    int32_t column_index,
    int32_t row)
{
    ecs_assert(column_index < table->column_count, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *column = &table->data.columns[column_index];
    return (flecs_component_ptr_t){
        .ti = column->ti,
        .ptr = ECS_ELEM(column->data, column->ti->size, row)
    };
}

flecs_component_ptr_t flecs_get_component_ptr(
    const ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!cr) {
        return (flecs_component_ptr_t){0};
    }

    if (cr->flags & (EcsIdSparse|EcsIdDontFragment)) {
        ecs_entity_t entity = ecs_table_entities(table)[row];
        return (flecs_component_ptr_t){
            .ti = cr->type_info,
            .ptr = flecs_component_sparse_get(world, cr, table, entity)
        };
    }

    const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
    if (!tr || (tr->column == -1)) {
        return (flecs_component_ptr_t){0};
    }

    return flecs_table_get_component(table, tr->column, row);
}

void* flecs_get_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr)
{
    return flecs_get_component_ptr(world, table, row, cr).ptr;
}

void* flecs_get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component,
    ecs_component_record_t *cr,
    int32_t recur_depth)
{
    ecs_check(recur_depth < ECS_MAX_RECURSION, ECS_INVALID_OPERATION,
        "cycle detected in IsA relationship");

    /* Table (and thus entity) does not have component, look for base */
    if (!(table->flags & EcsTableHasIsA)) {
        return NULL;
    }

    if (!(cr->flags & EcsIdOnInstantiateInherit)) {
        return NULL;
    }

    /* Exclude Name */
    if (component == ecs_pair(ecs_id(EcsIdentifier), EcsName)) {
        return NULL;
    }

    /* Table should always be in the table index for (IsA, *), otherwise the
     * HasBase flag should not have been set */
    const ecs_table_record_t *tr_isa = flecs_component_get_table(
        world->cr_isa_wildcard, table);
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
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
        if (!tr) {
            if (cr->flags & EcsIdDontFragment) {
                ptr = flecs_component_sparse_get(world, cr, table, base);
            }

            if (!ptr) {
                ptr = flecs_get_base_component(world, table, component, cr, 
                    recur_depth + 1);
            }
        } else {
            if (cr->flags & EcsIdSparse) {
                return flecs_component_sparse_get(world, cr, table, base);
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

ecs_entity_t flecs_new_id(
    const ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);

    flecs_check_exclusive_world_access_write(world);

    /* It is possible that the world passed to this function is a stage, so
     * make sure we have the actual world. Cast away const since this is one of
     * the few functions that may modify the world while it is in readonly mode,
     * since it is thread safe (uses atomic inc when in threading mode) */
    ecs_world_t *unsafe_world = ECS_CONST_CAST(ecs_world_t*, world);

    ecs_assert(!(unsafe_world->flags & EcsWorldMultiThreaded),
        ECS_INVALID_OPERATION, "cannot create entities in multithreaded mode");

    ecs_entity_t entity = flecs_entities_new_id(unsafe_world);

    ecs_assert(!unsafe_world->info.max_id || 
        ecs_entity_t_lo(entity) <= unsafe_world->info.max_id, 
        ECS_OUT_OF_RANGE, NULL);

    return entity;
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

    ecs_assert(ecs_table_count(table) > row, ECS_INTERNAL_ERROR, NULL);
    flecs_actions_new(world, table, row, 1, diff, evt_flags, ctor, true);
    ecs_assert(table == record->table, ECS_INTERNAL_ERROR, NULL);

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
    ecs_assert(src_table->type.count >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_table_count(src_table) > src_row, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record == flecs_entities_get(world, entity), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record->table == src_table, ECS_INTERNAL_ERROR, NULL);

    /* Append new row to destination table */
    int32_t dst_row = flecs_table_append(world, dst_table, entity, 
        false, false);

    /* Invoke remove actions for removed components */
    flecs_actions_move_remove(world, src_table, dst_table, src_row, 1, diff);

    /* Copy entity & components from src_table to dst_table */
    flecs_table_move(world, entity, entity, dst_table, dst_row, 
        src_table, src_row, ctor);
    ecs_assert(record->table == src_table, ECS_INTERNAL_ERROR, NULL);

    /* Update entity index & delete old data after running remove actions */
    record->table = dst_table;
    record->row = ECS_ROW_TO_RECORD(dst_row, record->row & ECS_ROW_FLAGS_MASK);
    
    flecs_table_delete(world, src_table, src_row, false);

    flecs_actions_move_add(world, dst_table, src_table, dst_row, 1, diff, 
        evt_flags, ctor, true);

    ecs_assert(record->table == dst_table, ECS_INTERNAL_ERROR, NULL);
}

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

    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *src_table = record->table;
    int is_trav = (record->row & EcsEntityIsTraversable) != 0;
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (src_table == dst_table) {
        /* If source and destination table are the same no action is needed *
         * However, if a component was added in the process of traversing a
         * table, this suggests that a union relationship could have changed. */
        ecs_flags32_t non_fragment_flags = 
            src_table->flags & EcsTableHasDontFragment;
        if (non_fragment_flags) {
            diff->added_flags |= non_fragment_flags;
            diff->removed_flags |= non_fragment_flags;

            flecs_actions_move_add(world, src_table, src_table, 
                ECS_RECORD_TO_ROW(record->row), 1, diff, evt_flags, 
                    construct, true);

            flecs_actions_move_remove(world, src_table, src_table, 
                ECS_RECORD_TO_ROW(record->row), 1, diff);
        }
        flecs_journal_end();
        return;
    }

    ecs_os_perf_trace_push("flecs.commit");

    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_table_traversable_add(dst_table, is_trav);

    flecs_move_entity(world, entity, record, dst_table, diff, 
        construct, evt_flags);

    flecs_table_traversable_add(src_table, -is_trav);

    /* If the entity is being watched, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (is_trav) {
        flecs_update_component_monitors(world, &diff->added, &diff->removed);
    }

    if (!src_table->type.count && world->range_check_enabled) {
        ecs_check(!world->info.max_id || entity <= world->info.max_id, 
            ECS_OUT_OF_RANGE, 0);
        ecs_check(entity >= world->info.min_id, 
            ECS_OUT_OF_RANGE, 0);
    }

    ecs_os_perf_trace_pop("flecs.commit");

error:
    flecs_journal_end();
    return;
}

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

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_defer_begin(world, world->stages[0]);

    int32_t row = flecs_table_appendn(world, table, count, entities);

    ecs_type_t type = table->type;
    if (!type.count && !component_data) {
        flecs_defer_end(world, world->stages[0]);
        return entities;        
    }

    ecs_type_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = type.array;
        component_array.count = type.count;
    }

    flecs_actions_move_add(world, table, NULL, row, count, diff,
        (component_data == NULL) ? 0 : EcsEventNoOnSet, true, true);

    if (component_data) {
        int32_t c_i;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            /* Find component in storage type */
            ecs_entity_t id = component_ids->array[c_i];
            ecs_component_record_t *cr = flecs_components_get(world, id);
            ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *ti = cr->type_info;
            if (!ti) {
                ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, 
                    "component '%s' passed to to bulk_new() at index %d is a "
                        "tag/zero sized",
                            flecs_errstr(ecs_id_str(world, id)), c_i);
            }

            int32_t size = ti->size;
            void *ptr;

            if (cr->flags & EcsIdSparse) {
                int32_t e;
                for (e = 0; e < count; e ++) {
                    ptr = flecs_component_sparse_get(
                        world, cr, table, entities[e]);

                    ecs_copy_t copy;
                    ecs_move_t move;
                    if (is_move && (move = ti->hooks.move)) {
                        move(ptr, src_ptr, 1, ti);
                    } else if (!is_move && (copy = ti->hooks.copy)) {
                        copy(ptr, src_ptr, 1, ti);
                    } else {
                        ecs_os_memcpy(ptr, src_ptr, size);
                    }

                    flecs_notify_on_set(world, table, row + e, id, true);

                    src_ptr = ECS_OFFSET(src_ptr, size);
                }

            } else {
                const ecs_table_record_t *tr = 
                    flecs_component_get_table(cr, table);
                ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(tr->column != -1, ECS_INVALID_PARAMETER, 
                    "component '%s' passed to bulk_new() at index %d is a "
                        "tag/zero sized",
                            flecs_errstr(ecs_id_str(world, id)));
                ecs_assert(tr->count == 1, ECS_INVALID_PARAMETER,
                    "component passed to bulk_new() at index %d is "
                    "invalid/a wildcard",
                        flecs_errstr(ecs_id_str(world, id)));

                int32_t index = tr->column;
                ecs_column_t *column = &table->data.columns[index];
                ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
                ptr = ECS_ELEM(column->data, size, row);

                ecs_copy_t copy;
                ecs_move_t move;
                if (is_move && (move = ti->hooks.move)) {
                    move(ptr, src_ptr, count, ti);
                } else if (!is_move && (copy = ti->hooks.copy)) {
                    copy(ptr, src_ptr, count, ti);
                } else {
                    ecs_os_memcpy(ptr, src_ptr, size * count);
                }
            }
        };

        int32_t j, storage_count = table->column_count;
        for (j = 0; j < storage_count; j ++) {
            ecs_id_t component = flecs_column_id(table, j);
            ecs_type_t set_type = {
                .array = &component,
                .count = 1
            };

            flecs_notify_on_set_ids(world, table, row, count, &set_type);
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
    ecs_id_t component,
    bool construct)
{
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *src_table = record->table;
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &component, &diff);
    flecs_commit(world, entity, record, dst_table, &diff, construct, 
        EcsEventNoOnSet); /* No OnSet, this function is only called from
                           * functions that are about to set the component. */
}

void flecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_add(stage, entity, component)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *src_table = r->table;

    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, &component, &diff);

    flecs_commit(world, entity, r, dst_table, &diff, true, 0);

    flecs_defer_end(world, stage);
}

void flecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_remove(stage, entity, component)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *src_table = r->table;
    ecs_table_diff_t diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, &component, &diff);

    flecs_commit(world, entity, r, dst_table, &diff, true, 0);

    flecs_defer_end(world, stage);
}

void flecs_add_ids(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t *ids,
    int32_t count)
{
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);

    ecs_table_t *table = ecs_get_table(world, entity);
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t component = ids[i];
        table = flecs_find_table_add(world, table, component, &diff);
    }

    ecs_table_diff_t table_diff;
    flecs_table_diff_build_noalloc(&diff, &table_diff);
    flecs_commit(world, entity, r, table, &table_diff, true, 0);
    flecs_table_diff_builder_fini(world, &diff);
}

flecs_component_ptr_t flecs_ensure(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    ecs_record_t *r,
    ecs_size_t size)
{
    flecs_component_ptr_t dst = {0};

    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_component_record_t *cr = NULL;
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (component < FLECS_HI_COMPONENT_ID) {
        int16_t column_index = table->component_map[component];
        if (column_index > 0) {
            ecs_column_t *column = &table->data.columns[column_index - 1];
            ecs_assert(column->ti->size == size, ECS_INTERNAL_ERROR, NULL);
            dst.ptr = ECS_ELEM(column->data, size, ECS_RECORD_TO_ROW(r->row));
            dst.ti = column->ti;
            return dst;
        } else if (column_index < 0) {
            column_index = flecs_ito(int16_t, -column_index - 1);
            const ecs_table_record_t *tr = &table->_->records[column_index];
            cr = tr->hdr.cr;
            if (cr->flags & EcsIdSparse) {
                dst.ptr = flecs_component_sparse_get(
                    world, cr, r->table, entity);
                dst.ti = cr->type_info;
                ecs_assert(dst.ti->size == size, ECS_INTERNAL_ERROR, NULL);
                return dst;
            }
        }
    } else {
        cr = flecs_components_get(world, component);
        dst = flecs_get_component_ptr(
            world, table, ECS_RECORD_TO_ROW(r->row), cr);
        if (dst.ptr) {
            ecs_assert(dst.ti->size == size, ECS_INTERNAL_ERROR, NULL);
            return dst;
        }
    }

    /* If entity didn't have component yet, add it */
    flecs_add_id_w_record(world, entity, r, component, true);

    /* Flush commands so the pointer we're fetching is stable */
    flecs_defer_end(world, world->stages[0]);
    flecs_defer_begin(world, world->stages[0]);

    if (!cr) {
        cr = flecs_components_get(world, component);
        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
    return flecs_get_component_ptr(
        world, r->table, ECS_RECORD_TO_ROW(r->row), cr);
}

flecs_component_ptr_t flecs_get_mut(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t id,
    ecs_record_t *r,
    ecs_size_t size)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);    
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
    (void)entity;

    world = ecs_get_world(world);

    flecs_check_exclusive_world_access_write(world);

    flecs_component_ptr_t result;

    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_lookup[id]) {
            ecs_table_t *table = r->table;
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(table->component_map != NULL, ECS_INTERNAL_ERROR, NULL);
            int16_t column_index = table->component_map[id];
            if (column_index > 0) {
                ecs_column_t *column = &table->data.columns[column_index - 1];
                ecs_check(column->ti->size == size, 
                    ECS_INVALID_PARAMETER, "invalid component size");
                result.ptr = ECS_ELEM(column->data, size, 
                    ECS_RECORD_TO_ROW(r->row));
                result.ti = column->ti;
                return result;
            }
            return (flecs_component_ptr_t){0};
        }
    }

    ecs_component_record_t *cr = flecs_components_get(world, id);
    int32_t row = ECS_RECORD_TO_ROW(r->row);
    return flecs_get_component_ptr(world, r->table, row, cr);
error:
    return (flecs_component_ptr_t){0};
}

void flecs_record_add_flag(
    ecs_record_t *record,
    uint32_t flag)
{
    if (flag == EcsEntityIsTraversable) {
        if (!(record->row & flag)) {
            ecs_table_t *table = record->table;
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
            flecs_table_traversable_add(table, 1);
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

void flecs_add_to_root_table(
    ecs_world_t *world,
    ecs_entity_t e)
{
    flecs_poly_assert(world, ecs_world_t);

    ecs_assert(!(world->flags & EcsWorldMultiThreaded), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *r = flecs_entities_get(world, e);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table == NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t row = flecs_table_append(world, &world->store.root, e, false, false);
    r->table = &world->store.root;
    r->row = ECS_ROW_TO_RECORD(row, r->row & ECS_ROW_FLAGS_MASK);

    flecs_journal(world, EcsJournalNew, e, 0, 0);
}

const char* flecs_entity_invalid_reason(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    if (!entity) {
        return "entity id cannot be 0";
    }

    if (entity & ECS_PAIR) {
        return "cannot use a pair as an entity";
    }

    if (entity & ECS_ID_FLAGS_MASK) {
        return "entity id contains flag bits (TOGGLE or AUTO_OVERRIDE)";
    }

    /* Entities should not contain data in dead zone bits */
    if (entity & ~0xFF00FFFFFFFFFFFF) {
        return "entity id is not a valid bit pattern for an entity";
    }

    if (!ecs_is_alive(world, entity)) {
        return "entity is not alive";
    }

    return NULL;
}

#define flecs_assert_entity_valid(world, entity, function) \
    ecs_check(entity && ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, \
        "invalid entity '%s' passed to %s(): %s", \
            flecs_errstr(ecs_id_str(world, entity)),\
            function,\
            flecs_entity_invalid_reason(world, entity));

#define flecs_assert_component_valid(world, entity, component, function)\
    ecs_check(ecs_id_is_valid(world, component), ECS_INVALID_PARAMETER, \
        "invalid component '%s' passed to %s() for entity '%s': %s", \
            flecs_errstr(ecs_id_str(world, component)), \
            function,\
            flecs_errstr_1(ecs_get_path(world, entity)), \
            flecs_id_invalid_reason(world, component))


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
    flecs_assert_entity_valid(world, entity, "commit");
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
        diff.added_flags = table->flags & EcsTableAddEdgeFlags;
    }
    if (removed) {
        diff.removed = *removed;
        if (src_table) {
            diff.removed_flags = src_table->flags & EcsTableRemoveEdgeFlags;
        }
    }

    ecs_defer_begin(world);
    flecs_commit(world, entity, record, table, &diff, true, 0);
    ecs_defer_end(world);

    return src_table != table;
error:
    return false;
}

ecs_entity_t ecs_new(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_stage_from_world(&world);
    ecs_entity_t e = flecs_new_id(world);
    flecs_add_to_root_table(world, e);
    return e;
error:
    return 0;
}

ecs_entity_t ecs_new_low_id(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(world->flags & EcsWorldMultiThreaded), 
        ECS_INVALID_OPERATION, "cannot create entities in multithreaded mode");

    flecs_stage_from_world(&world);

    flecs_check_exclusive_world_access_write(world);

    ecs_entity_t e = 0;
    if (world->info.last_component_id < FLECS_HI_COMPONENT_ID) {
        do {
            e = world->info.last_component_id ++;
        } while (ecs_exists(world, e) && e <= FLECS_HI_COMPONENT_ID);        
    }

    if (!e || e >= FLECS_HI_COMPONENT_ID) {
        /* If the low component ids are depleted, return a regular entity id */
        e = ecs_new(world);
    } else {
        flecs_entities_ensure(world, e);
        flecs_add_to_root_table(world, e);
    }

    return e;
error: 
    return 0;
}

ecs_entity_t ecs_new_w_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_stage_from_world(&world);    
    ecs_entity_t entity = flecs_new_id(world);
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_flags32_t flags = table->flags & EcsTableAddEdgeFlags;
    if (table->flags & EcsTableHasIsA) {
        flags |= EcsTableHasOnAdd;
    }

    ecs_table_diff_t table_diff = { 
        .added = table->type,
        .added_flags = flags
    };

    flecs_new_entity(world, entity, r, table, &table_diff, true, 0);

    return entity;
error:
    return 0;
}

ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, component), ECS_INVALID_PARAMETER,
        "invalid component '%s' passed to new_w(): %s",
            flecs_errstr(ecs_id_str(world, component)),
            flecs_id_invalid_reason(world, component))

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_cmd(stage)) {
        ecs_entity_t e = ecs_new(world);
        ecs_add_id(world, e, component);
        return e;
    }

    ecs_table_diff_t table_diff = ECS_TABLE_DIFF_INIT;
    ecs_table_t *table = flecs_table_traverse_add(
        world, &world->store.root, &component, &table_diff);
    
    ecs_entity_t entity = flecs_new_id(world);
    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_new_entity(world, entity, r, table, &table_diff, true, 0);

    flecs_defer_end(world, stage);

    return entity;
error:
    return 0;
}

static
void flecs_copy_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *r,
    ecs_id_t component,
    size_t size,
    void *dst_ptr,
    const void *src_ptr,
    const ecs_type_info_t *ti)
{
    ecs_assert(dst_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ti->hooks.on_replace) {
        flecs_invoke_replace_hook(
            world, r->table, entity, component, dst_ptr, src_ptr, ti);
    }

    ecs_copy_t copy = ti->hooks.copy;
    if (copy) {
        copy(dst_ptr, src_ptr, 1, ti);
    } else {
        ecs_os_memcpy(dst_ptr, src_ptr, flecs_utosize(size));
    }

    flecs_table_mark_dirty(world, r->table, component);

    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_notify_on_set(
        world, table, ECS_RECORD_TO_ROW(r->row), component, true);
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
#ifdef FLECS_QUERY_DSL
    const char *ptr = expr;
    if (ptr) {
        ecs_id_t component = 0;
        while (ptr[0] && (ptr = flecs_id_parse(world, name, ptr, &component))) {
            if (!component) {
                break;
            }

            if (!ecs_id_is_valid(world, component)) {
                char *idstr = ecs_id_str(world, component);
                ecs_parser_error(name, expr, (ptr - expr), 
                    "'%s' is invalid for ecs_entity_desc_t::add_expr", idstr);
                ecs_os_free(idstr);
                goto error;
            }

            ecs_vec_append_t(&world->allocator, ids, ecs_id_t)[0] = component;
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
#ifdef FLECS_QUERY_DSL
    const char *ptr = expr;
    if (ptr) {
        ecs_id_t component = 0;
        while (ptr[0] && (ptr = flecs_id_parse(world, name, ptr, &component))) {
            if (!component) {
                break;
            }
            ecs_add_id(world, entity, component);
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
            ecs_assert(!ecs_os_strcmp(desc->symbol, sym), ECS_INCONSISTENT_NAME, 
                "entity symbol inconsistent: %s (provided) vs. %s (existing)",
                    desc->symbol, sym);
        } else {
            ecs_set_symbol(world, result, desc->symbol);
        }
    }

    /* If a name is provided but not yet assigned, add the Name component */
    if (name && !name_assigned) {
        if (!ecs_add_path_w_sep(world, result, scope, name, sep, root_sep)) {
            if (name[0] == '#') {
                /* Numerical ids should always return, unless it's invalid */
                goto error;
            }
        }
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
        ecs_id_t component;

        while ((component = desc->add[i ++])) {
            table = flecs_find_table_add(world, table, component, &diff);
        }
    }

    /* Add components from the 'set' array */
    if (desc->set) {
        int32_t i = 0;
        ecs_id_t component;

        while ((component = desc->set[i ++].type)) {
            table = flecs_find_table_add(world, table, component, &diff);
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
            ecs_component_record_t *cr = flecs_components_get(world, v->type);
            flecs_component_ptr_t ptr = flecs_get_component_ptr(
                world, table, row, cr);
            ecs_check(ptr.ptr != NULL, ECS_INVALID_OPERATION, 
                "component '%s' added to entity '%s' was removed during the "
                "operation, make sure not to remove the component in hooks/observers",
                    flecs_errstr(ecs_id_str(world, v->type)),
                    flecs_errstr_2(ecs_get_path(world, result)));
            
            const ecs_type_info_t *ti = cr->type_info;
            flecs_copy_id(world, result, r, v->type, 
                flecs_itosize(ti->size), ptr.ptr, v->ptr, ti);
        }

        flecs_defer_end(world, world->stages[0]);
    }

    flecs_table_diff_builder_fini(world, &diff);
    ecs_vec_fini_t(&world->allocator, &ids, ecs_id_t);
    return 0;
error:
    flecs_table_diff_builder_fini(world, &diff);
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
    if (desc->add) {
        int32_t i = 0;
        ecs_id_t component;

        while ((component = desc->add[i ++])) {
            bool defer = true;
            if (ECS_HAS_ID_FLAG(component, PAIR) && 
                ECS_PAIR_FIRST(component) == EcsChildOf) 
            {
                scope = ECS_PAIR_SECOND(component);
                if (name && (!desc->id || !name_assigned)) {
                    /* New named entities are created by temporarily going out of
                     * readonly mode to ensure no duplicates are created. */
                    defer = false;
                }
            }
            if (defer) {
                ecs_add_id(world, entity, component);
            }
        }
    }

    /* Set component values */
    if (desc->set) {
        int32_t i = 0;
        const ecs_value_t *v;
        while ((void)(v = &desc->set[i ++]), v->type) {
            if (v->ptr) {
                ecs_check(v->type != 0, ECS_INVALID_PARAMETER,
                    "0 passed for component to ecs_entity_desc_t::set[%d]", i);
                const ecs_type_info_t *ti = ecs_get_type_info(world, v->type);
                ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
                    "component '%s' passed to ecs_entity_desc_t::set[%d] is a "
                    "tag/zero sized",
                        flecs_errstr(ecs_id_str(world, v->type)), i);
                ecs_set_id(world, entity, v->type, 
                    flecs_ito(size_t, ti->size), v->ptr);
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
error:
    return;
}

ecs_entity_t ecs_entity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_entity_desc_t is uninitialized, initialize to {0} before using");

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_entity_t scope = stage->scope;
    ecs_id_t with = ecs_get_with(world);
    ecs_entity_t result = desc->id;

#ifdef FLECS_DEBUG
    if (desc->add) {
        ecs_id_t component;
        int32_t i = 0;
        while ((component = desc->add[i ++])) {
            if (ECS_HAS_ID_FLAG(component, PAIR) && 
                (ECS_PAIR_FIRST(component) == EcsChildOf))
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
                ecs_err(
                    "the '#xxx' string provided to ecs_entity_desc_t::name "
                    "does not match the id provided to ecs_entity_desc_t::id");
                return 0;
            }

            name = NULL;
            result = id;
        }
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
        ecs_check(ecs_is_valid(world, desc->parent), ECS_INVALID_PARAMETER, 
            "the entity provided in ecs_entity_desc_t::parent is not valid");
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
            new_entity = true;
            ecs_assert(ecs_get_type(world, result) != NULL,
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ecs_get_type(world, result)->count == 0,
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

    if (ecs_is_deferred(world)) {
        flecs_deferred_add_remove((ecs_world_t*)stage, result, name, desc, 
            scope, with, new_entity, name_assigned);
    } else {
        if (flecs_traverse_add(world, result, name, desc,
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
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_bulk_desc_t is uninitialized, set to {0} before using");

    flecs_check_exclusive_world_access_write(world);

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
            ecs_assert(!ecs_is_alive(world, entities[i]), ECS_INVALID_PARAMETER,
                "cannot pass alive entities to ecs_bulk_init()");
            flecs_entities_ensure(world, entities[i]);
        }
    }

    ecs_type_t ids;
    ecs_table_t *table = desc->table;
    if (!table) {
        table = &world->store.root;
    }

    if (!table->type.count) {
        ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
        flecs_table_diff_builder_init(world, &diff);

        int32_t i = 0;
        ecs_id_t component;
        while ((component = desc->ids[i])) {
            table = flecs_find_table_add(world, table, component, &diff);
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

        int32_t i = 0;
        while ((desc->ids[i])) {
            i ++;
        }

        ids.array = ECS_CONST_CAST(ecs_id_t*, desc->ids);
        ids.count = i;

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

const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t component,
    int32_t count)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    const ecs_entity_t *ids;
    if (flecs_defer_bulk_new(world, stage, count, component, &ids)) {
        return ids;
    }

    ecs_table_t *table = &world->store.root;
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);
    
    if (component) {
        table = flecs_find_table_add(world, table, component, &diff);
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
        "ecs_component_desc_t is uninitialized, set to {0} before using");

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
                ecs_trace("#[green]tag#[reset] %s registered", 
                    ecs_get_name(world, result));
            } else {
                ecs_trace("#[green]component#[reset] %s registered", 
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

    if (result >= world->info.last_component_id && 
        result < FLECS_HI_COMPONENT_ID) 
    {
        world->info.last_component_id = result + 1;
    }

    flecs_resume_readonly(world, &readonly_state);

    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_has(world, result, EcsComponent), ECS_INTERNAL_ERROR, NULL);

    return result;
error:
    return 0;
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
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (table->type.count) {
        ecs_table_diff_t diff = {
            .removed = table->type,
            .removed_flags = table->flags & EcsTableRemoveEdgeFlags
        };

        flecs_commit(world, entity, r, &world->store.root, &diff, false, 0);
    }

    flecs_entity_remove_non_fragmenting(world, entity, NULL);

    flecs_defer_end(world, stage);
error:
    return;
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

    ecs_os_perf_trace_push("flecs.delete");

    ecs_record_t *r = flecs_entities_try(world, entity);
    if (r) {
        ecs_check(!ecs_has_pair(world, entity, EcsOnDelete, EcsPanic),
            ECS_CONSTRAINT_VIOLATED,
                "cannot delete entity '%s' with (OnDelete, Panic) trait",
                    flecs_errstr(ecs_get_path(world, entity)));

        flecs_journal_begin(world, EcsJournalDelete, entity, NULL, NULL);

        ecs_flags32_t row_flags = ECS_RECORD_TO_ROW_FLAGS(r->row);
        ecs_table_t *table;
        if (row_flags) {
            if (row_flags & EcsEntityIsTarget) {
                flecs_on_delete(world, ecs_pair(EcsFlag, entity), 0, true, true);
                flecs_on_delete(world, ecs_pair(EcsWildcard, entity), 0, true, true);
            }

            if (row_flags & EcsEntityIsId) {
                flecs_on_delete(world, entity, 0, true, true);
                flecs_on_delete(world, ecs_pair(entity, EcsWildcard), 0, true, true);
            }

            /* Merge operations before deleting entity */
            flecs_defer_end(world, stage);
            flecs_defer_begin(world, stage);
        }

        /* Entity is still in use by a query */
        ecs_assert((world->flags & EcsWorldQuit) || 
                !flecs_component_is_delete_locked(world, entity), 
            ECS_INVALID_OPERATION, 
            "cannot delete '%s' as it is still in use by queries",
                flecs_errstr(ecs_id_str(world, entity)));

        table = r->table;

        if (table) { /* NULL if entity got cleaned up as result of cycle */
            ecs_table_diff_t diff = {
                .removed = table->type,
                .removed_flags = table->flags & EcsTableRemoveEdgeFlags
            };

            int32_t row = ECS_RECORD_TO_ROW(r->row);
            flecs_actions_move_remove(
                world, table, &world->store.root, row, 1, &diff);
            flecs_entity_remove_non_fragmenting(world, entity, r);
            flecs_table_delete(world, table, row, true);

            if (row_flags & EcsEntityIsTraversable) {
                flecs_table_traversable_add(table, -1);
            }
        }
        
        flecs_entities_remove(world, entity);

        flecs_journal_end();
    }

    flecs_defer_end(world, stage);
error:
    ecs_os_perf_trace_pop("flecs.delete");
    return;
}

void ecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "add");
    flecs_assert_component_valid(world, entity, component, "add");
    flecs_add_id(world, entity, component);
error:
    return;
}

void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "remove");

    /* Component validity check is slightly different for remove() because it is
     * allowed to remove wildcards, but not allowed to add wildcards. */
    ecs_check(ecs_id_is_valid(world, component) || 
        ecs_id_is_wildcard(component), ECS_INVALID_PARAMETER, 
            "invalid component '%s' passed to remove() for entity '%s': %s",
                flecs_errstr(ecs_id_str(world, component)),
                flecs_errstr_1(ecs_get_path(world, entity)),
                flecs_id_invalid_reason(world, component));

    flecs_remove_id(world, entity, component);
error:
    return;
}

void ecs_auto_override_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    flecs_assert_component_valid(world, entity, component, "auto_override");
    ecs_add_id(world, entity, ECS_AUTO_OVERRIDE | component);
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
    ecs_check(ecs_is_alive(world, src), ECS_INVALID_PARAMETER, NULL);
    ecs_check(!dst || (ecs_get_table(world, dst)->type.count == 0), 
        ECS_INVALID_PARAMETER, 
            "target entity for clone() cannot have components");

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
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *dst_table = src_table;
    if (src_table->flags & EcsTableHasName) {
        dst_table = ecs_table_remove_id(world, src_table, 
            ecs_pair_t(EcsIdentifier, EcsName));
    }

    ecs_type_t dst_type = dst_table->type;
    ecs_table_diff_t diff = {
        .added = dst_type,
        .added_flags = dst_table->flags & EcsTableAddEdgeFlags
    };

    ecs_record_t *dst_r = flecs_entities_get(world, dst);

    if (dst_table != dst_r->table) {
        flecs_move_entity(world, dst, dst_r, dst_table, &diff, true, 0);
    }

    if (copy_value) {
        int32_t row = ECS_RECORD_TO_ROW(dst_r->row);
        int32_t i, count = src_table->column_count;
        for (i = 0; i < count; i ++) {
            int32_t type_id = ecs_table_column_to_type_index(src_table, i);
            ecs_id_t component = src_table->type.array[type_id];

            void *dst_ptr = ecs_get_mut_id(world, dst, component);
            if (!dst_ptr) {
                continue;
            }

            const void *src_ptr = ecs_get_id(world, src, component);
            const ecs_type_info_t *ti = src_table->data.columns[i].ti;
            if (ti->hooks.copy) {
                ti->hooks.copy(dst_ptr, src_ptr, 1, ti);
            } else {
                ecs_os_memcpy(dst_ptr, src_ptr, ti->size);
            }

            flecs_notify_on_set(world, dst_table, row, component, true);
        }

        if (dst_table->flags & EcsTableHasSparse) {
            count = dst_table->type.count;
            for (i = 0; i < count; i ++) {
                const ecs_table_record_t *tr = &dst_table->_->records[i];
                ecs_component_record_t *cr = tr->hdr.cr;
                if (cr->sparse) {
                    void *src_ptr = flecs_component_sparse_get(
                        world, cr, src_table, src);
                    if (src_ptr) {
                        ecs_set_id(world, dst, cr->id, 
                            flecs_ito(size_t, cr->type_info->size), src_ptr);
                    }
                }
            }
        }
    }

    if (src_r->row & EcsEntityHasDontFragment) {
        ecs_component_record_t *cur = world->cr_non_fragmenting_head;
        while (cur) {
            if (!ecs_id_is_wildcard(cur->id)) {
                ecs_assert(cur->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
                if (cur->sparse) {
                    if (cur->type_info) {
                        void *src_ptr = flecs_sparse_get(cur->sparse, 0, src);
                        if (src_ptr) {
                            ecs_set_id(world, dst, cur->id, 
                                flecs_ito(size_t, cur->type_info->size), src_ptr);
                        }
                    } else {
                        if (flecs_sparse_has(cur->sparse, src)) {
                            ecs_add_id(world, dst, cur->id);
                        }
                    }
                }
            }
    
            cur = cur->non_fragmenting.next;
        }
    }

    flecs_defer_end(world, stage);
    return dst;
error:
    return 0;
}

const void* ecs_get_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get");
    ecs_check(ecs_id_is_valid(world, component) || ecs_id_is_wildcard(component), 
        ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (component < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_lookup[component]) {
            ecs_get_low_id(table, r, component);
            return NULL;
        }
    }

    ecs_component_record_t *cr = flecs_components_get(world, component);
    if (!cr) {
        return NULL;
    }

    if (cr->flags & EcsIdDontFragment) {
        void *ptr = flecs_component_sparse_get(world, cr, table, entity);
        if (ptr) {
            return ptr;
        }
    }

    const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
    if (!tr) {
        return flecs_get_base_component(world, table, component, cr, 0);
    } else {
        if (cr->flags & EcsIdSparse) {
            return flecs_component_sparse_get(world, cr, table, entity);
        }
        ecs_check(tr->column != -1, ECS_INVALID_PARAMETER,
            "component '%s' passed to get() is a tag/zero sized",
                flecs_errstr(ecs_id_str(world, component)));
    }

    int32_t row = ECS_RECORD_TO_ROW(r->row);
    return flecs_table_get_component(table, tr->column, row).ptr;
error:
    return NULL;
}

#ifdef FLECS_DEBUG
static
bool flecs_component_has_on_replace(
    const ecs_world_t *world,
    ecs_id_t component,
    const char *funcname)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "invalid component '%s' for %s(): component cannot be a tag/zero sized",
            flecs_errstr(ecs_id_str(world, component)), funcname);
    return ti->hooks.on_replace != NULL;
error:
    return false;
}
#endif

void* ecs_get_mut_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_mut");
    flecs_assert_component_valid(world, entity, component, "get_mut");
    ecs_dbg_assert(!flecs_component_has_on_replace(world, component, "get_mut"), 
        ECS_INVALID_PARAMETER,
        "cannot call get_mut() for component '%s' which has an on_replace hook "
        "(use set()/assign())",
            flecs_errstr(ecs_id_str(world, component)));

    world = ecs_get_world(world);

    flecs_check_exclusive_world_access_write(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);

    if (component < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_lookup[component]) {
            ecs_get_low_id(r->table, r, component);
            return NULL;
        }
    }

    ecs_component_record_t *cr = flecs_components_get(world, component);
    int32_t row = ECS_RECORD_TO_ROW(r->row);
    return flecs_get_component_ptr(world, r->table, row, cr).ptr;
error:
    return NULL;
}

void* ecs_ensure_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "ensure");
    flecs_assert_component_valid(world, entity, component, "ensure");
    ecs_dbg_assert(!flecs_component_has_on_replace(world, component, "ensure"),
        ECS_INVALID_PARAMETER,
        "cannot call ensure() for component '%s' which has an on_replace hook "
        "(use set()/assign())",
            flecs_errstr(ecs_id_str(world, component)));

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_cmd(stage)) {
        return flecs_defer_ensure(
            world, stage, entity, component, flecs_uto(int32_t, size));
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    void *result = flecs_ensure(world, entity, component, r, 
        flecs_uto(int32_t, size)).ptr;
    ecs_check(result != NULL, ECS_INVALID_OPERATION, 
        "component '%s' ensured on entity '%s' was removed during the "
        "operation, make sure not to remove the component in hooks/observers",
            flecs_errstr(ecs_id_str(world, component)),
            flecs_errstr_2(ecs_get_path(world, entity)));

    flecs_defer_end(world, stage);
    return result;
error:
    return NULL;
}

void* ecs_emplace_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size,
    bool *is_new)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "emplace");
    flecs_assert_component_valid(world, entity, component, "emplace");
    ecs_dbg_assert(!flecs_component_has_on_replace(world, component, "emplace"),
        ECS_INVALID_PARAMETER,
        "cannot call emplace() for component '%s' which has an on_replace hook "
        "(use set()/entity::replace())",
            flecs_errstr(ecs_id_str(world, component)));

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_cmd(stage)) {
        return flecs_defer_emplace(
            world, stage, entity, component, flecs_uto(int32_t, size), is_new);
    }

    ecs_check(is_new || !ecs_has_id(world, entity, component), 
        ECS_INVALID_PARAMETER, 
            "cannot emplace() existing component '%s' for entity '%s' unless "
            "'is_new' argument is provided",
                flecs_errstr(ecs_id_str(world, component)),
                flecs_errstr_2(ecs_get_path(world, entity)));

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = r->table;

    ecs_component_record_t *cr = flecs_components_ensure(world, component);
    if (cr->flags & EcsIdDontFragment) {
        void *ptr = flecs_component_sparse_get(world, cr, table, entity);
        if (ptr) {
            if (is_new) {
                *is_new = false;
            }
            flecs_defer_end(world, stage);
            return ptr;
        }

        if (is_new) {
            *is_new = true;
        }
        is_new = NULL;
    }

    flecs_add_id_w_record(world, entity, r, component, false /* No ctor */);
    flecs_defer_end(world, stage);

    void *ptr = flecs_get_component(
        world, r->table, ECS_RECORD_TO_ROW(r->row), cr);
    ecs_check(ptr != NULL, ECS_INVALID_OPERATION, 
        "component '%s' emplaced on entity '%s' was removed during the "
        "operation, make sure not to remove the component in hooks/observers",
            flecs_errstr(ecs_id_str(world, component)),
            flecs_errstr_2(ecs_get_path(world, entity)));

    if (is_new) {
        *is_new = table != r->table;
    }

    return ptr;
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

    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_os_ainc(&table->_->lock);
    (void)count;
    if (write) {
        ecs_check(count == 1, ECS_ACCESS_VIOLATION, 
            "invalid concurrent access to table for entity '%s'",
                flecs_errstr(ecs_get_path(world, entity)));
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
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return table->data.entities[ECS_RECORD_TO_ROW(record->row)];
error:
    return 0;
}

const void* ecs_record_get_id(
    const ecs_world_t *stage,
    const ecs_record_t *r,
    ecs_id_t component)
{
    const ecs_world_t *world = ecs_get_world(stage);
    ecs_component_record_t *cr = flecs_components_get(world, component);
    return flecs_get_component(
        world, r->table, ECS_RECORD_TO_ROW(r->row), cr);
}

bool ecs_record_has_id(
    ecs_world_t *stage,
    const ecs_record_t *r,
    ecs_id_t component)
{
    const ecs_world_t *world = ecs_get_world(stage);
    if (r->table) {
        return ecs_table_has_id(world, r->table, component);
    }
    return false;
}

void* ecs_record_ensure_id(
    ecs_world_t *stage,
    ecs_record_t *r,
    ecs_id_t component)
{
    const ecs_world_t *world = ecs_get_world(stage);
    ecs_component_record_t *cr = flecs_components_get(world, component);
    return flecs_get_component(
        world, r->table, ECS_RECORD_TO_ROW(r->row), cr);
}

void flecs_modified_id_if(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    bool invoke_hook)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "modified");
    flecs_assert_component_valid(world, entity, component, "modified");

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_modified(stage, entity, component)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_component_record_t *cr = flecs_components_get(world, component);
    if (!cr || !flecs_component_get_table(cr, table)) {
        flecs_defer_end(world, stage);
        return;
    }

    flecs_notify_on_set(
        world, table, ECS_RECORD_TO_ROW(r->row), component, invoke_hook);

    flecs_table_mark_dirty(world, table, component);
    flecs_defer_end(world, stage);
error:
    return;
}

void ecs_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "modified");
    flecs_assert_component_valid(world, entity, component, "modified");

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (component < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[component]) {
            return;
        }
    }

    if (flecs_defer_modified(stage, entity, component)) {
        return;
    }

    /* If the entity does not have the component, calling ecs_modified is 
     * invalid. The assert needs to happen after the defer statement, as the
     * entity may not have the component when this function is called while
     * operations are being deferred. */
    ecs_check(ecs_has_id(world, entity, component), ECS_INVALID_PARAMETER, 
        "invalid call to modified(), entity '%s' does not have component '%s'",
            flecs_errstr(ecs_get_path(world, entity)),
            flecs_errstr_2(ecs_id_str(world, component)));

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_table_t *table = r->table;
    flecs_notify_on_set(
        world, table, ECS_RECORD_TO_ROW(r->row), component, true);

    flecs_table_mark_dirty(world, table, component);
    flecs_defer_end(world, stage);
error:
    return;
}

void flecs_set_id_move(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size,
    void *ptr,
    ecs_cmd_kind_t cmd_kind)
{
    if (flecs_defer_cmd(stage)) {
        ecs_throw(ECS_INVALID_OPERATION, 
            "cannot flush a command queue to a deferred stage. This happens "
            "when a stage is explicitly merged into the world/another stage "
            "that is deferred");
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_ensure(
        world, entity, component, r, flecs_uto(int32_t, size));
    ecs_check(dst.ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_type_info_t *ti = dst.ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ti->hooks.on_replace) {
        flecs_invoke_replace_hook(
            world, r->table, entity, component, dst.ptr, ptr, ti);
    }

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

    flecs_table_mark_dirty(world, r->table, component);

    if (cmd_kind == EcsCmdSet) {
        ecs_table_t *table = r->table;
        if (table->flags & EcsTableHasOnSet || ti->hooks.on_set) {
            ecs_type_t ids = { .array = &component, .count = 1 };
            flecs_notify_on_set_ids(
                world, table, ECS_RECORD_TO_ROW(r->row), 1, &ids);
        }
    }

    flecs_defer_end(world, stage);
error:
    return;
}

void ecs_set_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    size_t size,
    const void *ptr)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "set");
    flecs_assert_component_valid(world, entity, component, "set");
    ecs_check(size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, 
        "invalid call to set() for component '%s' and entity '%s': no "
        "component value provided",
            flecs_errstr(ecs_id_str(world, component)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_cmd(stage)) {
        flecs_defer_set(world, stage, entity, component, flecs_utosize(size), 
            ECS_CONST_CAST(void*, ptr));
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_ensure(world, entity, component, r, 
        flecs_uto(int32_t, size));

    if (component < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[component]) {
            ecs_os_memcpy(dst.ptr, ptr, size);
            goto done;
        }
    }

    flecs_copy_id(world, entity, r, component, size, dst.ptr, ptr, dst.ti);

done:
    flecs_defer_end(world, stage);
error:
    return;
}

#if defined(FLECS_DEBUG) || defined(FLECS_KEEP_ASSERT)
static
bool flecs_can_toggle(
    ecs_world_t *world,
    ecs_id_t component)
{
    ecs_component_record_t *cr = flecs_components_get(world, component);
    if (!cr) {
        return ecs_has_id(world, component, EcsCanToggle);
    }

    return (cr->flags & EcsIdCanToggle) != 0;
}
#endif

void ecs_enable_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    bool enable)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    flecs_assert_component_valid(world, entity, component, "enable_component");
    ecs_check(flecs_can_toggle(world, component), ECS_INVALID_OPERATION, 
        "cannot enable/disable component '%s' as it does not have the CanToggle trait",
            flecs_errstr(ecs_id_str(world, component)));

    ecs_entity_t bs_id = component | ECS_TOGGLE;
    ecs_add_id((ecs_world_t*)stage, entity, bs_id);

    if (flecs_defer_enable(stage, entity, component, enable)) {
        return;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);    
    ecs_table_t *table = r->table;
    int32_t index = ecs_table_get_type_index(world, table, bs_id);
    ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);

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
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "is_enabled");
    flecs_assert_component_valid(world, entity, component, "is_enabled");

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t bs_id = component | ECS_TOGGLE;
    int32_t index = ecs_table_get_type_index(world, table, bs_id);
    if (index == -1) {
        /* If table does not have TOGGLE column for component, component is
         * always enabled, if the entity has it */
        return ecs_has_id(world, entity, component);
    }

    ecs_assert(table->_ != NULL, ECS_INTERNAL_ERROR, NULL);
    index -= table->_->bs_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &table->_->bs_columns[index];

    return flecs_bitset_get(bs, ECS_RECORD_TO_ROW(r->row));
error:
    return false;
}

void ecs_set_child_order(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_entity_t *children,
    int32_t child_count)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, parent), ECS_INVALID_PARAMETER, NULL);
    ecs_check(children == NULL || child_count, ECS_INVALID_PARAMETER, 
        "children array passed to set_child_order() cannot be NULL if "
        "child_count is not 0");
    ecs_check(children != NULL || !child_count, ECS_INVALID_PARAMETER, 
        "children array passed to set_child_order() cannot be not-NULL if "
        "child_count is 0");
    ecs_check(!(world->flags & EcsWorldMultiThreaded), ECS_INVALID_OPERATION, 
        "cannot call set_child_oderder() while in multithreaded mode");

    flecs_stage_from_world(&world);

    flecs_check_exclusive_world_access_write(world);

    flecs_ordered_children_reorder(world, parent, children, child_count);

error:
    return;
}

ecs_entities_t ecs_get_ordered_children(
    const ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, parent), ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    flecs_check_exclusive_world_access_read(world);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(parent));

    ecs_check(cr != NULL && (cr->flags & EcsIdOrderedChildren), 
        ECS_INVALID_PARAMETER, 
            "invalid call to get_ordered_children(): parent '%s' does not have "
            "the OrderedChildren trait",
                flecs_errstr(ecs_get_path(world, parent)));

    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);

    return (ecs_entities_t){
        .count = ecs_vec_count(&cr->pair->ordered_children),
        .alive_count = ecs_vec_count(&cr->pair->ordered_children),
        .ids = ecs_vec_first(&cr->pair->ordered_children),
    };
error:
    return (ecs_entities_t){0};
}

bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "has");
    ecs_check(component != 0, ECS_INVALID_PARAMETER, 
        "invalid component passed to has(): component cannot be 0");

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get_any(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (component < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_lookup[component]) {
            return table->component_map[component] != 0;
        }
    }

    ecs_component_record_t *cr = flecs_components_get(world, component);
    bool can_inherit = false;

    if (cr) {
        const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
        if (tr) {
            return true;
        }

        if (cr->flags & (EcsIdDontFragment|EcsIdMatchDontFragment)) {
            if (flecs_component_sparse_has(cr, entity)) {
                return true;
            } else {
                return flecs_get_base_component(
                    world, table, component, cr, 0) != NULL;
            }
        }

        if (ECS_PAIR_FIRST(component) == EcsChildOf) {
            if (table->flags & EcsTableHasParent) {
                if (ECS_PAIR_SECOND(component) == EcsWildcard) {
                    return true;
                }

                int32_t column = table->component_map[ecs_id(EcsParent)];
                EcsParent *p = flecs_table_get_component(
                    table, column - 1, ECS_RECORD_TO_ROW(r->row)).ptr;
                return (uint32_t)p->value == ECS_PAIR_SECOND(component);
            }
        }

        can_inherit = cr->flags & EcsIdOnInstantiateInherit;
    }

    if (!(table->flags & EcsTableHasIsA)) {
        return false;
    }

    if (!can_inherit) {
        return false;
    }

    ecs_table_record_t *tr;
    int32_t column = ecs_search_relation(world, table, 0, component, 
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
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "owns");
    ecs_check(component != 0, ECS_INVALID_PARAMETER, 
        "invalid component passed to has(): component cannot be 0");

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get_any(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (component < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_lookup[component]) {
            return table->component_map[component] != 0;
        }
    }

    ecs_component_record_t *cr = flecs_components_get(world, component);
    if (cr) {
        const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
        if (tr) {
            return true;
        }

        if (cr->flags & (EcsIdDontFragment|EcsIdMatchDontFragment)) {
            return flecs_component_sparse_has(cr, entity);
        }

        if (ECS_PAIR_FIRST(component) == EcsChildOf) {
            if (table->flags & EcsTableHasParent) {
                if (ECS_PAIR_SECOND(component) == EcsWildcard) {
                    return true;
                }

                int32_t column = table->component_map[ecs_id(EcsParent)];
                EcsParent *p = flecs_table_get_component(
                    table, column - 1, ECS_RECORD_TO_ROW(r->row)).ptr;
                return (uint32_t)p->value == ECS_PAIR_SECOND(component);
            }
        }
    }

error:
    return false;
}

static
ecs_entity_t flecs_get_prefab_instance_child(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t prefab_child)
{
    ecs_map_val_t *index_ptr = ecs_map_get(
        &world->prefab_child_indices, prefab_child);
    if (!index_ptr) {
        return 0;
    }

    flecs_assert_entity_valid(world, prefab_child, "get_target");
    ecs_check(ecs_owns_id(world, prefab_child, EcsPrefab), 
        ECS_INVALID_OPERATION, 
        "cannot get target for '%s': entity is not/no longer a prefab",
        flecs_errstr(ecs_id_str(world, prefab_child)));

#ifdef FLECS_DEBUG
    ecs_entity_t prefab = ecs_get_parent(world, prefab_child);
    ecs_check(prefab != 0, ECS_INVALID_OPERATION,
        "cannot get target for '%s': entity has no parent",
        flecs_errstr(ecs_id_str(world, prefab_child)));

    ecs_check(ecs_owns_id(world, prefab, EcsPrefab), ECS_INVALID_OPERATION,
        "cannot get target for '%s': parent is not/no longer a prefab",
        flecs_errstr(ecs_id_str(world, prefab)),
        flecs_errstr_1(ecs_id_str(world, prefab_child)));

    ecs_check(ecs_has_pair(world, entity, EcsIsA, prefab),
        ECS_INVALID_OPERATION,
        "cannot get target for '%s': entity '%s' is not an instance of prefab '%s'",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)),
            flecs_errstr_2(ecs_id_str(world, prefab)));
#endif

    ecs_component_record_t *childof_cr = flecs_components_get(
        world, ecs_childof(entity));
    ecs_check(childof_cr != NULL, ECS_INVALID_OPERATION, 
        "cannot get target for '%s': children of '%s' have changed since "
        "prefab instantiation",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    ecs_vec_t *v = &childof_cr->pair->ordered_children;
    int32_t index = flecs_uto(int32_t, *index_ptr);
    ecs_check(ecs_vec_count(v) >= index, ECS_INVALID_OPERATION, 
        "cannot get target for '%s': children of '%s' have changed since "
        "prefab instantiation",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    ecs_entity_t tgt = ecs_vec_get_t(v, ecs_entity_t, index)[0];
    ecs_check(ecs_has_pair(world, tgt, EcsIsA, prefab_child), ECS_INVALID_OPERATION,
        "cannot get target for '%s': children of '%s' have changed since "
        "prefab instantiation",
            flecs_errstr(ecs_id_str(world, prefab_child)),
            flecs_errstr_1(ecs_id_str(world, entity)));

    return tgt;
error:
    return 0;
}

ecs_entity_t ecs_get_target(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    int32_t index)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_target");
    ecs_check(rel != 0, ECS_INVALID_PARAMETER, NULL);

    if (rel == EcsChildOf) {
        if (index > 0) {
            return 0;
        }
        return ecs_get_parent(world, entity);
    }

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t wc = ecs_pair(rel, EcsWildcard);
    ecs_component_record_t *cr = flecs_components_get(world, wc);
    if (!cr) {
        if (!index) {
            return flecs_get_prefab_instance_child(world, entity, rel);
        }
        return 0;
    }

    const ecs_table_record_t *tr = flecs_component_get_table(cr, table);;
    if (!tr) {
        if (cr->flags & EcsIdDontFragment) {
            if (cr->flags & EcsIdExclusive) {
                if (index > 0) {
                    return 0;
                }

                ecs_entity_t *tgt = flecs_sparse_get(cr->sparse, 0, entity);
                if (tgt) {
                    return *tgt;
                }
            } else {
                ecs_type_t *type = flecs_sparse_get(cr->sparse, 0, entity);
                if (type && (index < type->count)) {
                    return type->array[index];
                }
            }
        }

        if (cr->flags & EcsIdOnInstantiateInherit) {
            goto look_in_base;
        }

        return 0;
    }

    if (index >= tr->count) {
        index -= tr->count;
        goto look_in_base;
    }

    return ecs_pair_second(world, table->type.array[tr->index + index]);
look_in_base:
    if (table->flags & EcsTableHasIsA) {
        const ecs_table_record_t *tr_isa = flecs_component_get_table(
            world->cr_isa_wildcard, table);
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

error:
    return 0;
}

ecs_entity_t ecs_get_parent(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_parent");

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->flags & EcsTableHasParent) {
        int32_t column = table->component_map[ecs_id(EcsParent)];
        ecs_assert(column > 0, ECS_INTERNAL_ERROR, NULL);
        EcsParent *p = ecs_table_get_column(
            table, column - 1, ECS_RECORD_TO_ROW(r->row));
        ecs_assert(ecs_is_valid(world, p->value), ECS_INTERNAL_ERROR, 
            "Parent component points to invalid parent");
        return p->value;
    }

    ecs_component_record_t *cr = world->cr_childof_wildcard;
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

    const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
    if (!tr) {
        return 0;
    }

    ecs_entity_t id = table->type.array[tr->index];
    return flecs_entities_get_alive(world, ECS_PAIR_SECOND(id));
error:
    return 0;
}

ecs_entity_t ecs_new_w_parent(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(!(world->flags & EcsWorldMultiThreaded), ECS_INVALID_OPERATION,
        "cannot create new entity while world is multithreaded");

    ecs_component_record_t *cr = flecs_components_ensure(
        world, ecs_childof(parent));
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_pair_record_t *pr = cr->pair;
    ecs_assert(pr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = 0;
    if (name) {
        ecs_hashmap_t *name_index = pr->name_index;
        if (name_index) {
            entity = flecs_name_index_find(name_index, name, 0, 0);
            if (entity) {
                return entity;
            }
        }
    }

    ecs_id_t type_ids[] = {
        ecs_id(EcsParent), ecs_value_pair(EcsParentDepth, pr->depth)};

    ecs_type_t type = { .count = 2, .array = type_ids };
    ecs_table_t *table = flecs_table_find_or_create(world, &type);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    entity = flecs_new_id(world);
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_flags32_t flags = table->flags & EcsTableAddEdgeFlags;

    ecs_table_diff_t table_diff = { 
        .added = table->type,
        .added_flags = flags
    };

    int32_t row = flecs_table_append(world, table, entity, false, false);
    r->table = table;
    r->row = (uint32_t)row;

    flecs_actions_new(world, table, row, 1, &table_diff, 0, false, true);

    EcsParent *parent_ptr = table->data.columns[0].data;
    parent_ptr = &parent_ptr[row];
    parent_ptr->value = parent;

    if (name) {
        bool is_deferred = ecs_is_deferred(world);
        if (is_deferred) ecs_defer_suspend(world);
        flecs_set_identifier(world, stage, entity, EcsName, name);
        if (is_deferred) ecs_defer_resume(world);
    }

    flecs_add_non_fragmenting_child_w_records(world, parent, entity, cr, r);

    return entity;
}

ecs_entity_t ecs_get_target_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t component)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_target_for_id");

    if (!component) {
        return ecs_get_target(world, entity, rel, 0);
    }

    world = ecs_get_world(world);

    ecs_table_t *table = ecs_get_table(world, entity);
    ecs_entity_t src = 0;

    if (rel) {
        ecs_component_record_t *cr = flecs_components_get(world, component);
        if (!cr) {
            return 0;
        }

        int32_t column = ecs_search_relation_for_entity(
            world, entity, component, ecs_pair(rel, EcsWildcard), true, cr, &src, 0, 0);
        if (column == -1) {
            return 0;
        }
    } else {
        if (table) {
            ecs_id_t *ids = table->type.array;
            int32_t i, count = table->type.count;

            for (i = 0; i < count; i ++) {
                ecs_id_t ent = ids[i];
                if (ent & ECS_ID_FLAGS_MASK) {
                    /* Skip ids with pairs, roles since 0 was provided for rel */
                    break;
                }

                if (ecs_has_id(world, ent, component)) {
                    src = ent;
                    break;
                }
            }
        }
    }

    return src;
error:
    return 0;
}

int32_t ecs_get_depth(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel)
{
    ecs_check(ecs_is_valid(world, rel), ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_depth");
    ecs_check(ecs_has_id(world, rel, EcsAcyclic), ECS_INVALID_PARAMETER, 
        "cannot determine depth for non-acyclic relationship '%s' "
            "(add Acyclic trait to relationship)",
                flecs_errstr(ecs_get_path(world, rel)));

    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
        return ecs_table_get_depth(world, table, rel);
    }

    return 0;
error:
    return -1;
}

bool ecs_is_valid(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    #ifdef FLECS_DEBUG
    world = ecs_get_world(world);
    flecs_check_exclusive_world_access_read(world);
    #endif

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

    flecs_check_exclusive_world_access_read(world);

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

    flecs_check_exclusive_world_access_read(world);

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

    flecs_check_exclusive_world_access_write(world);

    /* The entity index can be mutated while in staged/readonly mode, as long as
     * the world is not multithreaded. */
    ecs_assert(!(world->flags & EcsWorldMultiThreaded), 
        ECS_INVALID_OPERATION, 
            "cannot call make_alive() while world is in multithreaded mode");

    /* Check if a version of the provided id is alive */
    ecs_entity_t current = ecs_get_alive(world, (uint32_t)entity);
    if (current == entity) {
        /* If alive and equal to the argument, there's nothing left to do */
        return;
    }

    /* If the id is currently alive but did not match the argument, fail */
    ecs_check(!current, ECS_INVALID_OPERATION, 
        "invalid call to make_alive(): entity %u is alive with different "
        "generation (%u vs %u)",
            (uint32_t)entity,
            (uint32_t)(current >> 32),
            (uint32_t)(entity >> 32));

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
    ecs_record_t *r = flecs_entities_ensure(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table == NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t row = flecs_table_append(
        world, &world->store.root, entity, false, false);
    r->table = &world->store.root;
    r->row = ECS_ROW_TO_RECORD(row, r->row & ECS_ROW_FLAGS_MASK);
error:
    return;
}

void ecs_make_alive_id(
    ecs_world_t *world,
    ecs_id_t component)
{
    flecs_poly_assert(world, ecs_world_t);

    if (ECS_HAS_ID_FLAG(component, PAIR)) {
        ecs_entity_t r = ECS_PAIR_FIRST(component);
        ecs_entity_t t = ECS_PAIR_SECOND(component);

        ecs_check(r != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_check(t != 0, ECS_INVALID_PARAMETER, NULL);

        if (flecs_entities_get_alive(world, r) == 0) {
            ecs_assert(!ecs_exists(world, r), ECS_INVALID_PARAMETER, 
                "first element of pair is not alive");
            ecs_make_alive(world, r);
        }

        if (!ECS_IS_VALUE_PAIR(component)) {
            if (flecs_entities_get_alive(world, t) == 0) {
                ecs_assert(!ecs_exists(world, t), ECS_INVALID_PARAMETER,
                    "second element of pair is not alive");
                ecs_make_alive(world, t);
            }
        }
    } else {
        ecs_make_alive(world, component & ECS_COMPONENT_MASK);
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

    flecs_check_exclusive_world_access_read(world);

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
        "cannot change generation for entity %u when world is in readonly mode",
            (uint32_t)entity_with_generation);
    ecs_assert(!(ecs_is_deferred(world)), ECS_INVALID_OPERATION, 
        "cannot change generation for entity %u while world is deferred",
            (uint32_t)entity_with_generation);

    flecs_check_exclusive_world_access_write(world);

    flecs_entities_make_alive(world, entity_with_generation);

    if (flecs_entities_is_alive(world, entity_with_generation)) {
        ecs_record_t *r = flecs_entities_get(world, entity_with_generation);
        if (r && r->table) {
            int32_t row = ECS_RECORD_TO_ROW(r->row);
            ecs_entity_t *entities = r->table->data.entities;
            entities[row] = entity_with_generation;
        }
    }
}


uint32_t ecs_get_version(
    ecs_entity_t entity)
{
    return flecs_uto(uint32_t, ECS_GENERATION(entity));
}

ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_table");
    ecs_check(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    
    world = ecs_get_world(world);

    flecs_check_exclusive_world_access_read(world);

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
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "get_type");
    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
        return &table->type;
    }

error:
    return NULL;
}

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_assert_entity_valid(world, entity, "enable");

    if (ecs_has_id(world, entity, EcsPrefab)) {
        /* If entity is a type, enable/disable all entities in the type */
        const ecs_type_t *type = ecs_get_type(world, entity);
        ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_id_t *ids = type->array;
        int32_t i, count = type->count;
        for (i = 0; i < count; i ++) {
            ecs_id_t component = ids[i];
            ecs_flags32_t flags = ecs_id_get_flags(world, component);
            if (!(flags & EcsIdOnInstantiateDontInherit)){
                ecs_enable(world, component, enabled);
            }
        }
    } else {
        if (enabled) {
            ecs_remove_id(world, entity, EcsDisabled);
        } else {
            ecs_add_id(world, entity, EcsDisabled);
        }
    }
error:
    return;
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

char* ecs_entity_str(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    flecs_assert_entity_valid(world, entity, "entity_str");

    ecs_get_path_w_sep_buf(world, 0, entity, 0, "", &buf, false);
    
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

ecs_table_range_t flecs_range_from_entity(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_record_t *r = flecs_entities_get(world, e);
    if (!r) {
        return (ecs_table_range_t){ 0 };
    }
    return (ecs_table_range_t){
        .table = r->table,
        .offset = ECS_RECORD_TO_ROW(r->row),
        .count = 1
    };
}
