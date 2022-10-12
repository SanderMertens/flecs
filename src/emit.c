#include "private_api.h"

#if 1

bool flecs_default_observer_next_callback(ecs_iter_t *it);
void flecs_default_uni_observer_run_callback(ecs_iter_t *it);

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
    return flecs_sparse_get(o->events, ecs_event_record_t, event);
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
    return flecs_sparse_ensure(o->events, ecs_event_record_t, event);
}

ecs_event_record_t* flecs_event_record_get_if(
    const ecs_observable_t *o,
    ecs_entity_t event)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_event_record_t *er = flecs_event_record_get(o, event);
    if (er) {
        if (!ecs_map_is_initialized(&er->event_ids)) {
            return NULL;
        }
    }

    return er;
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
    return ecs_map_get_ptr(&er->event_ids, ecs_event_id_record_t*, id);
}

ecs_event_id_record_t* flecs_event_id_record_get_if(
    const ecs_event_record_t *er,
    ecs_id_t id)
{
    ecs_event_id_record_t *ider = flecs_event_id_record_get(er, id);
    if (!ider) {
        return NULL;
    }

    if (ecs_map_is_initialized(&ider->self)) {
        ecs_assert(ecs_map_count(&ider->self) > 0, 
            ECS_INTERNAL_ERROR, NULL);
        return ider;
    }
    if (ecs_map_is_initialized(&ider->up)) {
        ecs_assert(ecs_map_count(&ider->up) > 0, 
            ECS_INTERNAL_ERROR, NULL);
        return ider;
    }
    if (ecs_map_is_initialized(&ider->self_up)) {
        ecs_assert(ecs_map_count(&ider->self_up) > 0, 
            ECS_INTERNAL_ERROR, NULL);
        return ider;
    }

    return NULL;
}

ecs_event_id_record_t* flecs_event_id_record_ensure(
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

    ecs_event_id_record_t **idt = ecs_map_ensure(
        &er->event_ids, ecs_event_id_record_t*, id);
    if (!idt[0]) {
        idt[0] = ider;
    }

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
        if (ecs_map_remove(&er->event_ids, id) == 0) {
            ecs_map_fini(&er->event_ids);
        }
    }
}

int32_t flecs_event_observers_get(
    const ecs_event_record_t *er,
    ecs_id_t id,
    ecs_event_id_record_t **iders)
{
    if (!er) {
        return 0;
    }

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

static
bool flecs_ignore_observer(
    ecs_world_t *world,
    ecs_observer_t *observer,
    ecs_table_t *table)
{
    int32_t *last_event_id = observer->last_event_id;
    if (last_event_id && last_event_id[0] == world->event_id) {
        return true;
    }

    if (!table) {
        return false;
    }
    
    ecs_filter_t *filter = &observer->filter;
    if (!(filter->flags & EcsFilterMatchPrefab) && 
        (table->flags & EcsTableIsPrefab)) 
    {
        return true;
    }
    if (!(filter->flags & EcsFilterMatchDisabled) && 
        (table->flags & EcsTableIsDisabled)) 
    {
        return true;
    }
    
    return false;
}

static
ecs_entity_t flecs_get_actual_event(
    ecs_term_t *term,
    ecs_entity_t event)
{
    /* If operator is Not, reverse the event */
    if (term->oper == EcsNot) {
        if (event == EcsOnAdd) {
            event = EcsOnRemove;
        } else if (event == EcsOnRemove) {
            event = EcsOnAdd;
        }
    }

    return event;
}

static
void flecs_observer_invoke(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_observer_t *observer,
    ecs_iter_action_t callback,
    ecs_table_t *table) 
{
    ecs_assert(it->callback != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_table_lock(it->world, table);
    if (ecs_should_log_3()) {
        char *path = ecs_get_fullpath(world, it->system);
        ecs_dbg_3("observer %s", path);
        ecs_os_free(path);
        ecs_log_push_3();
    }
    world->info.observers_ran_frame ++;

    ecs_filter_t *filter = &observer->filter;
    ecs_term_t *term = &filter->terms[0];
    ecs_entity_t observer_src = term->src.id;
    if (observer_src && !(term->src.flags & EcsIsEntity)) {
        observer_src = 0;
    }

    int32_t i, count = it->count;
    ecs_size_t size = it->sizes[0];
    ecs_entity_t src = it->sources[0];
    bool instanced = filter->flags & EcsFilterIsInstanced;

    if (!observer_src && ((count == 1) || (size == 0) || (src == 0) || instanced)) {
        ECS_BIT_COND(it->flags, EcsIterIsInstanced, instanced);
        callback(it);
    } else {
        ECS_BIT_CLEAR(it->flags, EcsIterIsInstanced);
        ecs_entity_t *entities = it->entities;
        it->count = 1;
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = entities[i];
            it->entities = &e;
            if (!observer_src) {
                callback(it);
            } else if (observer_src == e) {
                ecs_entity_t dummy = 0;
                it->entities = &dummy;
                if (!src) {
                    it->sources[0] = e;
                }
                callback(it);
                it->sources[0] = src;
                break;
            }
        }
        it->entities = entities;
        it->count = count;
    }

    ecs_table_unlock(it->world, table);
    ecs_log_pop_3();
}

static
void flecs_uni_observer_invoke(
    ecs_world_t *world,
    ecs_observer_t *observer,
    ecs_iter_t *it,
    ecs_table_t *table)
{
    ecs_filter_t *filter = &observer->filter;
    ecs_term_t *term = &filter->terms[0];
    if (flecs_ignore_observer(world, observer, table)) {
        return;
    }

    bool is_filter = term->inout == EcsInOutNone;
    ECS_BIT_COND(it->flags, EcsIterIsFilter, is_filter);
    it->system = observer->entity;
    it->ctx = observer->ctx;
    it->binding_ctx = observer->binding_ctx;
    it->term_index = observer->term_index;
    it->terms = term;

    ecs_entity_t event = it->event;
    it->event = flecs_get_actual_event(term, event);
    void *ptrs = it->ptrs;
    if (is_filter) {
        it->ptrs = NULL;
    }

    if (observer->run) {
        it->next = flecs_default_observer_next_callback;
        it->callback = flecs_default_uni_observer_run_callback;
        it->ctx = observer;
        observer->run(it);
    } else {
        ecs_iter_action_t callback = observer->callback;
        it->callback = callback;
        flecs_observer_invoke(world, it, observer, callback, table);
    }

    it->event = event;
    it->ptrs = ptrs;
}

static
void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table)
{
    if (ecs_map_is_initialized(observers)) {
        ecs_map_iter_t oit = ecs_map_iter(observers);
        ecs_observer_t *o;
        while ((o = ecs_map_next_ptr(&oit, ecs_observer_t*, NULL))) {
            flecs_uni_observer_invoke(world, o, it, table);
        }
    }
}

static
void flecs_emit_propagate(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_id_record_t *idr,
    ecs_entity_t entity,
    ecs_event_id_record_t **iders,
    int32_t ider_count)
{
    ecs_id_t pair = ecs_pair(EcsWildcard, entity);
    ecs_id_record_t *tgt_idr = flecs_id_record_get(world, pair);
    if (!tgt_idr) {
        return;
    }

    /* Iterate acyclic relationships */
    ecs_id_record_t *cur = tgt_idr;
    while ((cur = cur->acyclic.next)) {
        flecs_process_pending_tables(world);

        ecs_table_cache_iter_t idt;
        if (!flecs_table_cache_iter(&cur->cache, &idt)) {
            continue;
        }

        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&idt, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
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
                flecs_observers_invoke(world, &ider->up, it, table);

                if (!owned) {
                    /* Owned takes precedence */
                    flecs_observers_invoke(world, &ider->self_up, it, table);
                }
            }

            if (!table->observed_count) {
                continue;
            }

            ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
            ecs_record_t **records = ecs_vec_first(&table->data.records);

            for (e = 0; e < entity_count; e ++) {
                uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(records[e]->row);
                if (flags & EcsEntityObservedAcyclic) {
                    /* Only notify for entities that are used in pairs with
                     * acyclic relationships */
                    flecs_emit_propagate(world, it, idr, entities[e], 
                        iders, ider_count);
                }
            }
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
    ecs_id_record_t *idr,
    int32_t offset)
{
    if (it->event != EcsOnAdd || (it->flags & EcsEventNoOnSet)) {
        return NULL;
    }

    int32_t i = 0, count = emit_ids->count;
    ecs_id_t *ids = emit_ids->array;
    for (i = 0; i < count; i ++) {
        if (ids[i] == id) {
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
            void *ptr = ecs_vec_get(vec, size, offset);
            flecs_override_copy(ti, ptr, it->ptrs[0], it->count);
            return ptr;
        }
    }

    return NULL;
}

static
void flecs_emit_forward_pair_up(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t event,
    ecs_id_t id,
    ecs_vec_t *stack);

static
void flecs_emit_forward_table_up(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t event,
    ecs_entity_t rel,
    ecs_entity_t tgt,
    ecs_table_t *tgt_table,
    int32_t offset,
    ecs_vec_t *stack)
{
    int32_t sp, stack_count = ecs_vec_count(stack);
    ecs_table_t **stack_elems = ecs_vec_first(stack);
    int32_t i, id_count = tgt_table->type.count;
    ecs_id_t *ids = tgt_table->type.array;
    ecs_event_id_record_t *iders[5];
    ecs_event_id_record_t *iders_onset[5];

    /* Function may have to copy values from overridden components if an IsA
     * relationship was added together with other components. */
    bool inherit = rel == EcsIsA;
    bool may_override = inherit && (event == EcsOnAdd) && (emit_ids->count > 1);

    for (i = 0; i < id_count; i ++) {
        ecs_id_t id = ids[i];
        ecs_table_record_t *tgt_tr = &tgt_table->records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tgt_tr->hdr.cache;

        if (inherit && (idr->flags & EcsIdDontInherit)) {
            continue;
        }

        /* Id has the same relationship, traverse to find ids for forwarding */
        if (ECS_PAIR_FIRST(id) == rel) {
            ecs_table_t **t = ecs_vec_append_t(&world->allocator, stack, 
                ecs_table_t*);
            t[0] = tgt_table;

            do {
                flecs_emit_forward_pair_up(world, er, er_onset, emit_ids, it, 
                    table, event, id, stack);
                id = ids[++ i];
                if (ECS_PAIR_FIRST(id) != rel) {
                    break;
                }
            } while (true);

            ecs_vec_remove_last(stack);
            continue;
        }

        if (idr->flags & EcsDontInherit) {
            continue;
        }

        /* Skip id if there are no observers for it */
        int32_t ider_i, ider_count = flecs_event_observers_get(er, id, iders);
        int32_t ider_onset_i, ider_onset_count = 0;
        if (er_onset) {
            ider_onset_count = flecs_event_observers_get(
                er_onset, id, iders_onset);
        }

        if (!may_override && (!ider_count && !ider_onset_count)) {
            continue;
        }

        /* Skip id if it's masked by a lower table in the tree */
        for (sp = 0; sp < stack_count; sp ++) {
            ecs_table_t *elem = stack_elems[sp];
            if (flecs_id_record_get_table(idr, elem)) {
                break;
            }
        }
        if (sp != stack_count) {
            continue;
        }

        it->ids[0] = id;
        it->sources[0] = tgt;
        it->event_id = id;
        it->ptrs[0] = NULL;
        it->sizes[0] = 0;

        int32_t storage_i = ecs_table_type_to_storage_index(tgt_table, i);
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
            flecs_observers_invoke(world, &ider->up, it, table);

            /* Owned takes precedence */
            if (!owned) {
                flecs_observers_invoke(world, &ider->self_up, it, table);
            }
        }

        /* Emit OnSet events for newly inherited components */
        if (storage_i != -1) {
            bool override = false;

            /* If component was added together with IsA relationship, still emit
             * OnSet event, as it's a new value for the entity. */
            void *base_ptr = it->ptrs[0];
            void *ptr = flecs_override(it, emit_ids, id, table, idr, offset);
            if (ptr) {
                override = true;
                it->ptrs[0] = ptr;
            }

            if (event == EcsOnAdd) {
                it->event = EcsOnSet;
            } else if (event == EcsOnRemove) {
                it->event = EcsUnSet;
            }

            for (ider_onset_i = 0; ider_onset_i < ider_onset_count; ider_onset_i ++) {
                ecs_event_id_record_t *ider = iders_onset[ider_onset_i];
                flecs_observers_invoke(world, &ider->up, it, table);

                /* Owned takes precedence */
                if (!owned) {
                    flecs_observers_invoke(world, &ider->self_up, it, table);
                } else if (override) {
                    ecs_entity_t src = it->sources[0];
                    it->sources[0] = 0;
                    flecs_observers_invoke(world, &ider->self, it, table);
                    flecs_observers_invoke(world, &ider->self_up, it, table);
                    it->sources[0] = src;
                }
            }

            it->event = event;
            it->ptrs[0] = base_ptr;
        }
    }
}

static
void flecs_emit_forward_pair_up(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t event,
    ecs_id_t id,
    ecs_vec_t *stack)
{
    ecs_entity_t rel = ECS_PAIR_FIRST(id);
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    tgt = flecs_entities_get_current(world, tgt);
    ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t *r = flecs_entities_get(world, tgt);
    ecs_table_t *tgt_table;
    if (!r || !(tgt_table = r->table)) {
        return;
    }

    flecs_emit_forward_table_up(world, er, er_onset, emit_ids, it, table, event, 
        rel, tgt, tgt_table, ECS_RECORD_TO_ROW(r->row), stack);
}

static
void flecs_emit_forward(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_event_record_t *er_onset,
    const ecs_type_t *emit_ids,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t event,
    ecs_id_t id)
{
    ecs_vec_t stack;
    ecs_vec_init_t(&world->allocator, &stack, ecs_table_t*, 0);
    flecs_emit_forward_pair_up(world, er, er_onset, emit_ids, it, table, 
        event, id, &stack);
    it->sources[0] = 0;
    ecs_vec_fini_t(&world->allocator, &stack, ecs_table_t*);
}

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
    bool table_event = desc->flags & EcsEventTableOnly;
    bool no_on_set = desc->flags & EcsEventNoOnSet;
    if (!count && !table_event) {
        count = ecs_table_count(table) - offset;
    }

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

    world->event_id ++;

    ecs_observable_t *observable = ecs_get_observable(desc->observable);
    ecs_check(observable != NULL, ECS_INVALID_PARAMETER, NULL);

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
    bool can_override = count && (table->flags & EcsTableHasIsA) && (
        (event == EcsOnAdd) || (event == EcsOnRemove));
    bool can_unset = count && (event == EcsOnRemove) && !no_on_set;
    bool can_forward = event != EcsOnSet;

    ecs_event_id_record_t *iders[5] = {0};
    int32_t unset_count = 0;

repeat_event:
    for (i = 0; i < id_count; i ++) {
        ecs_id_record_t *idr = NULL;
        const ecs_type_info_t *ti = NULL;
        ecs_id_t id = id_array[i];
        int32_t ider_i, ider_count = 0;
        bool is_pair = ECS_IS_PAIR(id);
        void *override_ptr = NULL;
        ecs_entity_t base = 0;

        if ((can_forward && is_pair) || can_override) {
            idr = flecs_query_id_record_get(world, id);
            ecs_flags32_t idr_flags = idr->flags;

            if (is_pair && (idr_flags & EcsIdAcyclic)) {
                ecs_event_record_t *er_fwd = NULL;
                if (ECS_PAIR_FIRST(id) == EcsIsA) {
                    if (event == EcsOnAdd) {
                        if (!world->stages[0].base) {
                            /* IsA relationship is added, instantiate */
                            ecs_entity_t tgt = ECS_PAIR_SECOND(id);
                            world->stages[0].base = tgt;
                            flecs_instantiate(world, tgt, table, offset, count);
                            world->stages[0].base = 0;
                        }
                        er_fwd = er_onset;
                    } else if (event == EcsOnRemove) {
                        er_fwd = er_unset;
                    }
                }

                /* Forward events for components from base */
                flecs_emit_forward(
                    world, er, er_fwd, ids, &it, table, event, id);
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
            ider_count = flecs_event_observers_get(er, id, iders);
            idr = idr ? idr : flecs_query_id_record_get(world, id);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
        }

        if (can_unset) {
            idr = idr ? idr : flecs_query_id_record_get(world, id);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            unset_count += (idr->type_info != NULL);
        }

        if (!ider_count && !override_ptr) {
            continue;
        }

        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
        const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t column = tr->column, storage_i = -1;
        it.columns[0] = column;
        it.ptrs[0] = NULL;
        it.sizes[0] = 0;
        it.event_id = id;
        it.ids[0] = id;

        if (count) {
            storage_i = ecs_table_type_to_storage_index(table, column);
            if (storage_i != -1) {
                ecs_assert(idr->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_vec_t *vec = &columns[storage_i];
                ecs_size_t size = idr->type_info->size;
                void *ptr = ecs_vec_get(vec, size, offset);
                it.sizes[0] = size;

                if (override_ptr) {
                    if (event == EcsOnAdd) {
                        flecs_override_copy(ti, ptr, override_ptr, count);
                    } else if (er_onset) {
                        ecs_assert(event == EcsOnRemove, ECS_INTERNAL_ERROR, NULL);
                        ecs_event_id_record_t *iders_set[5] = {0};
                        int32_t ider_set_i, ider_set_count = 
                            flecs_event_observers_get(er_onset, id, iders_set);
                        if (ider_set_count) {
                            it.sources[0] = base;
                            it.ptrs[0] = ptr;
                            for (ider_set_i = 0; ider_set_i < ider_set_count; ider_set_i ++) {
                                ecs_event_id_record_t *ider = iders_set[ider_set_i];
                                flecs_observers_invoke(world, &ider->self_up, &it, table);
                                flecs_observers_invoke(world, &ider->up, &it, table);
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

        for (ider_i = 0; ider_i < ider_count; ider_i ++) {
            ecs_event_id_record_t *ider = iders[ider_i];
            flecs_observers_invoke(world, &ider->self, &it, table);
            flecs_observers_invoke(world, &ider->self_up, &it, table);
        }

        if (!ider_count || !count || !table->observed_count) {
            continue;
        }

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

            uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
            if (flags & EcsEntityObservedAcyclic) {
                ecs_entity_t e = entities[r];
                it.sources[0] = e;
                flecs_emit_propagate(world, &it, idr, e, iders, ider_count);
            }
        }

        it.entities = entities;
        it.count = count;
        it.sources[0] = 0;
    }

    can_override = false; /* Don't override twice */
    can_unset = false; /* Don't unset twice */
    can_forward = false; /* Don't forward twice */

    if (unset_count && er_unset && (er != er_unset)) {
        unset_count = 0;
        er = er_unset;
        it.event = EcsUnSet;
        goto repeat_event;
    }

    if (wcer && er != wcer) {
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
    flecs_emit(world, stage, desc);
}

#endif
