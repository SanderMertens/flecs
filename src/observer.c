/**
 * @file observer.h
 * @brief Observer implementation.
 * 
 * The observer implementation contains functions for creating, deleting and
 * invoking observers. The code is split up into single-term observers and 
 * multi-term observers. Multi-term observers are created from multiple single-
 * term observers.
 */

#include "private_api.h"
#include <stddef.h>

static
ecs_entity_t flecs_get_observer_event(
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
ecs_flags32_t flecs_id_flag_for_event(
    ecs_entity_t e)
{
    if (e == EcsOnAdd) {
        return EcsIdHasOnAdd;
    }
    if (e == EcsOnRemove) {
        return EcsIdHasOnRemove;
    }
    if (e == EcsOnSet) {
        return EcsIdHasOnSet;
    }
    if (e == EcsUnSet) {
        return EcsIdHasUnSet;
    }
    return 0;
}

static
void flecs_inc_observer_count(
    ecs_world_t *world,
    ecs_entity_t event,
    ecs_event_record_t *evt,
    ecs_id_t id,
    int32_t value)
{
    ecs_event_id_record_t *idt = flecs_event_id_record_ensure(world, evt, id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t result = idt->observer_count += value;
    if (result == 1) {
        /* Notify framework that there are observers for the event/id. This 
         * allows parts of the code to skip event evaluation early */
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableTriggersForId,
            .event = event
        });

        ecs_flags32_t flags = flecs_id_flag_for_event(event);
        if (flags) {
            ecs_id_record_t *idr = flecs_id_record_get(world, id);
            if (idr) {
                idr->flags |= flags;
            }
        }
    } else if (result == 0) {
        /* Ditto, but the reverse */
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableNoTriggersForId,
            .event = event
        });

        ecs_flags32_t flags = flecs_id_flag_for_event(event);
        if (flags) {
            ecs_id_record_t *idr = flecs_id_record_get(world, id);
            if (idr) {
                idr->flags &= ~flags;
            }
        }

        flecs_event_id_record_remove(evt, id);
        ecs_os_free(idt);
    }
}

static
void flecs_register_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer,
    size_t offset)
{
    ecs_id_t term_id = observer->register_id;
    ecs_term_t *term = &observer->filter.terms[0];
    ecs_entity_t trav = term->src.trav;

    int i;
    for (i = 0; i < observer->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(
            term, observer->events[i]);

        /* Get observers for event */
        ecs_event_record_t *er = flecs_event_record_ensure(observable, event);
        ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get observers for (component) id for event */
        ecs_event_id_record_t *idt = flecs_event_id_record_ensure(
            world, er, term_id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *observers = ECS_OFFSET(idt, offset);
        ecs_map_init_w_params_if(observers, &world->allocators.ptr);
        ecs_map_insert_ptr(observers, observer->filter.entity, observer);

        flecs_inc_observer_count(world, event, er, term_id, 1);
        if (trav) {
            flecs_inc_observer_count(world, event, er, 
                ecs_pair(trav, EcsWildcard), 1);
        }
    }
}

static
void flecs_uni_observer_register(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer)
{
    ecs_term_t *term = &observer->filter.terms[0];
    ecs_flags32_t flags = term->src.flags;

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_register_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, self_up));
    } else if (flags & EcsSelf) {
        flecs_register_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, self));
    } else if (flags & EcsUp) {
        ecs_assert(term->src.trav != 0, ECS_INTERNAL_ERROR, NULL);
        flecs_register_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, up));
    }
}

static
void flecs_unregister_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer,
    size_t offset)
{
    ecs_id_t term_id = observer->register_id;
    ecs_term_t *term = &observer->filter.terms[0];
    ecs_entity_t trav = term->src.trav;

    int i;
    for (i = 0; i < observer->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(
            term, observer->events[i]);

        /* Get observers for event */
        ecs_event_record_t *er = flecs_event_record_get(observable, event);
        ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get observers for (component) id */
        ecs_event_id_record_t *idt = flecs_event_id_record_get(er, term_id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *id_observers = ECS_OFFSET(idt, offset);
        ecs_map_remove(id_observers, observer->filter.entity);
        if (!ecs_map_count(id_observers)) {
            ecs_map_fini(id_observers);
        }

        flecs_inc_observer_count(world, event, er, term_id, -1);
        if (trav) {
            flecs_inc_observer_count(world, event, er, 
                ecs_pair(trav, EcsWildcard), -1);
        }
    }
}

static
void flecs_unregister_observer(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer)
{
    ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!observer->filter.terms) {
        ecs_assert(observer->filter.term_count == 0, ECS_INTERNAL_ERROR, NULL);
        return;
    }

    ecs_term_t *term = &observer->filter.terms[0];
    ecs_flags32_t flags = term->src.flags;

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_unregister_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, self_up));
    } else if (flags & EcsSelf) {
        flecs_unregister_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, self));
    } else if (flags & EcsUp) {
        flecs_unregister_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, up));
    }
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
void flecs_observer_invoke(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_observer_t *observer,
    ecs_iter_action_t callback,
    ecs_table_t *table,
    int32_t term_index) 
{
    ecs_assert(it->callback != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_lock(it->world, table);
    if (ecs_should_log_3()) {
        char *path = ecs_get_fullpath(world, it->system);
        ecs_dbg_3("observer: invoke %s", path);
        ecs_os_free(path);
    }

    ecs_log_push_3();

    world->info.observers_ran_frame ++;

    ecs_filter_t *filter = &observer->filter;
    ecs_assert(term_index < filter->term_count, ECS_INTERNAL_ERROR, NULL);
    ecs_term_t *term = &filter->terms[term_index];
    ecs_entity_t observer_src = term->src.id;
    if (observer_src && !(term->src.flags & EcsIsEntity)) {
        observer_src = 0;
    }

    if (term->oper != EcsNot) {
        ecs_assert((it->offset + it->count) <= ecs_table_count(table), 
            ECS_INTERNAL_ERROR, NULL);
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

    ecs_log_pop_3();
    ecs_table_unlock(it->world, table);
}

static
void flecs_uni_observer_invoke(
    ecs_world_t *world,
    ecs_observer_t *observer,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    ecs_filter_t *filter = &observer->filter;
    ecs_term_t *term = &filter->terms[0];
    if (flecs_ignore_observer(world, observer, table)) {
        return;
    }

    ecs_assert(trav == 0 || it->sources[0] != 0, ECS_INTERNAL_ERROR, NULL);

    if (trav && term->src.trav != trav) {
        return;
    }

    bool is_filter = term->inout == EcsInOutNone;
    ECS_BIT_COND(it->flags, EcsIterIsFilter, is_filter);
    it->system = observer->filter.entity;
    it->ctx = observer->ctx;
    it->binding_ctx = observer->binding_ctx;
    it->term_index = observer->term_index;
    it->terms = term;

    ecs_entity_t event = it->event;
    it->event = flecs_get_observer_event(term, event);
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
        flecs_observer_invoke(world, it, observer, callback, table, 0);
    }

    it->event = event;
    it->ptrs = ptrs;
}

void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    if (ecs_map_is_init(observers)) {
        ecs_map_iter_t oit = ecs_map_iter(observers);
        while (ecs_map_next(&oit)) {
            ecs_observer_t *o = ecs_map_ptr(&oit);
            ecs_assert(it->table == table, ECS_INTERNAL_ERROR, NULL);
            flecs_uni_observer_invoke(world, o, it, table, trav);
        }
    }
}

static
bool flecs_multi_observer_invoke(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    ecs_world_t *world = it->real_world;

    if (o->last_event_id[0] == world->event_id) {
        /* Already handled this event */
        return false;
    }

    o->last_event_id[0] = world->event_id;

    ecs_iter_t user_it = *it;
    user_it.field_count = o->filter.field_count;
    user_it.terms = o->filter.terms;
    user_it.flags = 0;
    ECS_BIT_COND(user_it.flags, EcsIterIsFilter,    
        ECS_BIT_IS_SET(o->filter.flags, EcsFilterNoData));
    user_it.ids = NULL;
    user_it.columns = NULL;
    user_it.sources = NULL;
    user_it.sizes = NULL;
    user_it.ptrs = NULL;
    flecs_iter_init(it->world, &user_it, flecs_iter_cache_all);

    ecs_table_t *table = it->table;
    ecs_table_t *prev_table = it->other_table;
    int32_t pivot_term = it->term_index;
    ecs_term_t *term = &o->filter.terms[pivot_term];

    int32_t column = it->columns[0];
    if (term->oper == EcsNot) {
        table = it->other_table;
        prev_table = it->table;
    }

    if (!table) {
        table = &world->store.root;
    }
    if (!prev_table) {
        prev_table = &world->store.root;
    }

    if (column < 0) {
        column = -column;
    }

    user_it.columns[0] = 0;
    user_it.columns[pivot_term] = column;
    user_it.sources[pivot_term] = it->sources[0];

    if (flecs_filter_match_table(world, &o->filter, table, user_it.ids, 
        user_it.columns, user_it.sources, NULL, NULL, false, pivot_term, 
        user_it.flags))
    {
        /* Monitor observers only invoke when the filter matches for the first
         * time with an entity */
        if (o->is_monitor) {
            if (flecs_filter_match_table(world, &o->filter, prev_table, 
                NULL, NULL, NULL, NULL, NULL, true, -1, user_it.flags)) 
            {
                goto done;
            }
        }

        /* While filter matching needs to be reversed for a Not term, the
         * component data must be fetched from the table we got notified for.
         * Repeat the matching process for the non-matching table so we get the
         * correct column ids and sources, which we need for populate_data */
        if (term->oper == EcsNot) {
            flecs_filter_match_table(world, &o->filter, prev_table, user_it.ids, 
                user_it.columns, user_it.sources, NULL, NULL, false, -1, 
                user_it.flags | EcsFilterPopulate);
        }
        
        flecs_iter_populate_data(world, &user_it, it->table, it->offset, 
            it->count, user_it.ptrs, user_it.sizes);

        if (it->ptrs) {
            user_it.ptrs[pivot_term] = it->ptrs[0];
            user_it.sizes[pivot_term] = it->sizes[0];
        }
        user_it.ids[pivot_term] = it->event_id;
        user_it.system = o->filter.entity;
        user_it.term_index = pivot_term;
        user_it.ctx = o->ctx;
        user_it.binding_ctx = o->binding_ctx;
        user_it.field_count = o->filter.field_count;
        user_it.callback = o->callback;
        
        flecs_iter_validate(&user_it);
        flecs_observer_invoke(world, &user_it, o, o->callback, table, pivot_term);
        ecs_iter_fini(&user_it);
        return true;
    }

done:
    ecs_iter_fini(&user_it);
    return false;
}

bool ecs_observer_default_run_action(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    if (o->is_multi) {
        return flecs_multi_observer_invoke(it);
    } else {
        it->ctx = o->ctx;
        flecs_observer_invoke(it->real_world, it, o, o->callback, it->table, 0);
        return true;
    }
}

static
void flecs_default_multi_observer_run_callback(ecs_iter_t *it) {
    flecs_multi_observer_invoke(it);
}

void flecs_default_uni_observer_run_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    it->ctx = o->ctx;
    it->callback = o->callback;

    if (ecs_should_log_3()) {
        char *path = ecs_get_fullpath(it->world, it->system);
        ecs_dbg_3("observer %s", path);
        ecs_os_free(path);
    }

    ecs_log_push_3();
    flecs_observer_invoke(it->real_world, it, o, o->callback, it->table, 0);
    ecs_log_pop_3();
}

/* For convenience, so applications can (in theory) use a single run callback 
 * that uses ecs_iter_next to iterate results */
bool flecs_default_observer_next_callback(ecs_iter_t *it) {
    if (it->interrupted_by) {
        return false;
    } else {
        /* Use interrupted_by to signal the next iteration must return false */
        it->interrupted_by = it->system;
        return true;
    }
}

/* Run action for children of multi observer */
static
void flecs_multi_observer_builtin_run(ecs_iter_t *it) {
    ecs_observer_t *observer = it->ctx;
    ecs_run_action_t run = observer->run;

    if (run) {
        it->next = flecs_default_observer_next_callback;
        it->callback = flecs_default_multi_observer_run_callback;
        it->interrupted_by = 0;
        run(it);
    } else {
        flecs_multi_observer_invoke(it);
    }
}

static
void flecs_uni_observer_trigger_existing(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    ecs_iter_action_t callback = observer->callback;

    /* If yield existing is enabled, observer for each thing that matches
     * the event, if the event is iterable. */
    int i, count = observer->event_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t evt = observer->events[i];
        const EcsIterable *iterable = ecs_get(world, evt, EcsIterable);
        if (!iterable) {
            continue;
        }

        ecs_iter_t it;
        iterable->init(world, world, &it, &observer->filter.terms[0]);
        it.system = observer->filter.entity;
        it.ctx = observer->ctx;
        it.binding_ctx = observer->binding_ctx;
        it.event = evt;

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            it.event_id = it.ids[0];
            callback(&it);
        }

        ecs_iter_fini(&it);
    }
}

static
void flecs_multi_observer_yield_existing(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    ecs_run_action_t run = observer->run;
    if (!run) {
        run = flecs_default_multi_observer_run_callback;
    }

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    int32_t pivot_term = ecs_filter_pivot_term(world, &observer->filter);
    if (pivot_term < 0) {
        return;
    }

    /* If yield existing is enabled, invoke for each thing that matches
     * the event, if the event is iterable. */
    int i, count = observer->event_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t evt = observer->events[i];
        const EcsIterable *iterable = ecs_get(world, evt, EcsIterable);
        if (!iterable) {
            continue;
        }

        ecs_iter_t it;
        iterable->init(world, world, &it, &observer->filter.terms[pivot_term]);
        it.terms = observer->filter.terms;
        it.field_count = 1;
        it.term_index = pivot_term;
        it.system = observer->filter.entity;
        it.ctx = observer;
        it.binding_ctx = observer->binding_ctx;
        it.event = evt;

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            it.event_id = it.ids[0];
            run(&it);
            world->event_id ++;
        }
    }
}

static
int flecs_uni_observer_init(
    ecs_world_t *world,
    ecs_observer_t *observer,
    const ecs_observer_desc_t *desc)
{
    ecs_term_t *term = &observer->filter.terms[0];
    observer->last_event_id = desc->last_event_id;    
    observer->register_id = flecs_from_public_id(world, term->id);
    term->field_index = desc->term_index;

    if (ecs_id_is_tag(world, term->id)) {
        /* If id is a tag, downgrade OnSet/UnSet to OnAdd/OnRemove. */
        int32_t e, count = observer->event_count;
        for (e = 0; e < count; e ++) {
            if (observer->events[e] == EcsOnSet) {
                observer->events[e] = EcsOnAdd;
            } else
            if (observer->events[e] == EcsUnSet) {
                observer->events[e] = EcsOnRemove;
            }
        }
    }

    flecs_uni_observer_register(world, observer->observable, observer);

    if (desc->yield_existing) {
        flecs_uni_observer_trigger_existing(world, observer);
    }

    return 0;
}

static
int flecs_multi_observer_init(
    ecs_world_t *world,
    ecs_observer_t *observer,
    const ecs_observer_desc_t *desc)
{
    /* Create last event id for filtering out the same event that arrives from
     * more than one term */
    observer->last_event_id = ecs_os_calloc_t(int32_t);
    
    /* Mark observer as multi observer */
    observer->is_multi = true;

    /* Create a child observer for each term in the filter */
    ecs_filter_t *filter = &observer->filter;
    ecs_observer_desc_t child_desc = *desc;
    child_desc.last_event_id = observer->last_event_id;
    child_desc.run = NULL;
    child_desc.callback = flecs_multi_observer_builtin_run;
    child_desc.ctx = observer;
    child_desc.ctx_free = NULL;
    child_desc.filter.expr = NULL;
    child_desc.filter.terms_buffer = NULL;
    child_desc.filter.terms_buffer_count = 0;
    child_desc.binding_ctx = NULL;
    child_desc.binding_ctx_free = NULL;
    child_desc.yield_existing = false;
    ecs_os_zeromem(&child_desc.entity);
    ecs_os_zeromem(&child_desc.filter.terms);
    ecs_os_memcpy_n(child_desc.events, observer->events, 
        ecs_entity_t, observer->event_count);

    int i, term_count = filter->term_count;
    bool optional_only = filter->flags & EcsFilterMatchThis;
    for (i = 0; i < term_count; i ++) {
        if (filter->terms[i].oper != EcsOptional) {
            if (ecs_term_match_this(&filter->terms[i])) {
                optional_only = false;
            }
        }
    }

    if (filter->flags & EcsFilterMatchPrefab) {
        child_desc.filter.flags |= EcsFilterMatchPrefab;
    }
    if (filter->flags & EcsFilterMatchDisabled) {
        child_desc.filter.flags |= EcsFilterMatchDisabled;
    }

    /* Create observers as children of observer */
    ecs_entity_t old_scope = ecs_set_scope(world, observer->filter.entity);

    for (i = 0; i < term_count; i ++) {
        if (filter->terms[i].src.flags & EcsFilter) {
            continue;
        }

        ecs_term_t *term = &child_desc.filter.terms[0];
        child_desc.term_index = filter->terms[i].field_index;
        *term = filter->terms[i];

        ecs_oper_kind_t oper = term->oper;
        ecs_id_t id = term->id;

        /* AndFrom & OrFrom terms insert multiple observers */
        if (oper == EcsAndFrom || oper == EcsOrFrom) {
            const ecs_type_t *type = ecs_get_type(world, id);
            int32_t ti, ti_count = type->count;
            ecs_id_t *ti_ids = type->array;

            /* Correct operator will be applied when an event occurs, and
             * the observer is evaluated on the observer source */
            term->oper = EcsAnd;
            for (ti = 0; ti < ti_count; ti ++) {
                ecs_id_t ti_id = ti_ids[ti];
                ecs_id_record_t *idr = flecs_id_record_get(world, ti_id);
                if (idr->flags & EcsIdDontInherit) {
                    continue;
                }

                term->first.name = NULL;
                term->first.id = ti_ids[ti];
                term->id = ti_ids[ti];

                if (ecs_observer_init(world, &child_desc) == 0) {
                    goto error;
                }
            }
            continue;
        }

        /* If observer only contains optional terms, match everything */
        if (optional_only) {
            term->id = EcsAny;
            term->first.id = EcsAny;
            term->src.id = EcsThis;
            term->src.flags = EcsIsVariable;
            term->second.id = 0;
        } else if (term->oper == EcsOptional) {
            continue;
        }
        if (ecs_observer_init(world, &child_desc) == 0) {
            goto error;
        }

        if (optional_only) {
            break;
        }
    }

    ecs_set_scope(world, old_scope);

    if (desc->yield_existing) {
        flecs_multi_observer_yield_existing(world, observer);
    }

    return 0; 
error:
    return -1;
}

ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, NULL);

    ecs_entity_t entity = desc->entity;
    if (!entity) {
        entity = ecs_new(world, 0);
    }
    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_observer_t);
    if (!poly->poly) {
        ecs_check(desc->callback != NULL || desc->run != NULL, 
            ECS_INVALID_OPERATION, NULL);

        ecs_observer_t *observer = ecs_poly_new(ecs_observer_t);
        ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
        observer->dtor = (ecs_poly_dtor_t)flecs_observer_fini;

        /* Make writeable copy of filter desc so that we can set name. This will
         * make debugging easier, as any error messages related to creating the
         * filter will have the name of the observer. */
        ecs_filter_desc_t filter_desc = desc->filter;
        filter_desc.entity = entity;
        ecs_filter_t *filter = filter_desc.storage = &observer->filter;
        *filter = ECS_FILTER_INIT;

        /* Parse filter */
        if (ecs_filter_init(world, &filter_desc) == NULL) {
            flecs_observer_fini(observer);
            return 0;
        }

        /* Observer must have at least one term */
        ecs_check(observer->filter.term_count > 0, ECS_INVALID_PARAMETER, NULL);

        poly->poly = observer;

        ecs_observable_t *observable = desc->observable;
        if (!observable) {
            observable = ecs_get_observable(world);
        }

        observer->run = desc->run;
        observer->callback = desc->callback;
        observer->ctx = desc->ctx;
        observer->binding_ctx = desc->binding_ctx;
        observer->ctx_free = desc->ctx_free;
        observer->binding_ctx_free = desc->binding_ctx_free;
        observer->term_index = desc->term_index;
        observer->observable = observable;

        /* Check if observer is monitor. Monitors are created as multi observers
         * since they require pre/post checking of the filter to test if the
         * entity is entering/leaving the monitor. */
        int i;
        for (i = 0; i < ECS_OBSERVER_DESC_EVENT_COUNT_MAX; i ++) {
            ecs_entity_t event = desc->events[i];
            if (!event) {
                break;
            }

            if (event == EcsMonitor) {
                /* Monitor event must be first and last event */
                ecs_check(i == 0, ECS_INVALID_PARAMETER, NULL);

                observer->events[0] = EcsOnAdd;
                observer->events[1] = EcsOnRemove;
                observer->event_count ++;
                observer->is_monitor = true;
            } else {
                observer->events[i] = event;
            }

            observer->event_count ++;
        }

        /* Observer must have at least one event */
        ecs_check(observer->event_count != 0, ECS_INVALID_PARAMETER, NULL);

        bool multi = false;

        if (filter->term_count == 1 && !desc->last_event_id) {
            ecs_term_t *term = &filter->terms[0];
            /* If the filter has a single term but it is a *From operator, we
             * need to create a multi observer */
            multi |= (term->oper == EcsAndFrom) || (term->oper == EcsOrFrom);
            
            /* An observer with only optional terms is a special case that is
             * only handled by multi observers */
            multi |= term->oper == EcsOptional;
        }

        if (filter->term_count == 1 && !observer->is_monitor && !multi) {
            if (flecs_uni_observer_init(world, observer, desc)) {
                goto error;
            }
        } else {
            if (flecs_multi_observer_init(world, observer, desc)) {
                goto error;
            }
        }

        if (ecs_get_name(world, entity)) {
            ecs_trace("#[green]observer#[reset] %s created", 
                ecs_get_name(world, entity));
        }
    } else {
        /* If existing entity handle was provided, override existing params */
        if (desc->callback) {
            ecs_poly(poly->poly, ecs_observer_t)->callback = desc->callback;
        }
        if (desc->ctx) {
            ecs_poly(poly->poly, ecs_observer_t)->ctx = desc->ctx;
        }
        if (desc->binding_ctx) {
            ecs_poly(poly->poly, ecs_observer_t)->binding_ctx = desc->binding_ctx;
        }
    }

    ecs_poly_modified(world, entity, ecs_observer_t);

    return entity;
error:
    ecs_delete(world, entity);
    return 0;
}

void* ecs_get_observer_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsPoly *o = ecs_poly_bind_get(world, observer, ecs_observer_t);
    if (o) {
        ecs_poly_assert(o->poly, ecs_observer_t);
        return ((ecs_observer_t*)o->poly)->ctx;
    } else {
        return NULL;
    }     
}

void* ecs_get_observer_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsPoly *o = ecs_poly_bind_get(world, observer, ecs_observer_t);
    if (o) {
        ecs_poly_assert(o->poly, ecs_observer_t);
        return ((ecs_observer_t*)o->poly)->binding_ctx;
    } else {
        return NULL;
    }      
}

void flecs_observer_fini(
    ecs_observer_t *observer)
{
    if (observer->is_multi) {
        /* Child observers get deleted up by entity cleanup logic */
        ecs_os_free(observer->last_event_id);
    } else {
        if (observer->filter.term_count) {
            flecs_unregister_observer(
                observer->filter.world, observer->observable, observer);
        } else {
            /* Observer creation failed while creating filter */
        }
    }

    /* Cleanup filters */
    ecs_filter_fini(&observer->filter);

    /* Cleanup context */
    if (observer->ctx_free) {
        observer->ctx_free(observer->ctx);
    }

    if (observer->binding_ctx_free) {
        observer->binding_ctx_free(observer->binding_ctx);
    }

    ecs_poly_free(observer, ecs_observer_t);
}
