/**
 * @file component_actions.c
 * @brief Post-add/remove actions: hooks, observers, sparse storage, name index.
 */

#include "private_api.h"

/* Invoke a component lifecycle hook (on_add, on_set, on_remove).
 * Temporarily unsuspends deferring so hooks can enqueue commands. */
void flecs_invoke_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_component_record_t *cr,
    const ecs_table_record_t *tr,
    int32_t count,
    int32_t row,
    const ecs_entity_t *entities,
    ecs_id_t id,
    const ecs_type_info_t *ti,
    ecs_entity_t event,
    ecs_iter_action_t hook)
{
    /* Unsuspend defer (negative -> positive) so the hook can enqueue
     * commands. Original defer depth is restored after the hook returns. */
    int32_t defer = world->stages[0]->defer;
    if (defer < 0) {
        world->stages[0]->defer *= -1;
    }

    ecs_iter_t it = { .field_count = 1 };
    it.entities = entities;

    ecs_table_record_t dummy_tr;
    if (!tr) {
        dummy_tr.hdr.cr = ECS_CONST_CAST(ecs_component_record_t*, cr);
        dummy_tr.hdr.table = table;
        dummy_tr.index = -1;
        dummy_tr.column = -1;
        dummy_tr.count = 0;
        tr = &dummy_tr;
    }

    ecs_entity_t dummy_src = 0;

    it.world = world;
    it.real_world = world;
    it.table = table;
    it.trs = &tr;
    it.row_fields = !!(tr->hdr.cr->flags & EcsIdSparse);
    it.ref_fields = it.row_fields;
    it.sizes = ECS_CONST_CAST(ecs_size_t*, &ti->size);
    it.ids = &id;
    it.sources = &dummy_src;
    it.event = event;
    it.event_id = id;
    it.ctx = ti->hooks.ctx;
    it.callback_ctx = ti->hooks.binding_ctx;
    it.count = count;
    it.offset = row;
    it.flags = EcsIterIsValid;

    hook(&it);

    world->stages[0]->defer = defer;
}

/* Invoke on_replace hook with both old and new component values.
 * Uses field_count=2: field[0]=old, field[1]=new. */
void flecs_invoke_replace_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_id_t id,
    const void *old_ptr,
    const void *new_ptr,
    const ecs_type_info_t *ti)
{
    /* Unsuspend defer for the hook, same as flecs_invoke_hook */
    int32_t defer = world->stages[0]->defer;
    if (defer < 0) {
        world->stages[0]->defer *= -1;
    }

    ecs_iter_t it = { .field_count = 2 };
    it.entities = &entity;

    const ecs_table_record_t *trs[] = {NULL, NULL};
    ecs_size_t sizes[] = {ti->size, ti->size};
    ecs_id_t ids[] = {id, id};
    ecs_entity_t srcs[] = {0, 0};
    const void *ptrs[] = {old_ptr, new_ptr};

    it.world = world;
    it.real_world = world;
    it.table = table;
    it.trs = trs;
    it.row_fields = 0;
    it.ref_fields = it.row_fields;
    it.sizes = sizes;
    it.ptrs = ECS_CONST_CAST(void**, ptrs);
    it.ids = ids;
    it.sources = srcs;
    it.event = 0;
    it.event_id = id;
    it.ctx = ti->hooks.ctx;
    it.callback_ctx = ti->hooks.binding_ctx;
    it.count = 1;
    it.offset = 0; /* ptrs are already resolved; no row offset needed */
    it.flags = EcsIterIsValid;

    ti->hooks.on_replace(&it);

    world->stages[0]->defer = defer;
}

/* Handle reparenting: update name index, ordered children, non-fragmenting. */
static
void flecs_on_reparent(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count)
{
    flecs_reparent_name_index(world, table, other_table, row, count);
    flecs_ordered_children_reparent(world, table, other_table, row, count);
    flecs_non_fragmenting_childof_reparent(world, table, other_table, row, count);
}

/* Handle unparenting: clean up name index, ordered children, non-fragmenting. */
static
void flecs_on_unparent(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count)
{
    if (other_table) {
        flecs_unparent_name_index(world, table, other_table, row, count);
    }
    flecs_ordered_children_unparent(world, table, row, count);
    flecs_non_fragmenting_childof_unparent(world, other_table, table, row, count);
}

/* Initialize sparse storage for a single component on add. */
bool flecs_sparse_on_add_cr(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr,
    bool construct,
    void **ptr_out)
{
    bool is_new = false;

    if (cr && cr->flags & EcsIdSparse) {
        void *result = NULL;
        int32_t sparse_count = flecs_sparse_count(cr->sparse);

        if (construct) {
            result = flecs_component_sparse_insert(
                world, cr, table, row);
        } else {
            result = flecs_component_sparse_emplace(
                world, cr, table, row);
        }

        if (ptr_out)  {
            *ptr_out = result;
        }

        if (cr->flags & EcsIdDontFragment) {
            is_new = sparse_count != flecs_sparse_count(cr->sparse);
            if (is_new) {
                const ecs_entity_t *entities = ecs_table_entities(table);
                ecs_record_t *r = flecs_entities_get(world, entities[row]);
                r->row |= EcsEntityHasDontFragment;
            }
        }
    }

    return is_new;
}

/* Initialize sparse storage for all added sparse components. */
bool flecs_sparse_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *added,
    bool construct)
{
    bool is_new = false;

    int32_t i, j;
    for (i = 0; i < added->count; i ++) {
        ecs_id_t id = added->array[i];
        ecs_component_record_t *cr = flecs_components_get(world, id);

        for (j = 0; j < count; j ++) {
            is_new |= flecs_sparse_on_add_cr(
                world, table, row + j, cr, construct, NULL);
        }
    }

    return is_new;
}

/* Remove sparse storage entries for removed sparse components. */
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
        ecs_component_record_t *cr = flecs_components_get(world, id);
        if (cr && cr->flags & EcsIdSparse) {
            for (j = 0; j < count; j ++) {
                flecs_component_sparse_remove(world, cr, table, row + j);
            }
        }
    }
}

/* Check if any removed ids have non-fragmenting sparse data on these entities. */
static
bool flecs_dont_fragment_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *removed)
{
    int32_t i, j;
    for (i = 0; i < removed->count; i ++) {
        ecs_id_t id = removed->array[i];
        ecs_component_record_t *cr = flecs_components_get(world, id);
        if (cr && cr->flags & EcsIdDontFragment) {
            const ecs_entity_t *entities = ecs_table_entities(table);
            for (j = 0; j < count; j ++) {
                ecs_entity_t e = entities[row + j];
                if (flecs_component_sparse_has(cr, e)) {
                    return true;
                }
            }
        }
    }

    return false;
}

/* Remove all non-fragmenting sparse components from an entity. */
void flecs_entity_remove_non_fragmenting(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_record_t *r)
{
    if (!r) {
        r = flecs_entities_get(world, e);
    }

    if (!r || !(r->row & EcsEntityHasDontFragment)) {
        return;
    }

    ecs_component_record_t *cur = world->cr_non_fragmenting_head;
    while (cur) {
        ecs_assert(cur->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
        if (cur->sparse && !(ecs_id_is_wildcard(cur->id))) {
            if (flecs_sparse_has(cur->sparse, e)) {
                ecs_type_t type = { .count = 1, .array = &cur->id };

                flecs_emit(world, world, &(ecs_event_desc_t) {
                    .event = EcsOnRemove,
                    .ids = &type,
                    .table = r->table,
                    .other_table = r->table,
                    .offset = ECS_RECORD_TO_ROW(r->row),
                    .count = 1,
                    .observable = world
                });

                flecs_component_sparse_remove(
                    world, cur, r->table, ECS_RECORD_TO_ROW(r->row));
            }
        }

        cur = cur->non_fragmenting.next;
    }

    r->row &= ~EcsEntityHasDontFragment;
}

/* Run on-add actions: reparenting, sparse storage init, event emission. */
static
void flecs_actions_on_add_intern(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t flags,
    bool construct,
    bool sparse)
{
    ecs_flags32_t diff_flags = diff->added_flags;
    if (!diff_flags) {
        return;
    }

    const ecs_type_t *added = &diff->added;

    if (diff_flags & EcsTableEdgeReparent) {
        flecs_on_reparent(world, table, other_table, row, count);
    }

    if (sparse && (diff_flags & EcsTableHasSparse)) {
        if (flecs_sparse_on_add(world, table, row, count, added, construct)) {
            diff_flags |= EcsTableHasOnAdd;
        }
    }

    if (diff_flags & (EcsTableHasOnAdd|EcsTableHasTraversable)) {
        flecs_emit(world, world, &(ecs_event_desc_t){
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

/* Run on-remove actions: event emission and sparse storage cleanup. */
static
void flecs_actions_on_remove_intern(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t diff_flags)
{
    const ecs_type_t *removed = &diff->removed;

    ecs_assert(diff_flags != 0, ECS_INTERNAL_ERROR, NULL);

    if (diff_flags & EcsTableHasDontFragment) {
        if (flecs_dont_fragment_on_remove(
            world, table, row, count, removed)) 
        {
            diff_flags |= EcsTableHasOnRemove;
        }
    }

    if (diff_flags & EcsTableHasOnRemove) {
        flecs_emit(world, world, &(ecs_event_desc_t) {
            .event = EcsOnRemove,
            .ids = removed,
            .table = table,
            .other_table = other_table,
            .offset = row,
            .count = count,
            .observable = world
        });
    }

    if (diff_flags & EcsTableHasSparse) {
        flecs_sparse_on_remove(world, table, row, count, removed);
    }
}

/* Run on-remove actions, including unparenting when entity loses its parent. */
static
void flecs_actions_on_remove_intern_w_reparent(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff)
{
    if (!(world->flags & EcsWorldFini)) {
        ecs_check(!(table->flags & EcsTableHasBuiltins), 
            ECS_INVALID_OPERATION,
            "removing components from builtin entities is not allowed");
    }

    ecs_flags32_t diff_flags = diff->removed_flags;
    if (!diff_flags) {
        return;
    }

    if (diff_flags & (EcsTableEdgeReparent|EcsTableHasOrderedChildren)) {
        if (!other_table || !(other_table->flags & EcsTableHasChildOf)) {
            flecs_on_unparent(world, table, other_table, row, count);
        }
    }

    flecs_actions_on_remove_intern(
        world, table, other_table, row, count, diff, diff_flags);

error:
    return;
}

/* Run actions for a newly created entity in a table. */
void flecs_actions_new(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t flags,
    bool construct,
    bool sparse)
{
    flecs_actions_on_add_intern(
        world, table, NULL, row, count, diff, flags, construct, sparse);
}

/* Run remove actions when deleting an entity. */
void flecs_actions_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff)
{
    if (diff->removed.count) {
        flecs_actions_on_remove_intern_w_reparent(
            world, table, NULL, row, count, diff);
    }
}

/* Run remove actions during recursive tree deletion (skips unparenting). */
void flecs_actions_delete_tree(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff)
{
    if (diff->removed.count) {
        ecs_flags32_t diff_flags = diff->removed_flags;
        if (!diff_flags) {
            return;
        }

        flecs_actions_on_remove_intern(
            world, table, NULL, row, count, diff, diff_flags);
    }
}

/* Run on-add actions after an entity moves to a new table with added components. */
void flecs_actions_move_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t flags,
    bool construct,
    bool sparse)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_t *added = &diff->added;

    if (added->count) {
        ecs_flags32_t table_flags = table->flags;

        if (table_flags & EcsTableHasTraversable) {
            flecs_emit_propagate_invalidate(world, table, row, count);
        }

        if (table_flags & EcsTableHasParent) {
            flecs_on_non_fragmenting_child_move_add(
                world, table, other_table, row, count);
        }

        flecs_actions_on_add_intern(world, table, other_table, row, count, diff, 
            flags, construct, sparse);
    }
}

/* Run on-remove actions when an entity moves to a table with fewer components. */
void flecs_actions_move_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    if (diff->removed.count) {
        ecs_flags32_t table_flags = table->flags;
        if (table_flags & EcsTableHasTraversable) {
            flecs_emit_propagate_invalidate(world, table, row, count);
        }

        if (table_flags & EcsTableHasParent) {
            bool update_parent_records = true;
            if (diff->added.count && (table->flags & EcsTableHasParent)) {
                update_parent_records = false;
            }

            flecs_on_non_fragmenting_child_move_remove(
                world, other_table, table, row, count, update_parent_records);
        }

        flecs_actions_on_remove_intern_w_reparent(
            world, table, other_table, row, count, diff);
    }
}

/* Run on-set hooks and emit OnSet events for multiple component ids. */
void flecs_notify_on_set_ids(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_type_t *ids)
{
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_entity_t *entities = &ecs_table_entities(table)[row];
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_table_count(table), 
        ECS_INTERNAL_ERROR, NULL);

    bool dont_fragment = false;

    int i;
    for (i = 0; i < ids->count; i ++) {
        ecs_id_t id = ids->array[i];
        ecs_component_record_t *cr = flecs_components_get(world, id);
        dont_fragment |= (cr->flags & EcsIdDontFragment) != 0;
        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
        const ecs_type_info_t *ti = cr->type_info;
        ecs_iter_action_t on_set = ti->hooks.on_set;
        if (!on_set) {
            continue;
        }

        ecs_table_record_t dummy_tr;
        const ecs_table_record_t *tr = 
        flecs_component_get_table(cr, table);
        if (!tr) {
            dummy_tr.hdr.cr = cr;
            dummy_tr.hdr.table = table;
            dummy_tr.column = -1;
            dummy_tr.index = -1;
            dummy_tr.count = 0;
            tr = &dummy_tr;
        }

        if (cr->flags & EcsIdSparse) {
            int32_t j;
            for (j = 0; j < count; j ++) {
                flecs_invoke_hook(world, table, cr, tr, 1, row, 
                    &entities[j], id, ti, EcsOnSet, on_set);
            }
        } else {
            ecs_assert(tr->column != -1, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);
            if (on_set) {
                flecs_invoke_hook(world, table, cr, tr, count, row, 
                    entities, id, ti, EcsOnSet, on_set);
            }
        }
    }

    /* Emit OnSet event for observers */
    if ((dont_fragment || table->flags & EcsTableHasOnSet) && ids->count) {
        flecs_emit(world, world, &(ecs_event_desc_t) {
            .event = EcsOnSet,
            .ids = ids,
            .table = table,
            .offset = row,
            .count = count,
            .observable = world
        });
    }
}

/* Run on-set hook and emit OnSet event for a single component id. */
void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t id,
    bool invoke_hook)
{
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    const ecs_entity_t *entities = &ecs_table_entities(table)[row];
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row <= ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);

    ecs_component_record_t *cr = flecs_components_get(world, id);
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    bool dont_fragment = (cr->flags & EcsIdDontFragment) != 0;

    if (invoke_hook) {        
        const ecs_type_info_t *ti = cr->type_info;
        ecs_iter_action_t on_set = ti->hooks.on_set;
        if (on_set) {
            ecs_table_record_t dummy_tr;
            const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
            if (!tr) {
                dummy_tr.hdr.cr = cr;
                dummy_tr.hdr.table = table;
                dummy_tr.column = -1;
                dummy_tr.index = -1;
                dummy_tr.count = 0;
                tr = &dummy_tr;
            }
    
            if (cr->flags & EcsIdSparse) {
                flecs_invoke_hook(world, table, cr, tr, 1, row, 
                    entities, id, ti, EcsOnSet, on_set);
            } else {
                ecs_assert(tr->column != -1, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);
                if (on_set) {
                    flecs_invoke_hook(world, table, cr, tr, 1, row, 
                        entities, id, ti, EcsOnSet, on_set);
                }
            }
        }
    }

    if ((dont_fragment || table->flags & EcsTableHasOnSet)) {
        ecs_type_t ids = { .array = &id, .count = 1 };
        flecs_emit(world, world, &(ecs_event_desc_t) {
            .event = EcsOnSet,
            .ids = &ids,
            .table = table,
            .offset = row,
            .count = 1,
            .observable = world
        });
    }
}
