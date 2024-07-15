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
        if (event == EcsOnAdd || event == EcsOnSet) {
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
    if (e == EcsWildcard) {
        return EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|
            EcsIdHasOnTableFill|EcsIdHasOnTableEmpty|
            EcsIdHasOnTableCreate|EcsIdHasOnTableDelete;
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
    ecs_observer_t *o,
    size_t offset)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_id_t term_id = flecs_observer_id(impl->register_id);
    ecs_term_t *term = &o->query->terms[0];
    ecs_entity_t trav = term->trav;

    int i;
    for (i = 0; i < o->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(
            term, o->events[i]);

        /* Get observers for event */
        ecs_event_record_t *er = flecs_event_record_ensure(observable, event);
        ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get observers for (component) id for event */
        ecs_event_id_record_t *idt = flecs_event_id_record_ensure(
            world, er, term_id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *observers = ECS_OFFSET(idt, offset);
        ecs_map_init_w_params_if(observers, &world->allocators.ptr);
        ecs_map_insert_ptr(observers, impl->id, o);

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
    ecs_observer_t *o)
{
    ecs_term_t *term = &o->query->terms[0];
    ecs_flags64_t flags = ECS_TERM_REF_FLAGS(&term->src);

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_register_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self_up));
    } else if (flags & EcsSelf) {
        flecs_register_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self));
    } else if (flags & EcsUp) {
        ecs_assert(term->trav != 0, ECS_INTERNAL_ERROR, NULL);
        flecs_register_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, up));
    }
}

static
void flecs_unregister_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *o,
    size_t offset)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_id_t term_id = flecs_observer_id(impl->register_id);
    ecs_term_t *term = &o->query->terms[0];
    ecs_entity_t trav = term->trav;

    int i;
    for (i = 0; i < o->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(
            term, o->events[i]);

        /* Get observers for event */
        ecs_event_record_t *er = flecs_event_record_get(observable, event);
        ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get observers for (component) id */
        ecs_event_id_record_t *idt = flecs_event_id_record_get(er, term_id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *id_observers = ECS_OFFSET(idt, offset);
        ecs_map_remove(id_observers, impl->id);
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
    ecs_observer_t *o)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    if (o->query->term_count == 0) {
        return;
    }

    ecs_term_t *term = &o->query->terms[0];
    ecs_flags64_t flags = ECS_TERM_REF_FLAGS(&term->src);

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_unregister_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self_up));
    } else if (flags & EcsSelf) {
        flecs_unregister_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self));
    } else if (flags & EcsUp) {
        flecs_unregister_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, up));
    }
}

static
bool flecs_ignore_observer(
    ecs_observer_t *o,
    ecs_table_t *table,
    ecs_iter_t *it)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    int32_t *last_event_id = impl->last_event_id;
    if (last_event_id && last_event_id[0] == it->event_cur) {
        return true;
    }

    if (impl->flags & (EcsObserverIsDisabled|EcsObserverIsParentDisabled)) {
        return true;
    }

    ecs_flags32_t table_flags = table->flags, query_flags = o->query->flags;

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
    ecs_observer_t *o,
    ecs_iter_action_t callback,
    int32_t term_index,
    bool simple_result) 
{
    ecs_assert(it->callback != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ecs_should_log_3()) {
        char *path = ecs_get_path(world, it->system);
        ecs_dbg_3("observer: invoke %s", path);
        ecs_os_free(path);
    }

    ecs_log_push_3();

    ecs_entity_t old_system = flecs_stage_set_system(
        world->stages[0], o->entity);
    world->info.observers_ran_frame ++;

    ecs_query_t *query = o->query;
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

    flecs_stage_set_system(world->stages[0], old_system);

    ecs_log_pop_3();
}

static
void flecs_default_uni_observer_run_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    it->ctx = o->ctx;
    it->callback = o->callback;

    if (ecs_should_log_3()) {
        char *path = ecs_get_path(it->world, it->system);
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
    ecs_observer_t *o,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav,
    bool simple_result)
{
    ecs_query_t *query = o->query;
    ecs_term_t *term = &query->terms[0];
    if (flecs_ignore_observer(o, table, it)) {
        return;
    }

    ecs_assert(trav == 0 || it->sources[0] != 0, ECS_INTERNAL_ERROR, NULL);
    if (trav && term->trav != trav) {
        return;
    }

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    bool is_filter = term->inout == EcsInOutNone;
    ECS_BIT_COND(it->flags, EcsIterNoData, is_filter);
    it->system = o->entity;
    it->ctx = o->ctx;
    it->callback_ctx = o->callback_ctx;
    it->run_ctx = o->run_ctx;
    it->term_index = impl->term_index;
    it->query = o->query;

    ecs_entity_t event = it->event;
    int32_t event_cur = it->event_cur;
    it->event = flecs_get_observer_event(term, event);

    if (o->run) {
        it->next = flecs_default_next_callback;
        it->callback = flecs_default_uni_observer_run_callback;
        it->ctx = o;
        o->run(it);
    } else {
        ecs_iter_action_t callback = o->callback;
        it->callback = callback;
        flecs_observer_invoke(world, it, o, callback, 0, simple_result);
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
void flecs_multi_observer_invoke(
    ecs_iter_t *it) 
{
    ecs_observer_t *o = it->ctx;
    flecs_poly_assert(o, ecs_observer_t);

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_world_t *world = it->real_world;
    
    if (impl->last_event_id[0] == it->event_cur) {
        /* Already handled this event */
        return;
    }

    impl->last_event_id[0] = world->event_id;

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
            match = ecs_query_has_table(impl->not_query, prev_table, &user_it);

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
        if (impl->flags & EcsObserverIsMonitor) {
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
        user_it.callback_ctx = o->callback_ctx;
        user_it.run_ctx = o->run_ctx;
        user_it.param = it->param;
        user_it.callback = o->callback;
        user_it.system = o->entity;
        user_it.event = it->event;
        user_it.event_id = it->event_id;
        user_it.other_table = it->other_table;

        ecs_entity_t old_system = flecs_stage_set_system(
            world->stages[0], o->entity);
        ecs_table_lock(it->world, table);

        if (o->run) {
            user_it.next = flecs_default_next_callback;
            o->run(&user_it);
        } else {
            user_it.callback(&user_it);
        }

        ecs_iter_fini(&user_it);

        ecs_table_unlock(it->world, table);
        flecs_stage_set_system(world->stages[0], old_system);
        
        return;
    } else {
        /* While the observer query was strictly speaking evaluated, it's more
         * useful to measure how often the observer was actually invoked. */
        o->query->eval_count --;
    }

done:
    return;
}

/* For convenience, so applications can (in theory) use a single run callback 
 * that uses ecs_iter_next to iterate results */
bool flecs_default_next_callback(ecs_iter_t *it) {
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
    ecs_observer_t *o = it->ctx;
    ecs_run_action_t run = o->run;

    if (run) {
        if (flecs_observer_impl(o)->flags & EcsObserverBypassQuery) {
            it->next = flecs_default_next_callback;
            it->callback = flecs_multi_observer_invoke;
            it->interrupted_by = 0;
            it->run_ctx = o->run_ctx;
            run(it);
            return;
        }
    }

    flecs_multi_observer_invoke(it);
}

static
void flecs_observer_yield_existing(
    ecs_world_t *world,
    ecs_observer_t *o)
{
    ecs_run_action_t run = o->run;
    if (!run) {
        run = flecs_multi_observer_invoke;
    }

    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    ecs_defer_begin(world);

    /* If yield existing is enabled, invoke for each thing that matches
     * the event, if the event is iterable. */
    int i, count = o->event_count;
    for (i = 0; i < count; i ++) {
        ecs_iter_t it = ecs_query_iter(world, o->query);
        it.system = o->entity;
        it.ctx = o;
        it.callback = flecs_default_uni_observer_run_callback;
        it.callback_ctx = o->callback_ctx;
        it.run_ctx = o->run_ctx;
        it.event = o->events[i];
        while (ecs_query_next(&it)) {
            it.event_id = it.ids[0];
            it.event_cur = ++ world->event_id;

            ecs_iter_next_action_t next = it.next;
            it.next = flecs_default_next_callback;
            run(&it);
            it.next = next;
            it.interrupted_by = 0;
        }
    }

    ecs_defer_end(world);
}

static
int flecs_uni_observer_init(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_observer_desc_t *desc)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_term_t *term = &o->query->terms[0];
    impl->last_event_id = desc->last_event_id;    
    if (!impl->last_event_id) {
        impl->last_event_id = &impl->last_event_id_storage;
    }
    impl->register_id = term->id;
    term->field_index = flecs_ito(int16_t, desc->term_index_);

    if (ecs_id_is_tag(world, term->id)) {
        /* If id is a tag, downgrade OnSet to OnAdd. */
        int32_t e, count = o->event_count;
        bool has_on_add = false;
        for (e = 0; e < count; e ++) {
            if (o->events[e] == EcsOnAdd) {
                has_on_add = true;
            }
        }

        for (e = 0; e < count; e ++) {
            if (o->events[e] == EcsOnSet) {
                if (has_on_add) {
                    /* Already registered */
                    o->events[e] = 0;
                } else {
                    o->events[e] = EcsOnAdd;
                }
            }
        }
    }

    flecs_uni_observer_register(world, o->observable, o);

    return 0;
}

static
int flecs_observer_add_child(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_observer_desc_t *child_desc)
{
    ecs_observer_t *child_observer = flecs_observer_init(
        world, 0, child_desc);
    if (!child_observer) {
        return -1;
    }

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_vec_append_t(&world->allocator, &impl->children, 
        ecs_observer_t*)[0] = child_observer;
    child_observer->entity = o->entity;
    return 0;
}

static
int flecs_multi_observer_init(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_observer_desc_t *desc)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);

    /* Create last event id for filtering out the same event that arrives from
     * more than one term */
    impl->last_event_id = ecs_os_calloc_t(int32_t);
    
    /* Mark observer as multi observer */
    impl->flags |= EcsObserverIsMulti;

    /* Vector that stores a single-component observer for each query term */
    ecs_vec_init_t(&world->allocator, &impl->children, ecs_observer_t*, 2);

    /* Create a child observer for each term in the query */
    ecs_query_t *query = o->query;
    ecs_observer_desc_t child_desc = *desc;
    child_desc.last_event_id = impl->last_event_id;
    child_desc.run = NULL;
    child_desc.callback = flecs_multi_observer_builtin_run;
    child_desc.ctx = o;
    child_desc.ctx_free = NULL;
    child_desc.query.expr = NULL;
    child_desc.callback_ctx = NULL;
    child_desc.callback_ctx_free = NULL;
    child_desc.run_ctx = NULL;
    child_desc.run_ctx_free = NULL;
    child_desc.yield_existing = false;
    ecs_os_zeromem(&child_desc.entity);
    ecs_os_zeromem(&child_desc.query.terms);
    ecs_os_zeromem(&child_desc.query);
    ecs_os_memcpy_n(child_desc.events, o->events, 
        ecs_entity_t, o->event_count);

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
        child_desc.term_index_ = query->terms[i].field_index;
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
                if (idr->flags & EcsIdOnInstantiateDontInherit) {
                    continue;
                }

                term->first.name = NULL;
                term->first.id = ti_ids[ti];
                term->id = ti_ids[ti];

                if (flecs_observer_add_child(world, o, &child_desc)) {
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
            term->src.id = EcsThis | EcsIsVariable | EcsSelf;
            term->second.id = 0;
        } else if (term->oper == EcsOptional) {
            continue;
        }

        if (flecs_observer_add_child(world, o, &child_desc)) {
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

        ecs_os_memcpy_n(not_desc.terms, o->query->terms, 
            ecs_term_t, term_count); /* cast suppresses warning */
  
        for (i = 0; i < term_count; i ++) {
            if (not_desc.terms[i].oper == EcsNot) {
                not_desc.terms[i].oper = EcsOptional;
            }
        }

        flecs_observer_impl(o)->not_query = 
            ecs_query_init(world, &not_desc);
    }

    return 0; 
error:
    return -1;
}

static
void flecs_observer_poly_fini(void *ptr) {
    flecs_observer_fini(ptr);
}

ecs_observer_t* flecs_observer_init(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_observer_desc_t *desc)
{
    ecs_assert(flecs_poly_is(world, ecs_world_t),
        ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc->callback != NULL || desc->run != NULL, 
        ECS_INVALID_OPERATION,
            "cannot create observer: must at least specify callback or run");

    ecs_observer_impl_t *impl = flecs_sparse_add_t(
        &world->store.observers, ecs_observer_impl_t);
    ecs_assert(impl != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_poly_init(impl, ecs_observer_t);
    ecs_observer_t *o = &impl->pub;
    impl->id = flecs_sparse_last_id(&world->store.observers);
    impl->dtor = flecs_observer_poly_fini;

    /* Make writeable copy of query desc so that we can set name. This will
     * make debugging easier, as any error messages related to creating the
     * query will have the name of the observer. */
    ecs_query_desc_t query_desc = desc->query;
    query_desc.entity = 0;
    query_desc.cache_kind = EcsQueryCacheNone;

    /* Create query */
    ecs_query_t *query = o->query = ecs_query_init(
        world, &query_desc);
    if (query == NULL) {
        flecs_observer_fini(o);
        return 0;
    }

    flecs_poly_assert(query, ecs_query_t);

    ecs_check(o->query->term_count > 0, ECS_INVALID_PARAMETER,
        "observer must have at least one term");

    ecs_observable_t *observable = desc->observable;
    if (!observable) {
        observable = ecs_get_observable(world);
    }

    o->run = desc->run;
    o->callback = desc->callback;
    o->ctx = desc->ctx;
    o->callback_ctx = desc->callback_ctx;
    o->run_ctx = desc->run_ctx;
    o->ctx_free = desc->ctx_free;
    o->callback_ctx_free = desc->callback_ctx_free;
    o->run_ctx_free = desc->run_ctx_free;
    o->observable = observable;
    o->entity = entity;
    impl->term_index = desc->term_index_;
    impl->flags = desc->flags_;

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
            ecs_check(i == 0, ECS_INVALID_PARAMETER,
                "monitor observers can only have a single Monitor event");

            o->events[0] = EcsOnAdd;
            o->events[1] = EcsOnRemove;
            o->event_count ++;
            impl->flags |= EcsObserverIsMonitor;
        } else {
            o->events[i] = event;
        }

        o->event_count ++;
    }

    /* Observer must have at least one event */
    ecs_check(o->event_count != 0, ECS_INVALID_PARAMETER,
        "observer must have at least one event");

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

    bool is_monitor = impl->flags & EcsObserverIsMonitor;
    if (query->term_count == 1 && !is_monitor && !multi) {
        if (flecs_uni_observer_init(world, o, desc)) {
            goto error;
        }
    } else {
        if (flecs_multi_observer_init(world, o, desc)) {
            goto error;
        }
    }

    if (desc->yield_existing) {
        flecs_observer_yield_existing(world, o);
    }

    return o;
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
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_observer_desc_t was not initialized to zero");
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, 
        "cannot create observer while world is being deleted");

    entity = desc->entity;
    if (!entity) {
        entity = ecs_entity(world, {0});
    }

    EcsPoly *poly = flecs_poly_bind(world, entity, ecs_observer_t);
    if (!poly->poly) {
        ecs_observer_t *o = flecs_observer_init(world, entity, desc);
        ecs_assert(o->entity == entity, ECS_INTERNAL_ERROR, NULL);
        poly->poly = o;

        if (ecs_get_name(world, entity)) {
            ecs_trace("#[green]observer#[reset] %s created", 
                ecs_get_name(world, entity));
        }
    } else {
        flecs_poly_assert(poly->poly, ecs_observer_t);
        ecs_observer_t *o = (ecs_observer_t*)poly->poly;

        if (desc->run) {
            o->run = desc->run;
        }

        if (desc->callback) {
            o->callback = desc->callback;
        }

        if (o->ctx_free) {
            if (o->ctx && o->ctx != desc->ctx) {
                o->ctx_free(o->ctx);
            }
        }

        if (o->callback_ctx_free) {
            if (o->callback_ctx && 
                o->callback_ctx != desc->callback_ctx) 
            {
                o->callback_ctx_free(o->callback_ctx);
            }
        }

        if (o->run_ctx_free) {
            if (o->run_ctx && 
                o->run_ctx != desc->run_ctx) 
            {
                o->run_ctx_free(o->run_ctx);
            }
        }

        if (desc->ctx) {
            o->ctx = desc->ctx;
        }

        if (desc->callback_ctx) {
            o->callback_ctx = desc->callback_ctx;
        }

        if (desc->run_ctx) {
            o->run_ctx = desc->run_ctx;
        }

        if (desc->ctx_free) {
            o->ctx_free = desc->ctx_free;
        }

        if (desc->callback_ctx_free) {
            o->callback_ctx_free = desc->callback_ctx_free;
        }

        if (desc->run_ctx_free) {
            o->run_ctx_free = desc->run_ctx_free;
        }
    }

    flecs_poly_modified(world, entity, ecs_observer_t);

    return entity;
error:
    if (entity) {
        ecs_delete(world, entity);
    }
    return 0;
}

const ecs_observer_t* ecs_observer_get(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    return flecs_poly_get(world, observer, ecs_observer_t);
}

void flecs_observer_fini(
    ecs_observer_t *o)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(o->query != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = o->query->world;
    ecs_observer_impl_t *impl = flecs_observer_impl(o);

    if (impl->flags & EcsObserverIsMulti) {
        ecs_observer_t **children = ecs_vec_first(&impl->children);
        int32_t i, children_count = ecs_vec_count(&impl->children);

        for (i = 0; i < children_count; i ++) {
            flecs_observer_fini(children[i]);
        }

        ecs_os_free(impl->last_event_id);
    } else {
        if (o->query->term_count) {
            flecs_unregister_observer(world, o->observable, o);
        } else {
            /* Observer creation failed while creating query */
        }
    }

    ecs_vec_fini_t(&world->allocator, &impl->children, ecs_observer_t*);

    /* Cleanup queries */
    ecs_query_fini(o->query);
    if (impl->not_query) {
        ecs_query_fini(impl->not_query);
    }

    /* Cleanup context */
    if (o->ctx_free) {
        o->ctx_free(o->ctx);
    }

    if (o->callback_ctx_free) {
        o->callback_ctx_free(o->callback_ctx);
    }

    if (o->run_ctx_free) {
        o->run_ctx_free(o->run_ctx);
    }

    flecs_poly_fini(o, ecs_observer_t);
    flecs_sparse_remove_t(
        &world->store.observers, ecs_observer_impl_t, impl->id);
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
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    if (impl->flags & EcsObserverIsMulti) {
        ecs_observer_t **children = ecs_vec_first(&impl->children);
        int32_t i, children_count = ecs_vec_count(&impl->children);
        if (children_count) {
            for (i = 0; i < children_count; i ++) {
                ECS_BIT_COND(flecs_observer_impl(children[i])->flags, bit, cond);
            }
        }
    } else {
        flecs_poly_assert(o, ecs_observer_t);
        ECS_BIT_COND(impl->flags, bit, cond);
    }
}
