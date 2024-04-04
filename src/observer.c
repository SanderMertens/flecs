/**
 * @file observer.c
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
    if (e == EcsOnTableFill) {
        return EcsIdHasOnTableFill;
    }
    if (e == EcsOnTableEmpty) {
        return EcsIdHasOnTableEmpty;
    }
    if (e == EcsOnTableCreate) {
        return EcsIdHasOnTableCreate;
    }
    if (e == EcsOnTableDelete) {
        return EcsIdHasOnTableDelete;
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
ecs_id_t flecs_observer_id(
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        if (ECS_PAIR_FIRST(id) == EcsAny) {
            id = ecs_pair(EcsWildcard, ECS_PAIR_SECOND(id));
        }
        if (ECS_PAIR_SECOND(id) == EcsAny) {
            id = ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard);
        }
    }

    return id;
}

static
void flecs_register_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer,
    size_t offset)
{
    ecs_id_t term_id = flecs_observer_id(observer->register_id);
    ecs_term_t *term = &observer->query->terms[0];
    ecs_entity_t trav = term->trav;

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
        ecs_map_insert_ptr(observers, observer->id, observer);

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
    ecs_term_t *term = &observer->query->terms[0];
    ecs_flags64_t flags = ECS_TERM_REF_FLAGS(&term->src);

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_register_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, self_up));
    } else if (flags & EcsSelf) {
        flecs_register_observer_for_id(world, observable, observer,
            offsetof(ecs_event_id_record_t, self));
    } else if (flags & EcsUp) {
        ecs_assert(term->trav != 0, ECS_INTERNAL_ERROR, NULL);
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
    ecs_id_t term_id = flecs_observer_id(observer->register_id);
    ecs_term_t *term = &observer->query->terms[0];
    ecs_entity_t trav = term->trav;

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
        ecs_map_remove(id_observers, observer->id);
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
    if (observer->query->term_count == 0) {
        return;
    }

    ecs_term_t *term = &observer->query->terms[0];
    ecs_flags64_t flags = ECS_TERM_REF_FLAGS(&term->src);

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
    ecs_observer_t *observer,
    ecs_table_t *table,
    ecs_iter_t *it)
{
    ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t *last_event_id = observer->last_event_id;
    if (last_event_id && last_event_id[0] == it->event_cur) {
        return true;
    }

    if (observer->flags & (EcsObserverIsDisabled|EcsObserverIsParentDisabled)) {
        return true;
    }

    ecs_flags32_t table_flags = table->flags, query_flags = observer->query->flags;

    bool result = (table_flags & EcsTableIsPrefab) &&
        !(query_flags & EcsQueryMatchPrefab);
    result = result || ((table_flags & EcsTableIsDisabled) &&
        !(query_flags & EcsQueryMatchDisabled));

    return result;
}

static
bool flecs_is_simple_result(
    ecs_iter_t *it)
{
    return (it->count == 1) || (it->sizes[0] == 0) || (it->sources[0] == 0);
}

static
void flecs_observer_invoke(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_observer_t *observer,
    ecs_iter_action_t callback,
    int32_t term_index,
    bool simple_result) 
{
    ecs_assert(it->callback != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ecs_should_log_3()) {
        char *path = ecs_get_fullpath(world, it->system);
        ecs_dbg_3("observer: invoke %s", path);
        ecs_os_free(path);
    }

    ecs_log_push_3();

    ecs_entity_t old_system = flecs_stage_set_system(
        &world->stages[0], observer->entity);
    world->info.observers_ran_frame ++;

    ecs_query_t *query = observer->query;
    ecs_assert(term_index < query->term_count, ECS_INTERNAL_ERROR, NULL);
    ecs_term_t *term = &query->terms[term_index];
    if (it->table && (term->oper != EcsNot)) {
        ecs_assert((it->offset + it->count) <= ecs_table_count(it->table), 
            ECS_INTERNAL_ERROR, NULL);
    }

    bool instanced = query->flags & EcsQueryIsInstanced;
    bool match_this = query->flags & EcsQueryMatchThis;
    bool table_only = it->flags & EcsIterTableOnly;

    if (match_this && (simple_result || instanced || table_only)) {
        callback(it);
        query->eval_count ++;
    } else {
        ecs_entity_t observer_src = ECS_TERM_REF_ID(&term->src);
        if (observer_src && !(term->src.id & EcsIsEntity)) {
            observer_src = 0;
        }

        ecs_entity_t *entities = it->entities;
        int32_t i, count = it->count;
        ecs_entity_t src = it->sources[0];
        it->count = 1;
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = entities[i];
            it->entities = &e;
            if (!observer_src) {
                callback(it);
                query->eval_count ++;
            } else if (observer_src == e) {
                ecs_entity_t dummy = 0;
                it->entities = &dummy;
                if (!src) {
                    it->sources[0] = e;
                }

                callback(it);
                query->eval_count ++;
                it->sources[0] = src;
                break;
            }
        }
        it->entities = entities;
        it->count = count;
    }

    flecs_stage_set_system(&world->stages[0], old_system);

    ecs_log_pop_3();
}

static
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
    flecs_observer_invoke(it->real_world, it, o, o->callback, 0,
        flecs_is_simple_result(it));
    ecs_log_pop_3();
}

static
void flecs_uni_observer_invoke(
    ecs_world_t *world,
    ecs_observer_t *observer,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav,
    bool simple_result)
{
    ecs_query_t *query = observer->query;
    ecs_term_t *term = &query->terms[0];
    if (flecs_ignore_observer(observer, table, it)) {
        return;
    }

    ecs_assert(trav == 0 || it->sources[0] != 0, ECS_INTERNAL_ERROR, NULL);
    if (trav && term->trav != trav) {
        return;
    }

    bool is_filter = term->inout == EcsInOutNone;
    ECS_BIT_COND(it->flags, EcsIterNoData, is_filter);
    it->system = observer->entity;
    it->ctx = observer->ctx;
    it->binding_ctx = observer->binding_ctx;
    it->term_index = observer->term_index;
    it->query = observer->query;

    ecs_entity_t event = it->event;
    int32_t event_cur = it->event_cur;
    it->event = flecs_get_observer_event(term, event);

    if (observer->run) {
        it->next = flecs_default_observer_next_callback;
        it->callback = flecs_default_uni_observer_run_callback;
        it->ctx = observer;
        observer->run(it);
    } else {
        ecs_iter_action_t callback = observer->callback;
        it->callback = callback;
        flecs_observer_invoke(world, it, observer, callback, 0, simple_result);
    }

    it->event = event;
    it->event_cur = event_cur;
}

void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    if (ecs_map_is_init(observers)) {
        ecs_table_lock(it->world, table);

        bool simple_result = flecs_is_simple_result(it);
        ecs_map_iter_t oit = ecs_map_iter(observers);
        while (ecs_map_next(&oit)) {
            ecs_observer_t *o = ecs_map_ptr(&oit);
            ecs_assert(it->table == table, ECS_INTERNAL_ERROR, NULL);
            flecs_uni_observer_invoke(
                world, o, it, table, trav, simple_result);
        }

        ecs_table_unlock(it->world, table);
    }
}

static
bool flecs_multi_observer_invoke(
    ecs_iter_t *it) 
{
    ecs_observer_t *o = it->ctx;
    ecs_world_t *world = it->real_world;

    if (o->last_event_id[0] == it->event_cur) {
        /* Already handled this event */
        return false;
    }

    o->last_event_id[0] = world->event_id;

    ecs_table_t *table = it->table;
    ecs_table_t *prev_table = it->other_table;
    int32_t pivot_term = it->term_index;
    ecs_term_t *term = &o->query->terms[pivot_term];

    bool is_not = term->oper == EcsNot;
    if (is_not) {
        table = it->other_table;
        prev_table = it->table;
    }

    table = table ? table : &world->store.root;
    prev_table = prev_table ? prev_table : &world->store.root;

    ecs_iter_t user_it;

    bool match;
    if (is_not) {
        match = ecs_query_has_table(o->query, table, &user_it);
        if (match) {
            /* The target table matches but the entity hasn't moved to it yet. 
             * Now match the not_query, which will populate the iterator with
             * data from the table the entity is still stored in. */
            ecs_iter_fini(&user_it);
            match = ecs_query_has_table(o->not_query, prev_table, &user_it);

            /* A not query replaces Not terms with Optional terms, so if the 
             * regular query matches, the not_query should also match. */
            ecs_assert(match, ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        ecs_table_range_t range = {
            .table = table,
            .offset = it->offset,
            .count = it->count
        };
        match = ecs_query_has_range(o->query, &range, &user_it);
    }

    if (match) {
        /* Monitor observers only invoke when the query matches for the first
         * time with an entity */
        if (o->flags & EcsObserverIsMonitor) {
            ecs_iter_t table_it;
            if (ecs_query_has_table(o->query, prev_table, &table_it)) {
                ecs_iter_fini(&table_it);
                ecs_iter_fini(&user_it);
                goto done;
            }
        }

        ECS_BIT_COND(user_it.flags, EcsIterNoData,    
            ECS_BIT_IS_SET(o->query->flags, EcsQueryNoData));

        /* Patch data from original iterator. If the observer query has 
         * wildcards which triggered the original event, the component id that
         * got matched by ecs_query_has_range may not be the same as the one
         * that caused the event. We need to make sure to communicate the
         * component id that actually triggered the observer. */
        int32_t pivot_field = term->field_index;
        ecs_assert(pivot_field >= 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(pivot_field < user_it.field_count, ECS_INTERNAL_ERROR, NULL);
        user_it.ids[pivot_field] = it->event_id;
        user_it.ptrs[pivot_field] = it->ptrs[0];
        user_it.columns[pivot_field] = it->columns[0];
        user_it.term_index = pivot_term;

        user_it.ctx = o->ctx;
        user_it.binding_ctx = o->binding_ctx;
        user_it.callback = o->callback;
        user_it.system = o->entity;
        user_it.event = it->event;
        user_it.event_id = it->event_id;
        user_it.other_table = it->other_table;

        ecs_entity_t old_system = flecs_stage_set_system(
            &world->stages[0], o->entity);
        ecs_table_lock(it->world, table);

        user_it.callback(&user_it);

        ecs_table_unlock(it->world, table);
        flecs_stage_set_system(&world->stages[0], old_system);

        ecs_iter_fini(&user_it);
        return true;
    } else {
        /* While the observer query was strictly speaking evaluated, it's more
         * useful to measure how often the observer was actually invoked. */
        o->query->eval_count --;
    }

done:
    return false;
}

bool ecs_observer_default_run_action(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    if (o->flags & EcsObserverIsMulti) {
        return flecs_multi_observer_invoke(it);
    } else {
        it->ctx = o->ctx;
        ecs_table_lock(it->world, it->table);
        flecs_observer_invoke(it->real_world, it, o, o->callback, 0,
            flecs_is_simple_result(it));
        ecs_table_unlock(it->world, it->table);
        return true;
    }
}

static
void flecs_default_multi_observer_run_callback(
    ecs_iter_t *it) 
{
    flecs_multi_observer_invoke(it);
}

/* For convenience, so applications can (in theory) use a single run callback 
 * that uses ecs_iter_next to iterate results */
bool flecs_default_observer_next_callback(ecs_iter_t *it) {
    if (it->interrupted_by) {
        return false;
    } else {
        /* Use interrupted_by to signal the next iteration must return false */
        ecs_assert(it->system != 0, ECS_INTERNAL_ERROR, NULL);
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
void flecs_observer_yield_existing(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    ecs_run_action_t run = observer->run;
    if (!run) {
        run = flecs_default_multi_observer_run_callback;
    }

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    ecs_defer_begin(world);

    /* If yield existing is enabled, invoke for each thing that matches
     * the event, if the event is iterable. */
    int i, count = observer->event_count;
    for (i = 0; i < count; i ++) {
        ecs_iter_t it = ecs_query_iter(world, observer->query);
        it.system = observer->entity;
        it.ctx = observer;
        it.binding_ctx = observer->binding_ctx;
        it.event = observer->events[i];;
        while (ecs_query_next(&it)) {
            it.event_id = it.ids[0];
            it.event_cur = ++ world->event_id;
            run(&it);
        }
    }

    ecs_defer_end(world);
}

static
int flecs_uni_observer_init(
    ecs_world_t *world,
    ecs_observer_t *observer,
    const ecs_observer_desc_t *desc)
{
    ecs_term_t *term = &observer->query->terms[0];
    observer->last_event_id = desc->last_event_id;    
    if (!observer->last_event_id) {
        observer->last_event_id = &observer->last_event_id_storage;
    }
    observer->register_id = term->id;
    term->field_index = flecs_ito(int16_t, desc->term_index);

    if (ecs_id_is_tag(world, term->id)) {
        /* If id is a tag, downgrade OnSet/UnSet to OnAdd/OnRemove. */
        int32_t e, count = observer->event_count;
        bool has_on_add = false;
        bool has_on_remove = false;
        for (e = 0; e < count; e ++) {
            if (observer->events[e] == EcsOnAdd) {
                has_on_add = true;
            }
            if (observer->events[e] == EcsOnRemove) {
                has_on_remove = true;
            }
        }

        for (e = 0; e < count; e ++) {
            if (observer->events[e] == EcsOnSet) {
                if (has_on_add) {
                    /* Already registered */
                    observer->events[e] = 0;
                } else {
                    observer->events[e] = EcsOnAdd;
                }
            } else
            if (observer->events[e] == EcsUnSet) {
                if (has_on_remove) {
                    /* Already registered */
                    observer->events[e] = 0;
                } else {
                    observer->events[e] = EcsOnRemove;
                }
            }
        }
    }

    flecs_uni_observer_register(world, observer->observable, observer);

    return 0;
}

static
int flecs_observer_add_child(
    ecs_world_t *world,
    ecs_observer_t *observer,
    const ecs_observer_desc_t *child_desc)
{
    ecs_observer_t *child_observer = flecs_observer_init(
        world, 0, child_desc);
    if (!child_observer) {
        return -1;
    }

    ecs_vec_append_t(&world->allocator, &observer->children, 
        ecs_observer_t*)[0] = child_observer;
    child_observer->entity = observer->entity;
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
    observer->flags |= EcsObserverIsMulti;

    /* Vector that stores a single-component observer for each query term */
    ecs_vec_init_t(&world->allocator, &observer->children, ecs_observer_t*, 2);

    /* Create a child observer for each term in the query */
    ecs_query_t *query = observer->query;
    ecs_observer_desc_t child_desc = *desc;
    child_desc.last_event_id = observer->last_event_id;
    child_desc.run = NULL;
    child_desc.callback = flecs_multi_observer_builtin_run;
    child_desc.ctx = observer;
    child_desc.ctx_free = NULL;
    child_desc.query.expr = NULL;
    child_desc.binding_ctx = NULL;
    child_desc.binding_ctx_free = NULL;
    child_desc.yield_existing = false;
    ecs_os_zeromem(&child_desc.entity);
    ecs_os_zeromem(&child_desc.query.terms);
    ecs_os_zeromem(&child_desc.query);
    ecs_os_memcpy_n(child_desc.events, observer->events, 
        ecs_entity_t, observer->event_count);

    int i, term_count = query->term_count;
    bool optional_only = query->flags & EcsQueryMatchThis;
    bool has_not = false;
    for (i = 0; i < term_count; i ++) {
        if (query->terms[i].oper != EcsOptional) {
            if (ecs_term_match_this(&query->terms[i])) {
                optional_only = false;
            }
        }

        if ((query->terms[i].oper == EcsNot) && 
            (query->terms[i].inout != EcsInOutFilter)) 
        {
            has_not = true;
        }
    }

    if (query->flags & EcsQueryMatchPrefab) {
        child_desc.query.flags |= EcsQueryMatchPrefab;
    }

    if (query->flags & EcsQueryMatchDisabled) {
        child_desc.query.flags |= EcsQueryMatchDisabled;
    }

    for (i = 0; i < term_count; i ++) {
        if (query->terms[i].inout == EcsInOutFilter) {
            continue;
        }

        ecs_term_t *term = &child_desc.query.terms[0];
        child_desc.term_index = query->terms[i].field_index;
        *term = query->terms[i];

        int16_t oper = term->oper;
        ecs_id_t id = term->id;

        /* AndFrom & OrFrom terms insert multiple observers */
        if (oper == EcsAndFrom || oper == EcsOrFrom) {
            const ecs_type_t *type = ecs_get_type(world, id);
            if (!type) {
                continue;
            }

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

                if (flecs_observer_add_child(world, observer, &child_desc)) {
                    goto error;
                }
            }
            continue;
        }

        /* Single component observers never use OR */
        if (oper == EcsOr) {
            term->oper = EcsAnd;
        }

        /* If observer only contains optional terms, match everything */
        if (optional_only) {
            term->id = EcsAny;
            term->first.id = EcsAny;
            term->src.id = EcsThis | EcsIsVariable;
            term->second.id = 0;
        } else if (term->oper == EcsOptional) {
            continue;
        }

        if (flecs_observer_add_child(world, observer, &child_desc)) {
            goto error;
        }

        if (optional_only) {
            break;
        }
    }

    /* If observer has Not terms, we need to create a query that replaces Not
     * with Optional which we can use to populate the observer data for the 
     * table that the entity moved away from (or to, if it's an OnRemove 
     * observer). */
    if (has_not) {
        ecs_query_desc_t not_desc = desc->query;
        not_desc.expr = NULL;

        ecs_os_memcpy_n(not_desc.terms, observer->query->terms, 
            ecs_term_t, term_count); /* cast suppresses warning */
  
        for (i = 0; i < term_count; i ++) {
            if (not_desc.terms[i].oper == EcsNot) {
                not_desc.terms[i].oper = EcsOptional;
            }
        }

        observer->not_query = ecs_query_init(world, &not_desc);
    }

    return 0; 
error:
    return -1;
}

ecs_observer_t* flecs_observer_init(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_observer_desc_t *desc)
{
    ecs_check(desc->callback != NULL || desc->run != NULL, 
        ECS_INVALID_OPERATION, NULL);

    ecs_observer_t *observer = flecs_sparse_add_t(
        &world->store.observers, ecs_observer_t);
    ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_poly_init(observer, ecs_observer_t);
    observer->id = flecs_sparse_last_id(&world->store.observers);
    observer->dtor = (ecs_poly_dtor_t)flecs_observer_fini;

    /* Make writeable copy of query desc so that we can set name. This will
     * make debugging easier, as any error messages related to creating the
     * query will have the name of the observer. */
    ecs_query_desc_t query_desc = desc->query;
    query_desc.entity = 0;
    query_desc.cache_kind = EcsQueryCacheNone;

    /* Create query */
    ecs_query_t *query = observer->query = ecs_query_init(
        world, &query_desc);
    if (query == NULL) {
        flecs_observer_fini(observer);
        return 0;
    }

    ecs_poly_assert(query, ecs_query_t);

    /* Observer must have at least one term */
    ecs_check(observer->query->term_count > 0, ECS_INVALID_PARAMETER, NULL);

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
    observer->entity = entity;

    /* Check if observer is monitor. Monitors are created as multi observers
     * since they require pre/post checking of the filter to test if the
     * entity is entering/leaving the monitor. */
    int i;
    for (i = 0; i < FLECS_EVENT_DESC_MAX; i ++) {
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
            observer->flags |= EcsObserverIsMonitor;
        } else {
            observer->events[i] = event;
        }

        observer->event_count ++;
    }

    /* Observer must have at least one event */
    ecs_check(observer->event_count != 0, ECS_INVALID_PARAMETER, NULL);

    bool multi = false;

    if (query->term_count == 1 && !desc->last_event_id) {
        ecs_term_t *term = &query->terms[0];
        /* If the query has a single term but it is a *From operator, we
         * need to create a multi observer */
        multi |= (term->oper == EcsAndFrom) || (term->oper == EcsOrFrom);
        
        /* An observer with only optional terms is a special case that is
         * only handled by multi observers */
        multi |= term->oper == EcsOptional;
    }

    bool is_monitor = observer->flags & EcsObserverIsMonitor;
    if (query->term_count == 1 && !is_monitor && !multi) {
        if (flecs_uni_observer_init(world, observer, desc)) {
            goto error;
        }
    } else {
        if (flecs_multi_observer_init(world, observer, desc)) {
            goto error;
        }
    }

    if (desc->yield_existing) {
        flecs_observer_yield_existing(world, observer);
    }

    return observer;
error:
    return NULL;
}

ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc)
{
    ecs_entity_t entity = 0;
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, NULL);

    entity = desc->entity;
    if (!entity) {
        entity = ecs_new(world, 0);
    }

    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_observer_t);
    if (!poly->poly) {
        ecs_observer_t *observer = flecs_observer_init(world, entity, desc);
        ecs_assert(observer->entity == entity, ECS_INTERNAL_ERROR, NULL);
        poly->poly = observer;

        if (ecs_get_name(world, entity)) {
            ecs_trace("#[green]observer#[reset] %s created", 
                ecs_get_name(world, entity));
        }
    } else {
        ecs_poly_assert(poly->poly, ecs_observer_t);
        ecs_observer_t *observer = (ecs_observer_t*)poly->poly;

        if (desc->run) {
            observer->run = desc->run;
        }

        if (desc->callback) {
            observer->callback = desc->callback;
        }

        if (observer->ctx_free) {
            if (observer->ctx && observer->ctx != desc->ctx) {
                observer->ctx_free(observer->ctx);
            }
        }

        if (observer->binding_ctx_free) {
            if (observer->binding_ctx && 
                observer->binding_ctx != desc->binding_ctx) 
            {
                observer->binding_ctx_free(observer->binding_ctx);
            }
        }

        if (desc->ctx) {
            observer->ctx = desc->ctx;
        }

        if (desc->binding_ctx) {
            observer->binding_ctx = desc->binding_ctx;
        }

        if (desc->ctx_free) {
            observer->ctx_free = desc->ctx_free;
        }

        if (desc->binding_ctx_free) {
            observer->binding_ctx_free = desc->binding_ctx_free;
        }
    }

    ecs_poly_modified(world, entity, ecs_observer_t);

    return entity;
error:
    if (entity) {
        ecs_delete(world, entity);
    }
    return 0;
}

void* ecs_observer_get_ctx(
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

void* ecs_observer_get_binding_ctx(
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

const ecs_query_t* ecs_observer_get_query(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsPoly *o = ecs_poly_bind_get(world, observer, ecs_observer_t);
    if (o) {
        ecs_poly_assert(o->poly, ecs_observer_t);
        return ((ecs_observer_t*)o->poly)->query;
    } else {
        return NULL;
    }   
}

void flecs_observer_fini(
    ecs_observer_t *observer)
{
    ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(observer->query != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = observer->query->world;

    if (observer->flags & EcsObserverIsMulti) {
        ecs_observer_t **children = ecs_vec_first(&observer->children);
        int32_t i, children_count = ecs_vec_count(&observer->children);

        for (i = 0; i < children_count; i ++) {
            flecs_observer_fini(children[i]);
        }

        ecs_os_free(observer->last_event_id);
    } else {
        if (observer->query->term_count) {
            flecs_unregister_observer(
                world, observer->observable, observer);
        } else {
            /* Observer creation failed while creating query */
        }
    }

    ecs_vec_fini_t(&world->allocator, &observer->children, ecs_observer_t*);

    /* Cleanup queries */
    ecs_query_fini(observer->query);
    if (observer->not_query) {
        ecs_query_fini(observer->not_query);
    }

    /* Cleanup context */
    if (observer->ctx_free) {
        observer->ctx_free(observer->ctx);
    }

    if (observer->binding_ctx_free) {
        observer->binding_ctx_free(observer->binding_ctx);
    }

    ecs_poly_fini(observer, ecs_observer_t);
    flecs_sparse_remove_t(
        &world->store.observers, ecs_observer_t, observer->id);
}

void flecs_observer_set_disable_bit(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_flags32_t bit,
    bool cond)
{
    const EcsPoly *poly = ecs_get_pair(world, e, EcsPoly, EcsObserver);
    if (!poly || !poly->poly) {
        return;
    }

    ecs_observer_t *o = poly->poly;
    if (o->flags & EcsObserverIsMulti) {
        ecs_observer_t **children = ecs_vec_first(&o->children);
        int32_t i, children_count = ecs_vec_count(&o->children);
        if (children_count) {
            for (i = 0; i < children_count; i ++) {
                ECS_BIT_COND(children[i]->flags, bit, cond);
            }
        }
    } else {
        ecs_poly_assert(o, ecs_observer_t);
        ECS_BIT_COND(o->flags, bit, cond);
    }
}
