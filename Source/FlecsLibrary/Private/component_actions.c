/**
 * @file component_actions.c
 * @brief Logic executed after adding/removing a component.
 * 
 * After a component is added to an entity there can be additional things that
 * need to be done, such as:
 * 
 * - Invoking hooks
 * - Notifying observers
 * - Updating sparse storage
 * - Update name lookup index
 */

#include "private_api.h"

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
    int32_t defer = world->stages[0]->defer;
    if (defer < 0) {
        world->stages[0]->defer *= -1;
    }

    ecs_iter_t it = { .field_count = 1};
    it.entities = entities;

    ecs_table_record_t dummy_tr;
    if (!tr) {
        dummy_tr.hdr.cache = ECS_CONST_CAST(ecs_table_cache_t*, cr);
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
    it.row_fields = !!(((ecs_component_record_t*)tr->hdr.cache)->flags & EcsIdIsSparse);
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

static
void flecs_on_reparent(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count)
{    
    flecs_reparent_name_index(world, other_table, table, row, count);
    flecs_ordered_children_reparent(world, other_table, table, row, count);
}

static
void flecs_on_unparent(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count)
{
    flecs_unparent_name_index(world, table, row, count);
    flecs_ordered_children_unparent(world, table, row, count);
}

bool flecs_sparse_on_add_cr(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr,
    bool construct,
    void **ptr_out)
{
    bool is_new = false;

    if (cr && cr->flags & EcsIdIsSparse) {
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
        if (cr && cr->flags & EcsIdIsSparse) {
            for (j = 0; j < count; j ++) {
                flecs_component_sparse_remove(world, cr, table, row + j);
            }
        }
    }
}

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
        ecs_assert(cur->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
        if (cur->sparse && !(ecs_id_is_wildcard(cur->id))) {
            if (flecs_sparse_has(cur->sparse, e)) {
                ecs_type_t type = { .count = 1, .array = &cur->id };

                flecs_emit(world, world, 0, &(ecs_event_desc_t) {
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

void flecs_notify_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t flags,
    ecs_flags64_t *set_mask,
    bool construct,
    bool sparse)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_t *added = &diff->added;

    if (added->count) {
        ecs_flags32_t diff_flags = 
            diff->added_flags|(table->flags & EcsTableHasTraversable);
        if (!diff_flags) {
            return;
        }

        if (diff_flags & EcsTableEdgeReparent) {
            flecs_on_reparent(world, table, other_table, row, count);
        }

        if (sparse && (diff_flags & EcsTableHasSparse)) {
            if (flecs_sparse_on_add(world, table, row, count, added, construct)) {
                diff_flags |= EcsTableHasOnAdd;
            }
        }

        if (diff_flags & (EcsTableHasOnAdd|EcsTableHasTraversable)) {
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
    const ecs_table_diff_t *diff)
{
    ecs_assert(diff != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_t *removed = &diff->removed;
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    if (removed->count) {
        ecs_flags32_t diff_flags = 
            diff->removed_flags|(table->flags & EcsTableHasTraversable);
        if (!diff_flags) {
            return;
        }

        if (diff_flags & (EcsTableEdgeReparent|EcsTableHasOrderedChildren)) {
            flecs_on_unparent(world, table, row, count);
        }

        if (diff_flags & EcsTableHasDontFragment) {
            if (flecs_dont_fragment_on_remove(
                world, table, row, count, removed)) 
            {
                diff_flags |= EcsTableHasOnRemove;
            }
        }

        if (diff_flags & (EcsTableHasOnRemove|EcsTableHasTraversable)) {
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

        if (diff_flags & EcsTableHasSparse) {
            flecs_sparse_on_remove(world, table, row, count, removed);
        }
    }
}

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
            dummy_tr.hdr.cache = (ecs_table_cache_t*)cr;
            dummy_tr.hdr.table = table;
            dummy_tr.column = -1;
            dummy_tr.index = -1;
            dummy_tr.count = 0;
            tr = &dummy_tr;
        }

        if (cr->flags & EcsIdIsSparse) {
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

    /* Run OnSet notifications */
    if ((dont_fragment || table->flags & EcsTableHasOnSet) && ids->count) {
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
    bool dont_fragment = (cr->flags & EcsIdDontFragment) != 0;

    if (invoke_hook) {        
        const ecs_type_info_t *ti = cr->type_info;
        ecs_iter_action_t on_set = ti->hooks.on_set;
        if (on_set) {
            ecs_table_record_t dummy_tr;
            const ecs_table_record_t *tr = 
            flecs_component_get_table(cr, table);
            if (!tr) {
                dummy_tr.hdr.cache = (ecs_table_cache_t*)cr;
                dummy_tr.hdr.table = table;
                dummy_tr.column = -1;
                dummy_tr.index = -1;
                dummy_tr.count = 0;
                tr = &dummy_tr;
            }
    
            if (cr->flags & EcsIdIsSparse) {
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

    /* Run OnSet notifications */
    if ((dont_fragment || table->flags & EcsTableHasOnSet)) {
        ecs_type_t ids = { .array = &id, .count = 1 };
        flecs_emit(world, world, 0, &(ecs_event_desc_t) {
            .event = EcsOnSet,
            .ids = &ids,
            .table = table,
            .offset = row,
            .count = 1,
            .observable = world
        });
    }
}
