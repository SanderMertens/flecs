/**
 * @file observable.c
 * @brief Observable implementation.
 * 
 * The observable implementation contains functions that find the set of 
 * observers to invoke for an event. The code also contains the implementation
 * of a reachable id cache, which is used to speedup event propagation when
 * relationships are added/removed to/from entities.
 */

#include "private_api.h"

void flecs_observable_init(
    ecs_observable_t *observable)
{
    flecs_sparse_init_t(&observable->events, NULL, NULL, ecs_event_record_t);
    observable->on_add.event = EcsOnAdd;
    observable->on_remove.event = EcsOnRemove;
    observable->on_set.event = EcsOnSet;
    observable->un_set.event = EcsUnSet;
}

void flecs_observable_fini(
    ecs_observable_t *observable)
{
    ecs_assert(!ecs_map_is_init(&observable->on_add.event_ids), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!ecs_map_is_init(&observable->on_remove.event_ids), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!ecs_map_is_init(&observable->on_set.event_ids), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!ecs_map_is_init(&observable->un_set.event_ids), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_sparse_t *events = &observable->events;
    int32_t i, count = flecs_sparse_count(events);
    for (i = 0; i < count; i ++) {
        ecs_event_record_t *er = 
            flecs_sparse_get_dense_t(events, ecs_event_record_t, i);
        ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);
        (void)er;

        /* All triggers should've unregistered by now */
        ecs_assert(!ecs_map_is_init(&er->event_ids), 
            ECS_INTERNAL_ERROR, NULL);
    }

    flecs_sparse_fini(&observable->events);
}

ecs_event_record_t* flecs_event_record_get(
    const ecs_observable_t *o,
    ecs_entity_t event)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    
    /* Builtin events*/
    if      (event == EcsOnAdd)     return (ecs_event_record_t*)&o->on_add;
    else if (event == EcsOnRemove)  return (ecs_event_record_t*)&o->on_remove;
    else if (event == EcsOnSet)     return (ecs_event_record_t*)&o->on_set;
    else if (event == EcsUnSet)     return (ecs_event_record_t*)&o->un_set;
    else if (event == EcsWildcard)  return (ecs_event_record_t*)&o->on_wildcard;

    /* User events */
    return flecs_sparse_try_t(&o->events, ecs_event_record_t, event);
}

ecs_event_record_t* flecs_event_record_ensure(
    ecs_observable_t *o,
    ecs_entity_t event)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_event_record_t *er = flecs_event_record_get(o, event);
    if (er) {
        return er;
    }
    er = flecs_sparse_ensure_t(&o->events, ecs_event_record_t, event);
    er->event = event;
    return er;
}

static
ecs_event_record_t* flecs_event_record_get_if(
    const ecs_observable_t *o,
    ecs_entity_t event)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_event_record_t *er = flecs_event_record_get(o, event);
    if (er) {
        if (ecs_map_is_init(&er->event_ids)) {
            return er;
        }
        if (er->any) {
            return er;
        }
        if (er->wildcard) {
            return er;
        }
        if (er->wildcard_pair) {
            return er;
        }
    }

    return NULL;
}

ecs_event_id_record_t* flecs_event_id_record_get(
    const ecs_event_record_t *er,
    ecs_id_t id)
{
    if (!er) {
        return NULL;
    }

    if (id == EcsAny)                                  return er->any;
    else if (id == EcsWildcard)                        return er->wildcard;
    else if (id == ecs_pair(EcsWildcard, EcsWildcard)) return er->wildcard_pair;
    else {
        if (ecs_map_is_init(&er->event_ids)) {
            return ecs_map_get_deref(&er->event_ids, ecs_event_id_record_t, id);
        }
        return NULL;
    }
}

static
ecs_event_id_record_t* flecs_event_id_record_get_if(
    const ecs_event_record_t *er,
    ecs_id_t id)
{
    ecs_event_id_record_t *ider = flecs_event_id_record_get(er, id);
    if (!ider) {
        return NULL;
    }

    if (ider->observer_count) {
        return ider;
    }

    return NULL;
}

ecs_event_id_record_t* flecs_event_id_record_ensure(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_id_t id)
{
    ecs_event_id_record_t *ider = flecs_event_id_record_get(er, id);
    if (ider) {
        return ider;
    }

    ider = ecs_os_calloc_t(ecs_event_id_record_t);

    if (id == EcsAny) {
        return er->any = ider;
    } else if (id == EcsWildcard) {
        return er->wildcard = ider;
    } else if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
        return er->wildcard_pair = ider;
    }

    ecs_map_init_w_params_if(&er->event_ids, &world->allocators.ptr);
    ecs_map_insert_ptr(&er->event_ids, id, ider);
    return ider;
}

void flecs_event_id_record_remove(
    ecs_event_record_t *er,
    ecs_id_t id)
{
    if (id == EcsAny) {
        er->any = NULL;
    } else if (id == EcsWildcard) {
        er->wildcard = NULL;
    } else if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
        er->wildcard_pair = NULL;
    } else {
        ecs_map_remove(&er->event_ids, id);
        if (!ecs_map_count(&er->event_ids)) {
            ecs_map_fini(&er->event_ids);
        }
    }
}

static
int32_t flecs_event_observers_get(
    const ecs_event_record_t *er,
    ecs_id_t id,
    ecs_event_id_record_t **iders)
{
    if (!er) {
        return 0;
    }

    /* Populate array with observer sets matching the id */
    int32_t count = 0;
    iders[0] = flecs_event_id_record_get_if(er, EcsAny);
    count += iders[count] != 0;

    iders[count] = flecs_event_id_record_get_if(er, id);
    count += iders[count] != 0;

    if (ECS_IS_PAIR(id)) {
        ecs_id_t id_fwc = ecs_pair(EcsWildcard, ECS_PAIR_SECOND(id));
        ecs_id_t id_swc = ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard);
        ecs_id_t id_pwc = ecs_pair(EcsWildcard, EcsWildcard);
        iders[count] = flecs_event_id_record_get_if(er, id_fwc);
        count += iders[count] != 0;
        iders[count] = flecs_event_id_record_get_if(er, id_swc);
        count += iders[count] != 0;
        iders[count] = flecs_event_id_record_get_if(er, id_pwc);
        count += iders[count] != 0;
    } else {
        iders[count] = flecs_event_id_record_get_if(er, EcsWildcard);
        count += iders[count] != 0;
    }

    return count;
}

bool flecs_check_observers_for_event(
    const ecs_poly_t *object,
    ecs_id_t id,
    ecs_entity_t event)
{    
    ecs_observable_t *observable = ecs_get_observable(object);
    ecs_event_record_t *er = flecs_event_record_get_if(observable, event);
    if (!er) {
        return false;
    }

    return flecs_event_id_record_get_if(er, id) != NULL;
}

static
void flecs_emit_propagate(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_id_record_t *idr,
    ecs_id_record_t *tgt_idr,
    ecs_event_id_record_t **iders,
    int32_t ider_count)
{
    ecs_assert(tgt_idr != NULL, ECS_INTERNAL_ERROR, NULL);    

    if (ecs_should_log_3()) {
        char *idstr = ecs_id_str(world, tgt_idr->id);
        ecs_dbg_3("propagate events/invalidate cache for %s", idstr);
        ecs_os_free(idstr);
    }
    ecs_log_push_3();

    /* Propagate to records of acyclic relationships */
    ecs_id_record_t *cur = tgt_idr;
    while ((cur = cur->acyclic.next)) {
        cur->reachable.generation ++; /* Invalidate cache */

        ecs_table_cache_iter_t idt;
        if (!flecs_table_cache_all_iter(&cur->cache, &idt)) {
            continue;
        }

        /* Get traversed relationship */
        ecs_entity_t trav = ECS_PAIR_FIRST(cur->id);

        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&idt, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            if (!ecs_table_count(table)) {
                continue;
            }

            bool owned = flecs_id_record_get_table(idr, table);

            int32_t e, entity_count = ecs_table_count(table);
            it->table = table;
            it->other_table = NULL;
            it->offset = 0;
            it->count = entity_count;
            if (entity_count) {
                it->entities = ecs_vec_first(&table->data.entities);
            }

            /* Treat as new event as this could invoke observers again for
             * different tables. */
            world->event_id ++;

            int32_t ider_i;
            for (ider_i = 0; ider_i < ider_count; ider_i ++) {
                ecs_event_id_record_t *ider = iders[ider_i];
                flecs_observers_invoke(world, &ider->up, it, table, trav);

                if (!owned) {
                    /* Owned takes precedence */
                    flecs_observers_invoke(
                        world, &ider->self_up, it, table, trav);
                }
            }

            if (!table->observed_count) {
                continue;
            }

            ecs_record_t **records = ecs_vec_first(&table->data.records);
            for (e = 0; e < entity_count; e ++) {
                ecs_id_record_t *idr_t = records[e]->idr;
                if (idr_t) {
                    /* Only notify for entities that are used in pairs with
                     * acyclic relationships */
                    flecs_emit_propagate(world, it, idr, idr_t,
                        iders, ider_count);
                }
            }
        }
    }

    ecs_log_pop_3();
}

static
void flecs_emit_propagate_invalidate_tables(
    ecs_world_t *world,
    ecs_id_record_t *tgt_idr)
{
    ecs_assert(tgt_idr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ecs_should_log_3()) {
        char *idstr = ecs_id_str(world, tgt_idr->id);
        ecs_dbg_3("invalidate reachable cache for %s", idstr);
        ecs_os_free(idstr);
    }

    /* Invalidate records of acyclic relationships */
    ecs_id_record_t *cur = tgt_idr;
    while ((cur = cur->acyclic.next)) {
        ecs_reachable_cache_t *rc = &cur->reachable;
        if (rc->current != rc->generation) {
            /* Subtree is already marked invalid */
            continue;
        }

        rc->generation ++;

        ecs_table_cache_iter_t idt;
        if (!flecs_table_cache_all_iter(&cur->cache, &idt)) {
            continue;
        }

        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&idt, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            if (!table->observed_count) {
                continue;
            }

            int32_t e, entity_count = ecs_table_count(table);
            ecs_record_t **records = ecs_vec_first(&table->data.records);

            for (e = 0; e < entity_count; e ++) {
                ecs_id_record_t *idr_t = records[e]->idr;
                if (idr_t) {
                    /* Only notify for entities that are used in pairs with
                     * acyclic relationships */
                    flecs_emit_propagate_invalidate_tables(world, idr_t);
                }
            }
        }
    }
}

void flecs_emit_propagate_invalidate(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t offset,
    int32_t count)
{
    ecs_record_t **recs = ecs_vec_get_t(&table->data.records, 
        ecs_record_t*, offset);
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_record_t *record = recs[i];
        if (!record) {
            /* If the event is emitted after a bulk operation, it's possible
             * that it hasn't been populated with entities yet. */
            continue;
        }

        ecs_id_record_t *idr_t = record->idr;
        if (idr_t) {
            /* Event is used as target in acyclic relationship, propagate */
            flecs_emit_propagate_invalidate_tables(world, idr_t);
        }
    }
}

static
void flecs_override_copy(
    const ecs_type_info_t *ti,
    void *dst,
    const void *src,
    int32_t count)
{
    ecs_copy_t copy = ti->hooks.copy;
    ecs_size_t size = ti->size;
    int32_t i;
    if (copy) {
        for (i = 0; i < count; i ++) {
            copy(dst, src, count, ti);
            dst = ECS_OFFSET(dst, size);
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_os_memcpy(dst, src, size);
            dst = ECS_OFFSET(dst, size);
        }
    }
}

static
void* flecs_override(
    ecs_iter_t *it, 
    const ecs_type_t *emit_ids,
    ecs_id_t id,
    ecs_table_t *table,
    ecs_id_record_t *idr)
{
    if (it->event != EcsOnAdd || (it->flags & EcsEventNoOnSet)) {
        return NULL;
    }

    int32_t i = 0, count = emit_ids->count;
    ecs_id_t *ids = emit_ids->array;
    for (i = 0; i < count; i ++) {
        if (ids[i] == id) {
            /* If an id was both inherited and overridden in the same event
             * (like what happens during an auto override), we need to copy the
             * value of the inherited component to the new component.
             * Also flag to the callee that this component was overridden, so
             * that an OnSet event can be emmitted for it.
             * Note that this is different from a component that was overridden
             * after it was inherited, as this does not change the actual value
             * of the component for the entity (it is copied from the existing
             * overridden component), and does not require an OnSet event. */
            const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
            if (!tr) {
                continue;
            }

            int32_t column = tr->column;
            column = ecs_table_type_to_storage_index(table, column);
            ecs_assert(column != -1, ECS_INTERNAL_ERROR, NULL);

            const ecs_type_info_t *ti = idr->type_info;
            ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_size_t size = ti->size;
            ecs_vec_t *vec = &table->data.columns[column];
            return ecs_vec_get(vec, size, it->offset);
        }
    }

    return NULL;
}

static
void flecs_emit_forward_up(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_id_record_t *idr,
    ecs_vec_t *stack,
    ecs_vec_t *reachable_ids);

static
void flecs_emit_forward_id(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_id_record_t *idr,
    ecs_entity_t tgt,
    ecs_table_t *tgt_table,
    int32_t column,
    int32_t offset,
    ecs_entity_t trav)
{
    ecs_id_t id = idr->id;
    ecs_entity_t event = er ? er->event : 0;
    bool inherit = trav == EcsIsA;
    bool may_override = inherit && (event == EcsOnAdd) && (emit_ids->count > 1);
    ecs_event_id_record_t *iders[5];
    ecs_event_id_record_t *iders_onset[5];

    /* Skip id if there are no observers for it */
    int32_t ider_i, ider_count = flecs_event_observers_get(er, id, iders);
    int32_t ider_onset_i, ider_onset_count = 0;
    if (er_onset) {
        ider_onset_count = flecs_event_observers_get(
            er_onset, id, iders_onset);
    }

    if (!may_override && (!ider_count && !ider_onset_count)) {
        return;
    }

    it->ids[0] = id;
    it->sources[0] = tgt;
    it->event_id = id;
    it->ptrs[0] = NULL;
    it->sizes[0] = 0;

    int32_t storage_i = ecs_table_type_to_storage_index(tgt_table, column);
    if (storage_i != -1) {
        ecs_assert(idr->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_t *vec = &tgt_table->data.columns[storage_i];
        ecs_size_t size = idr->type_info->size;
        it->ptrs[0] = ecs_vec_get(vec, size, offset);
        it->sizes[0] = size;
    }

    const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    bool owned = tr != NULL;

    for (ider_i = 0; ider_i < ider_count; ider_i ++) {
        ecs_event_id_record_t *ider = iders[ider_i];
        flecs_observers_invoke(world, &ider->up, it, table, trav);

        /* Owned takes precedence */
        if (!owned) {
            flecs_observers_invoke(world, &ider->self_up, it, table, trav);
        }
    }

    /* Emit OnSet events for newly inherited components */
    if (storage_i != -1) {
        bool override = false;

        /* If component was added together with IsA relationship, still emit
         * OnSet event, as it's a new value for the entity. */
        void *base_ptr = it->ptrs[0];
        void *ptr = flecs_override(it, emit_ids, id, table, idr);
        if (ptr) {
            override = true;
            it->ptrs[0] = ptr;
        }

        if (ider_onset_count) {
            it->event = er_onset->event;

            for (ider_onset_i = 0; ider_onset_i < ider_onset_count; ider_onset_i ++) {
                ecs_event_id_record_t *ider = iders_onset[ider_onset_i];
                flecs_observers_invoke(world, &ider->up, it, table, trav);

                /* Owned takes precedence */
                if (!owned) {
                    flecs_observers_invoke(
                        world, &ider->self_up, it, table, trav);
                } else if (override) {
                    ecs_entity_t src = it->sources[0];
                    it->sources[0] = 0;
                    flecs_observers_invoke(world, &ider->self, it, table, 0);
                    flecs_observers_invoke(world, &ider->self_up, it, table, 0);
                    it->sources[0] = src;
                }
            }

            it->event = event;
            it->ptrs[0] = base_ptr;
        }
    }
}

static
void flecs_emit_forward_and_cache_id(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_id_record_t *idr,
    ecs_entity_t tgt,
    ecs_record_t *tgt_record,
    ecs_table_t *tgt_table,
    const ecs_table_record_t *tgt_tr,
    int32_t column,
    int32_t offset,
    ecs_vec_t *reachable_ids,
    ecs_entity_t trav)
{
    /* Cache forwarded id for (rel, tgt) pair */
    ecs_reachable_elem_t *elem = ecs_vec_append_t(&world->allocator,
        reachable_ids, ecs_reachable_elem_t);
    elem->tr = tgt_tr;
    elem->record = tgt_record;
    elem->src = tgt;
    elem->id = idr->id;
#ifndef NDEBUG
    elem->table = tgt_table;
#endif
    ecs_assert(tgt_table == tgt_record->table, ECS_INTERNAL_ERROR, NULL);

    flecs_emit_forward_id(world, er, er_onset, emit_ids, it, table, idr,
        tgt, tgt_table, column, offset, trav);
}

static
int32_t flecs_emit_stack_at(
    ecs_vec_t *stack,
    ecs_id_record_t *idr)
{
    int32_t sp = 0, stack_count = ecs_vec_count(stack);
    ecs_table_t **stack_elems = ecs_vec_first(stack);

    for (sp = 0; sp < stack_count; sp ++) {
        ecs_table_t *elem = stack_elems[sp];
        if (flecs_id_record_get_table(idr, elem)) {
            break;
        }
    }

    return sp;
}

static
bool flecs_emit_stack_has(
    ecs_vec_t *stack,
    ecs_id_record_t *idr)
{
    return flecs_emit_stack_at(stack, idr) != ecs_vec_count(stack);
}

static
void flecs_emit_forward_cached_ids(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_reachable_cache_t *rc,
    ecs_vec_t *reachable_ids,
    ecs_vec_t *stack,
    ecs_entity_t trav)
{
    ecs_reachable_elem_t *elems = ecs_vec_first_t(&rc->ids, 
        ecs_reachable_elem_t);
    int32_t i, count = ecs_vec_count(&rc->ids);
    for (i = 0; i < count; i ++) {
        ecs_reachable_elem_t *rc_elem = &elems[i];
        const ecs_table_record_t *rc_tr = rc_elem->tr;
        ecs_assert(rc_tr != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_id_record_t *rc_idr = (ecs_id_record_t*)rc_tr->hdr.cache;
        ecs_record_t *rc_record = rc_elem->record;

        ecs_assert(rc_idr->id == rc_elem->id, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(rc_record != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(flecs_entities_get(world, rc_elem->src) == 
            rc_record, ECS_INTERNAL_ERROR, NULL);
        ecs_dbg_assert(rc_record->table == rc_elem->table, 
            ECS_INTERNAL_ERROR, NULL);

        if (flecs_emit_stack_has(stack, rc_idr)) {
            continue;
        }

        int32_t rc_offset = ECS_RECORD_TO_ROW(rc_record->row);
        flecs_emit_forward_and_cache_id(world, er, er_onset, emit_ids,
            it, table, rc_idr, rc_elem->src,
                rc_record, rc_record->table, rc_tr, rc_tr->column,
                    rc_offset, reachable_ids, trav);
    }
}

static
void flecs_emit_dump_cache(
    ecs_world_t *world,
    const ecs_vec_t *vec)
{
    ecs_reachable_elem_t *elems = ecs_vec_first_t(vec, ecs_reachable_elem_t);
    for (int i = 0; i < ecs_vec_count(vec); i ++) {
        ecs_reachable_elem_t *elem = &elems[i];
        char *idstr = ecs_id_str(world, elem->id);
        char *estr = ecs_id_str(world, elem->src);
        ecs_dbg_3("- id: %s (%u), src: %s (%u), table: %p", 
            idstr, (uint32_t)elem->id,
            estr, (uint32_t)elem->src,
            elem->table);
        ecs_os_free(idstr);
        ecs_os_free(estr);
    }
    if (!ecs_vec_count(vec)) {
        ecs_dbg_3("- no entries");
    }
}

static
void flecs_emit_forward_table_up(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t tgt,
    ecs_table_t *tgt_table,
    ecs_record_t *tgt_record,
    ecs_id_record_t *tgt_idr,
    ecs_vec_t *stack,
    ecs_vec_t *reachable_ids)
{
    ecs_allocator_t *a = &world->allocator;
    int32_t i, id_count = tgt_table->type.count;
    ecs_id_t *ids = tgt_table->type.array;
    int32_t offset = ECS_RECORD_TO_ROW(tgt_record->row);
    int32_t rc_child_offset = ecs_vec_count(reachable_ids);
    int32_t stack_count = ecs_vec_count(stack);

    /* If tgt_idr is out of sync but is not the current id record being updated,
     * keep track so that we can update two records for the cost of one. */
    ecs_reachable_cache_t *rc = &tgt_idr->reachable;
    bool parent_revalidate = (reachable_ids != &rc->ids) && 
        (rc->current != rc->generation);
    if (parent_revalidate) {
        ecs_vec_reset_t(a, &rc->ids, ecs_reachable_elem_t);
    }

    if (ecs_should_log_3()) {
        char *idstr = ecs_id_str(world, tgt_idr->id);
        ecs_dbg_3("forward events from %s", idstr);
        ecs_os_free(idstr);
    }
    ecs_log_push_3();

    /* Function may have to copy values from overridden components if an IsA
     * relationship was added together with other components. */
    ecs_entity_t trav = ECS_PAIR_FIRST(tgt_idr->id);
    bool inherit = trav == EcsIsA;

    for (i = 0; i < id_count; i ++) {
        ecs_id_t id = ids[i];
        ecs_table_record_t *tgt_tr = &tgt_table->records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tgt_tr->hdr.cache;
        if (inherit && (idr->flags & EcsIdDontInherit)) {
            continue;
        }

        /* Id has the same relationship, traverse to find ids for forwarding */
        if (ECS_PAIR_FIRST(id) == trav) {
            ecs_table_t **t = ecs_vec_append_t(&world->allocator, stack, 
                ecs_table_t*);
            t[0] = tgt_table;

            ecs_reachable_cache_t *idr_rc = &idr->reachable;
            if (idr_rc->current == idr_rc->generation) {
                /* Cache hit, use cached ids to prevent traversing the same
                 * hierarchy multiple times. This especially speeds up code 
                 * where (deep) hierarchies are created. */
                if (ecs_should_log_3()) {
                    char *idstr = ecs_id_str(world, id);
                    ecs_dbg_3("forward cached for %s", idstr);
                    ecs_os_free(idstr);
                }
                ecs_log_push_3();
                flecs_emit_forward_cached_ids(world, er, er_onset, emit_ids, it,
                    table, idr_rc, reachable_ids, stack, trav);
                ecs_log_pop_3();
            } else {
                /* Cache is dirty, traverse upwards */
                do {
                    flecs_emit_forward_up(world, er, er_onset, emit_ids, it, 
                        table, idr, stack, reachable_ids);
                    if (++i >= id_count) {
                        break;
                    }

                    id = ids[i];
                    if (ECS_PAIR_FIRST(id) != trav) {
                        break;
                    }
                } while (true);
            }

            ecs_vec_remove_last(stack);
            continue;
        }

        int32_t stack_at = flecs_emit_stack_at(stack, idr);
        if (parent_revalidate && (stack_at == (stack_count - 1))) {
            /* If parent id record needs to be revalidated, add id */
            ecs_reachable_elem_t *elem = ecs_vec_append_t(a, &rc->ids, 
                ecs_reachable_elem_t);
            elem->tr = tgt_tr;
            elem->record = tgt_record;
            elem->src = tgt;
            elem->id = idr->id;
#ifndef NDEBUG
            elem->table = tgt_table;
#endif
        }

        /* Skip id if it's masked by a lower table in the tree */
        if (stack_at != stack_count) {
            continue;
        }

        flecs_emit_forward_and_cache_id(world, er, er_onset, emit_ids, it,
            table, idr, tgt, tgt_record, tgt_table, tgt_tr, i, 
                offset, reachable_ids, trav);
    }

    if (parent_revalidate) {
        /* If this is not the current cache being updated, but it's marked
         * as out of date, use intermediate results to populate cache. */
        int32_t rc_parent_offset = ecs_vec_count(&rc->ids);

        /* Only add ids that were added for this table */
        int32_t count = ecs_vec_count(reachable_ids);
        count -= rc_child_offset;

        /* Append ids to any ids that already were added /*/
        if (count) {
            ecs_vec_grow_t(a, &rc->ids, ecs_reachable_elem_t, count);
            ecs_reachable_elem_t *dst = ecs_vec_get_t(&rc->ids, 
                ecs_reachable_elem_t, rc_parent_offset);
            ecs_reachable_elem_t *src = ecs_vec_get_t(reachable_ids,
                ecs_reachable_elem_t, rc_child_offset);
            ecs_os_memcpy_n(dst, src, ecs_reachable_elem_t, count);
        }

        if (it->event == EcsOnAdd || it->event == EcsOnRemove) {
            /* Only OnAdd/OnRemove events can validate a cache */
            rc->current = rc->generation;
        }

        if (ecs_should_log_3()) {
            char *idstr = ecs_id_str(world, tgt_idr->id);
            ecs_dbg_3("cache revalidated for %s:", idstr);
            ecs_os_free(idstr);
            flecs_emit_dump_cache(world, &rc->ids);
        }
    }

    ecs_log_pop_3();
}

static
void flecs_emit_forward_up(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_id_record_t *idr,
    ecs_vec_t *stack,
    ecs_vec_t *reachable_ids)
{
    ecs_id_t id = idr->id;
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    tgt = flecs_entities_get_current(world, tgt);
    ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t *tgt_record = flecs_entities_try(world, tgt);
    ecs_table_t *tgt_table;
    if (!tgt_record || !(tgt_table = tgt_record->table)) {
        return;
    }

    flecs_emit_forward_table_up(world, er, er_onset, emit_ids, it, table, 
        tgt, tgt_table, tgt_record, idr, stack, reachable_ids);
}

static
void flecs_emit_forward(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_id_record_t *idr)
{
    ecs_reachable_cache_t *rc = &idr->reachable;

    if (rc->current != rc->generation) {
        /* Cache miss, iterate the tree to find ids to forward */
        if (ecs_should_log_3()) {
            char *idstr = ecs_id_str(world, idr->id);
            ecs_dbg_3("reachable cache miss for %s", idstr);
            ecs_os_free(idstr);
        }
        ecs_log_push_3();

        ecs_vec_t stack;
        ecs_vec_init_t(&world->allocator, &stack, ecs_table_t*, 0);
        ecs_vec_reset_t(&world->allocator, &rc->ids, ecs_reachable_elem_t);
        flecs_emit_forward_up(world, er, er_onset, emit_ids, it, table, 
            idr, &stack, &rc->ids);
        it->sources[0] = 0;
        ecs_vec_fini_t(&world->allocator, &stack, ecs_table_t*);

        if (it->event == EcsOnAdd || it->event == EcsOnRemove) {
            /* Only OnAdd/OnRemove events can validate a cache */
            rc->current = rc->generation;
        }

        if (ecs_should_log_3()) {
            ecs_dbg_3("cache after rebuild:");
            flecs_emit_dump_cache(world, &rc->ids);
        }

        ecs_log_pop_3();
    } else {
        /* Cache hit, use cached values instead of walking the tree */
        if (ecs_should_log_3()) {
            char *idstr = ecs_id_str(world, idr->id);
            ecs_dbg_3("reachable cache hit for %s", idstr);
            ecs_os_free(idstr);
            flecs_emit_dump_cache(world, &rc->ids);
        }

        ecs_entity_t trav = ECS_PAIR_FIRST(idr->id);
        ecs_reachable_elem_t *elems = ecs_vec_first_t(&rc->ids, 
            ecs_reachable_elem_t);
        int32_t i, count = ecs_vec_count(&rc->ids);
        for (i = 0; i < count; i ++) {
            ecs_reachable_elem_t *elem = &elems[i];
            const ecs_table_record_t *tr = elem->tr;
            ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_id_record_t *rc_idr = (ecs_id_record_t*)tr->hdr.cache;
            ecs_record_t *r = elem->record;

            ecs_assert(rc_idr->id == elem->id, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(flecs_entities_get(world, elem->src) == r,
                ECS_INTERNAL_ERROR, NULL);
            ecs_dbg_assert(r->table == elem->table, ECS_INTERNAL_ERROR, NULL);

            int32_t offset = ECS_RECORD_TO_ROW(r->row);
            flecs_emit_forward_id(world, er, er_onset, emit_ids, it, table,
                rc_idr, elem->src, r->table, tr->column, offset, trav);
        }
    }
}

/* The emit function is responsible for finding and invoking the observers 
 * matching the emitted event. The function is also capable of forwarding events
 * for newly reachable ids (after adding a relationship) and propagating events
 * downwards. Both capabilities are not just useful in application logic, but
 * are also an important building block for keeping query caches in sync. */
void flecs_emit(
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_event_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->event != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->event != EcsWildcard, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->ids != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->ids->count != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->observable != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_time_t t = {0};
    bool measure_time = world->flags & EcsWorldMeasureSystemTime;
    if (measure_time) {
        ecs_time_measure(&t);
    }

    const ecs_type_t *ids = desc->ids;
    ecs_entity_t event = desc->event;
    ecs_table_t *table = desc->table;
    int32_t offset = desc->offset;
    int32_t i, r, count = desc->count;
    ecs_flags32_t table_flags = table->flags;

    /* Table events are emitted for internal table operations only, and do not
     * provide component data and/or entity ids. */
    bool table_event = desc->flags & EcsEventTableOnly;
    if (!count && !table_event) {
        /* If no count is provided, forward event for all entities in table */
        count = ecs_table_count(table) - offset;
    }

    /* When the NoOnSet flag is provided, no OnSet/UnSet events should be 
     * generated when new components are inherited. */
    bool no_on_set = desc->flags & EcsEventNoOnSet;

    ecs_id_t ids_cache = 0;
    void *ptrs_cache = NULL;
    ecs_size_t sizes_cache = 0;
    int32_t columns_cache = 0;
    ecs_entity_t sources_cache = 0;

    ecs_iter_t it = {
        .world = stage,
        .real_world = world,
        .event = event,
        .table = table,
        .field_count = 1,
        .ids = &ids_cache,
        .ptrs = &ptrs_cache,
        .sizes = &sizes_cache,
        .columns = &columns_cache,
        .sources = &sources_cache,
        .other_table = desc->other_table,
        .offset = offset,
        .count = count,
        .param = (void*)desc->param,
        .flags = desc->flags | EcsIterIsValid
    };

    /* The world event id is used to determine if an observer has already been
     * triggered for an event. Observers for multiple components are split up
     * into multiple observers for a single component, and this counter is used
     * to make sure a multi observer only triggers once, even if multiple of its
     * single-component observers trigger. */
    world->event_id ++;

    ecs_observable_t *observable = ecs_get_observable(desc->observable);
    ecs_check(observable != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Event records contain all observers for a specific event. In addition to
     * the emitted event, also request data for the Wildcard event (for 
     * observers subscribing to the wildcard event), OnSet and UnSet events. The
     * latter to are used for automatically emitting OnSet/UnSet events for 
     * inherited components, for example when an IsA relationship is added to an
     * entity. This doesn't add much overhead, as fetching records is cheap for
     * builtin event types. */
    ecs_event_record_t *er = flecs_event_record_get_if(observable, event);
    ecs_event_record_t *wcer = flecs_event_record_get_if(observable, EcsWildcard);
    ecs_event_record_t *er_onset = flecs_event_record_get_if(observable, EcsOnSet);
    ecs_event_record_t *er_unset = flecs_event_record_get_if(observable, EcsUnSet);

    ecs_data_t *storage = NULL;
    ecs_vec_t *columns = NULL;
    if (count) {
        storage = &table->data;
        columns = storage->columns;
        it.entities = ecs_vec_get_t(&storage->entities, ecs_entity_t, offset);
    }

    int32_t id_count = ids->count;
    ecs_id_t *id_array = ids->array;

    /* If a table has IsA relationships, OnAdd/OnRemove events can trigger 
     * (un)overriding a component. When a component is overridden its value is
     * initialized with the value of the overridden component. */
    bool can_override = count && (table_flags & EcsTableHasIsA) && (
        (event == EcsOnAdd) || (event == EcsOnRemove));

    /* When a new (acyclic) relationship is added (emitting an OnAdd/OnRemove
     * event) this will cause the components of the target entity to be 
     * propagated to the source entity. This makes it possible for observers to
     * get notified of any new reachable components though the relationship. */
    bool can_forward = event != EcsOnSet;

    /* Set if event has been propagated */
    bool propagated = false;

    /* Does table has observed entities */
    bool has_observed = table_flags & EcsTableHasObserved;

    /* When a relationship is removed, the events reachable through that 
     * relationship should emit UnSet events. This is part of the behavior that
     * allows observers to be agnostic of whether a component is inherited. */
    bool can_unset = count && (event == EcsOnRemove) && !no_on_set;

    ecs_event_id_record_t *iders[5] = {0};
    int32_t unset_count = 0;

repeat_event:
    /* This is the core event logic, which is executed for each event. By 
     * default this is just the event kind from the ecs_event_desc_t struct, but
     * can also include the Wildcard and UnSet events. The latter is emitted as
     * counterpart to OnSet, for any removed ids associated with data. */
    for (i = 0; i < id_count; i ++) {
        /* Emit event for each id passed to the function. In most cases this 
         * will just be one id, like a component that was added, removed or set.
         * In some cases events are emitted for multiple ids.
         * 
         * One example is when an id was added with a "With" property, or 
         * inheriting from a prefab with overrides. In these cases an entity is 
         * moved directly to the archetype with the additional components. */
        ecs_id_record_t *idr = NULL;
        const ecs_type_info_t *ti = NULL;
        ecs_id_t id = id_array[i];
        int32_t ider_i, ider_count = 0;
        bool is_pair = ECS_IS_PAIR(id);
        void *override_ptr = NULL;
        ecs_entity_t base = 0;

        /* Check if this id is a pair of an acyclic relationship. If so, we may
         * have to forward ids from the pair's target. */
        if ((can_forward && is_pair) || can_override) {
            idr = flecs_query_id_record_get(world, id);
            ecs_flags32_t idr_flags = idr->flags;

            if (is_pair && (idr_flags & EcsIdAcyclic)) {
                ecs_event_record_t *er_fwd = NULL;
                if (ECS_PAIR_FIRST(id) == EcsIsA) {
                    if (event == EcsOnAdd) {
                        if (!world->stages[0].base) {
                            /* Adding an IsA relationship can trigger prefab
                             * instantiation, which can instantiate prefab 
                             * hierarchies for the entity to which the 
                             * relationship was added. */
                            ecs_entity_t tgt = ECS_PAIR_SECOND(id);

                            /* Setting this value prevents flecs_instantiate 
                             * from being called recursively, in case prefab
                             * children also have IsA relationships. */
                            world->stages[0].base = tgt;
                            flecs_instantiate(world, tgt, table, offset, count);
                            world->stages[0].base = 0;
                        }

                        /* Adding an IsA relationship will emit OnSet events for
                         * any new reachable components. */
                        er_fwd = er_onset;
                    } else if (event == EcsOnRemove) {
                        /* Vice versa for removing an IsA relationship. */
                        er_fwd = er_unset;
                    }
                }

                /* Forward events for components from pair target */
                flecs_emit_forward(world, er, er_fwd, ids, &it, table, idr);
            }

            if (can_override && (!(idr_flags & EcsIdDontInherit))) {
                /* Initialize overridden components with value from base */
                ti = idr->type_info;
                if (ti) {
                    ecs_table_record_t *base_tr = NULL;
                    int32_t base_column = ecs_search_relation(world, table, 
                        0, id, EcsIsA, EcsUp, &base, NULL, &base_tr);
                    if (base_column != -1) {
                        /* Base found with component */
                        ecs_table_t *base_table = base_tr->hdr.table;
                        base_column = ecs_table_type_to_storage_index(
                            base_table, base_tr->column);
                        ecs_assert(base_column != -1, ECS_INTERNAL_ERROR, NULL);
                        ecs_record_t *base_r = flecs_entities_get(world, base);
                        ecs_assert(base_r != NULL, ECS_INTERNAL_ERROR, NULL);
                        int32_t base_row = ECS_RECORD_TO_ROW(base_r->row);
                        ecs_vec_t *base_v = &base_table->data.columns[base_column];
                        override_ptr = ecs_vec_get(base_v, ti->size, base_row);
                    }
                }
            }
        }

        if (er) {
            /* Get observer sets for id. There can be multiple sets of matching
             * observers, in case an observer matches for wildcard ids. For
             * example, both observers for (ChildOf, p) and (ChildOf, *) would
             * match an event for (ChildOf, p). */
            ider_count = flecs_event_observers_get(er, id, iders);
            idr = idr ? idr : flecs_query_id_record_get(world, id);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
        }

        if (can_unset) {
            /* Increase UnSet count in case this is a component (has data). This
             * will cause the event loop to be ran again as UnSet event. */
            idr = idr ? idr : flecs_query_id_record_get(world, id);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            unset_count += (idr->type_info != NULL);
        }

        if (!ider_count && !override_ptr) {
            /* If nothing more to do for this id, early out */
            continue;
        }

        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
        const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
        if (tr == NULL) {
            /* When a single batch contains multiple add's for an exclusive
             * relationship, it's possible that an id was in the added list
             * that is no longer available for the entity. */
            continue;
        }

        int32_t column = tr->column, storage_i = -1;
        it.columns[0] = column + 1;
        it.ptrs[0] = NULL;
        it.sizes[0] = 0;
        it.event_id = id;
        it.ids[0] = id;

        if (count) {
            storage_i = ecs_table_type_to_storage_index(table, column);
            if (storage_i != -1) {
                /* If this is a component, fetch pointer & size */
                ecs_assert(idr->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_vec_t *vec = &columns[storage_i];
                ecs_size_t size = idr->type_info->size;
                void *ptr = ecs_vec_get(vec, size, offset);
                it.sizes[0] = size;

                if (override_ptr) {
                    if (event == EcsOnAdd) {
                        /* If this is a new override, initialize the component
                         * with the value of the overridden component. */
                        flecs_override_copy(ti, ptr, override_ptr, count);
                    } else if (er_onset) {
                        /* If an override was removed, this re-exposes the
                         * overridden component. Because this causes the actual
                         * (now inherited) value of the component to change, an
                         * OnSet event must be emitted for the base component.*/
                        ecs_assert(event == EcsOnRemove, ECS_INTERNAL_ERROR, NULL);
                        ecs_event_id_record_t *iders_set[5] = {0};
                        int32_t ider_set_i, ider_set_count = 
                            flecs_event_observers_get(er_onset, id, iders_set);
                        if (ider_set_count) {
                            /* Set the source temporarily to the base and base
                             * component pointer. */
                            it.sources[0] = base;
                            it.ptrs[0] = ptr;
                            for (ider_set_i = 0; ider_set_i < ider_set_count; ider_set_i ++) {
                                ecs_event_id_record_t *ider = iders_set[ider_set_i];
                                flecs_observers_invoke(world, &ider->self_up, &it, table, EcsIsA);
                                flecs_observers_invoke(world, &ider->up, &it, table, EcsIsA);
                            }
                            it.sources[0] = 0;
                        }
                    }
                }

                it.ptrs[0] = ptr;
            } else {
                if (it.event == EcsUnSet) {
                    /* Only valid for components, not tags */
                    continue;
                }
            }
        }

        /* Actually invoke observers for this event/id */
        for (ider_i = 0; ider_i < ider_count; ider_i ++) {
            ecs_event_id_record_t *ider = iders[ider_i];
            flecs_observers_invoke(world, &ider->self, &it, table, 0);
            flecs_observers_invoke(world, &ider->self_up, &it, table, 0);
        }

        if (!ider_count || !count || !has_observed) {
            continue;
        }

        /* If event is propagated, we don't have to manually invalidate entities
         * lower in the tree(s). */
        propagated = true;

        /* The table->observed_count value indicates if the table contains any
         * entities that are used as targets of acyclic relationships. If the
         * entity/entities for which the event was generated is used as such a
         * target, events must be propagated downwards. */
        ecs_entity_t *entities = it.entities;
        it.entities = NULL;

        ecs_record_t **recs = ecs_vec_get_t(&storage->records, 
            ecs_record_t*, offset);
        for (r = 0; r < count; r ++) {
            ecs_record_t *record = recs[r];
            if (!record) {
                /* If the event is emitted after a bulk operation, it's possible
                 * that it hasn't been populated with entities yet. */
                continue;
            }

            ecs_id_record_t *idr_t = record->idr;
            if (idr_t) {
                /* Entity is used as target in acyclic pairs, propagate */
                ecs_entity_t e = entities[r];
                it.sources[0] = e;

                flecs_emit_propagate(world, &it, idr, idr_t, iders, ider_count);
            }
        }

        it.table = table;
        it.entities = entities;
        it.count = count;
        it.sources[0] = 0;
    }

    if (count && can_forward && has_observed && !propagated) {
        flecs_emit_propagate_invalidate(world, table, offset, count);
    }

    can_override = false; /* Don't override twice */
    can_unset = false; /* Don't unset twice */
    can_forward = false; /* Don't forward twice */

    if (unset_count && er_unset && (er != er_unset)) {
        /* Repeat event loop for UnSet event */
        unset_count = 0;
        er = er_unset;
        it.event = EcsUnSet;
        goto repeat_event;
    }

    if (wcer && er != wcer) {
        /* Repeat event loop for Wildcard event */
        er = wcer;
        it.event = event;
        goto repeat_event;
    }

error:
    if (measure_time) {
        world->info.emit_time_total += (ecs_ftime_t)ecs_time_measure(&t);
    }
    return;
}

void ecs_emit(
    ecs_world_t *stage,
    ecs_event_desc_t *desc)
{
    ecs_world_t *world = (ecs_world_t*)ecs_get_world(stage);
    if (desc->entity) {
        ecs_assert(desc->table == NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(desc->offset == 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(desc->count == 0, ECS_INVALID_PARAMETER, NULL);
        ecs_record_t *r = flecs_entities_get(world, desc->entity);
        ecs_table_t *table;
        if (!r || !(table = r->table)) {
            /* Empty entities can't trigger observers */
            return;
        }
        desc->table = table;
        desc->offset = ECS_RECORD_TO_ROW(r->row);
        desc->count = 1;
    }
    if (!desc->observable) {
        desc->observable = world;
    }
    flecs_emit(world, stage, desc);
}
