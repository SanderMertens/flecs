/**
 * @file observer.c
 * @brief Observer lifecycle and invocation.
 *
 * Single-term observers register directly for an event/id pair.
 * Multi-term observers create one single-term child observer per query term;
 * when any child fires, the parent's full query is evaluated before invoking
 * the user callback.
 */

#include "private_api.h"

/* Get the effective event for an observer term. Not operators invert
 * Add/Set to Remove and vice versa. */
static
ecs_entity_t flecs_get_observer_event(
    ecs_term_t *term,
    ecs_entity_t event)
{
    if (term && term->oper == EcsNot) {
        if (event == EcsOnAdd || event == EcsOnSet) {
            event = EcsOnRemove;
        } else if (event == EcsOnRemove) {
            event = EcsOnAdd;
        }
    }

    return event;
}

/* Return the component record flag(s) corresponding to an event type. */
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
    if (e == EcsOnTableCreate) {
        return EcsIdHasOnTableCreate;
    }
    if (e == EcsOnTableDelete) {
        return EcsIdHasOnTableDelete;
    }
    if (e == EcsWildcard) {
        return EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|
            EcsIdHasOnTableCreate|EcsIdHasOnTableDelete;
    }
    return 0;
}

/* Adjust observer count for an event/id pair by the given value (+1 or -1).
 * Transitions to 1 notify tables to start evaluating; transitions to 0
 * notify tables to stop and clean up the event id record. */
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
        /* First observer for this event/id: enable event evaluation */
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableTriggersForId,
            .event = event
        });

        ecs_flags32_t flags = flecs_id_flag_for_event(event);
        if (flags) {
            ecs_component_record_t *cr = flecs_components_get(world, id);
            if (cr) {
                cr->flags |= flags;
            }

            /* Mark component as having OnSet observers so set operations
             * don't skip event emission. */
            if (event == EcsOnSet || event == EcsWildcard) {
                if (id < FLECS_HI_COMPONENT_ID) {
                    world->non_trivial_set[id] = true;
                }
                
                if (id == EcsWildcard || id == EcsAny) {
                    ecs_os_memset_n(world->non_trivial_set, true, bool, 
                        FLECS_HI_COMPONENT_ID);
                }
            }
        }
    } else if (result == 0) {
        /* Last observer removed: disable event evaluation */
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableNoTriggersForId,
            .event = event
        });

        ecs_flags32_t flags = flecs_id_flag_for_event(event);
        if (flags) {
            ecs_component_record_t *cr = flecs_components_get(world, id);
            if (cr) {
                cr->flags &= ~flags;
            }
        }

        flecs_event_id_record_remove(evt, id);
        ecs_os_free(idt);
    }

    if (ECS_PAIR_FIRST(id) == EcsChildOf) {
        ecs_observable_t *observable = &world->observable;
        if (event == EcsOnAdd) {
            ecs_event_record_t *er_onset = 
                flecs_event_record_ensure(observable, EcsOnSet);
            flecs_inc_observer_count(
                world, EcsOnSet, er_onset, ecs_id(EcsParent), value);
        } else {
            flecs_inc_observer_count(
                world, event, evt, ecs_id(EcsParent), value);
        }
    }
}

/* Normalize observer id by replacing Any with Wildcard in pairs. */
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

/* Register an observer for a specific event and component id combination. */
static
void flecs_register_observer_for_event_and_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *o,
    size_t offset,
    const ecs_term_t *term,
    ecs_entity_t event,
    ecs_id_t term_id)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_entity_t trav = term ? term->trav : 0;

    ecs_event_record_t *er = flecs_event_record_ensure(observable, event);
    ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_event_id_record_t *idt = flecs_event_id_record_ensure(
        world, er, term_id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *observers = ECS_OFFSET(idt, offset);
    ecs_map_init_if(observers, &world->allocator);
    ecs_map_insert_ptr(observers, impl->id, o);

    flecs_inc_observer_count(world, event, er, term_id, 1);
    if (trav && (term_id != ecs_id(EcsParent))) {
        flecs_inc_observer_count(world, event, er, 
            ecs_pair(trav, EcsWildcard), 1);
    }
}

/* Register an observer for a component id across all its events. */
static
void flecs_register_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *o,
    size_t offset,
    ecs_id_t term_id)
{
    ecs_term_t *term = o->query ? &o->query->terms[0] : NULL;

    int i, j;
    for (i = 0; i < o->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(
            term, o->events[i]);

        for (j = 0; j < i; j ++) {
            if (event == flecs_get_observer_event(term, o->events[j])) {
                break;
            }
        }
        if (i != j) {
            /* Duplicate event, ignore */
            continue;
        }

        flecs_register_observer_for_event_and_id(
            world, observable, o, offset, term, event, term_id);
    }
}

/* Register a single-term observer in the Self, Self|Up, or Up observer map. */
static
void flecs_uni_observer_register(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *o)
{
    ecs_term_t *term = o->query ? &o->query->terms[0] : NULL;
    ecs_flags64_t flags = term ? ECS_TERM_REF_FLAGS(&term->src) : EcsSelf;

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_id_t term_id = flecs_observer_id(impl->register_id);

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_register_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self_up), term_id);
    } else if (flags & EcsSelf) {
        flecs_register_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self), term_id);
    } else if (flags & EcsUp) {
        ecs_assert(term->trav != 0, ECS_INTERNAL_ERROR, NULL);
        flecs_register_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, up), term_id);
    }

    if (term && (term->trav == EcsChildOf)) {
        ecs_assert(o->query != NULL, ECS_INTERNAL_ERROR, NULL);
        if (o->query->flags & EcsQueryTableOnly) {
            flecs_register_observer_for_id(world, observable, o,
                offsetof(ecs_event_id_record_t, self), ecs_id(EcsParent));
        }
    }
}

/* Unregister an observer for a component id across all its events. */
static
void flecs_unregister_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *o,
    size_t offset,
    ecs_id_t term_id)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_term_t *term = o->query ? &o->query->terms[0] : NULL;
    ecs_entity_t trav = term ? term->trav : 0;

    int i, j;
    for (i = 0; i < o->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(
            term, o->events[i]);
        for (j = 0; j < i; j ++) {
            if (event == flecs_get_observer_event(term, o->events[j])) {
                break;
            }
        }
        if (i != j) {
            /* Duplicate event, ignore */
            continue;
        }

        ecs_event_record_t *er = flecs_event_record_get(observable, event);
        ecs_assert(er != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_event_id_record_t *idt = flecs_event_id_record_get(er, term_id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *id_observers = ECS_OFFSET(idt, offset);
        ecs_map_remove(id_observers, impl->id);
        if (!ecs_map_count(id_observers)) {
            ecs_map_fini(id_observers);
        }

        flecs_inc_observer_count(world, event, er, term_id, -1);
        if (trav && (term_id != ecs_id(EcsParent))) {
            flecs_inc_observer_count(world, event, er, 
                ecs_pair(trav, EcsWildcard), -1);
        }
    }
}

/* Unregister a single-term observer from the observable. */
static
void flecs_unregister_observer(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *o)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_query_t *q = o->query;
    if (q && q->term_count == 0) {
        return;
    }

    ecs_term_t *term = q ? &q->terms[0] : NULL;
    ecs_flags64_t flags = term ? ECS_TERM_REF_FLAGS(&term->src) : EcsSelf;

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_id_t term_id = flecs_observer_id(impl->register_id);

    if ((flags & (EcsSelf|EcsUp)) == (EcsSelf|EcsUp)) {
        flecs_unregister_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self_up), term_id);
    } else if (flags & EcsSelf) {
        flecs_unregister_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, self), term_id);
    } else if (flags & EcsUp) {
        flecs_unregister_observer_for_id(world, observable, o,
            offsetof(ecs_event_id_record_t, up), term_id);
    }

    if (term && (term->trav == EcsChildOf)) {
        if (o->query->flags & EcsQueryTableOnly) {
            flecs_unregister_observer_for_id(world, observable, o,
                offsetof(ecs_event_id_record_t, self), ecs_id(EcsParent));
        }
    }
}

/* Check if an observer should be skipped: already handled this event (dedup),
 * observer is disabled, or table is a prefab/disabled entity table that the
 * observer did not opt into matching. */
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

    ecs_flags32_t table_flags = table->flags, query_flags = impl->flags;

    bool result = (table_flags & EcsTableIsPrefab) &&
        !(query_flags & EcsQueryMatchPrefab);
    result = result || ((table_flags & EcsTableIsDisabled) &&
        !(query_flags & EcsQueryMatchDisabled));

    return result;
}

/* Default run callback for single-term observers; delegates to user callback. */
static
void flecs_default_uni_observer_run_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    it->ctx = o->ctx;
    it->callback = o->callback;
    o->callback(it);
}

/* Invoke an observer's run or callback function. */
static
void flecs_observer_invoke(
    ecs_observer_t *o,
    ecs_iter_t *it)
{
    if (o->run) {
        it->next = flecs_default_next_callback;
        it->callback = o->callback;
        it->interrupted_by = 0;
        if (flecs_observer_impl(o)->flags & EcsObserverBypassQuery) {
            it->ctx = o;
        } else {
            it->ctx = o->ctx;
        }
        o->run(it);
    } else {
        ecs_iter_action_t callback = o->callback;
        it->callback = callback;
        callback(it);
    }
}

/* Invoke a single-term observer for a matched table event. */
static
void flecs_uni_observer_invoke(
    ecs_world_t *world,
    ecs_observer_t *o,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    if (flecs_ignore_observer(o, table, it)) {
        return;
    }

    if (ecs_should_log_3()) {
        char *path = ecs_get_path(world, it->system);
        ecs_dbg_3("observer: invoke %s", path);
        ecs_os_free(path);
    }

    ecs_log_push_3();

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    it->system = o->entity;
    it->ctx = o->ctx;
    it->callback_ctx = o->callback_ctx;
    it->run_ctx = o->run_ctx;
    it->term_index = impl->term_index;

    ecs_entity_t event = it->event;
    int32_t event_cur = it->event_cur;
    ecs_entity_t old_system = flecs_stage_set_system(
        world->stages[0], o->entity);
    ecs_flags32_t set_fields_cur = it->set_fields;
    it->set_fields = 1;

    ecs_query_t *query = o->query;
    it->query = query;

    if (!query) {
        /* Invoke trivial observer */
        it->event = event;
        flecs_observer_invoke(o, it);
    } else {
        ecs_term_t *term = &query->terms[0];
        ecs_assert(trav == 0 || it->sources[0] != 0, ECS_INTERNAL_ERROR, NULL);
        if (trav && term->trav != trav) {
            return;
        }

        bool is_filter = term->inout == EcsInOutNone;
        ECS_BIT_COND(it->flags, EcsIterNoData, is_filter);
        it->ref_fields = query->fixed_fields | query->row_fields;
        ecs_termset_t row_fields = it->row_fields;
        it->row_fields = query->row_fields;
        it->event = flecs_get_observer_event(term, event);

        bool match_this = query->flags & EcsQueryMatchThis;

        if (match_this) {
            flecs_observer_invoke(o, it);
            ecs_os_inc(&query->eval_count);
        } else {
            /* Non-$this source: translate event data to match the observer's
             * fixed source entity. */
            ecs_entity_t observer_src = ECS_TERM_REF_ID(&term->src);
            ecs_assert(observer_src != 0, ECS_INTERNAL_ERROR, NULL);
            const ecs_entity_t *entities = it->entities;
            int32_t i, count = it->count;
            ecs_entity_t src = it->sources[0];
            ecs_table_t *old_table = it->table;

            it->entities = NULL;
            it->count = 0;
            it->table = NULL;

            /* Check each entity in the event range for the observer's source */
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];

                if (observer_src == e) {
                    if (!src) {
                        /* Set source unless event was forwarded/propagated */
                        it->sources[0] = e;
                    }

                    flecs_observer_invoke(o, it);
                    ecs_os_inc(&query->eval_count);
                    it->sources[0] = src;

                    /* Fixed source matches at most once */
                    break;
                }
            }

            it->entities = entities;
            it->count = count;
            it->table = old_table;
        }

        it->row_fields = row_fields;
    }

    flecs_stage_set_system(world->stages[0], old_system);

    it->event = event;
    it->event_cur = event_cur;
    it->set_fields = set_fields_cur;

    ecs_log_pop_3();

    world->info.observers_ran_total ++;
}

/* Invoke all observers in a map for a given event. */
void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    if (ecs_map_is_init(observers)) {
        ecs_table_lock(it->world, table);

        ecs_map_iter_t oit = ecs_map_iter(observers);
        while (ecs_map_next(&oit)) {
            ecs_observer_t *o = ecs_map_ptr(&oit);
            ecs_assert(it->table == table, ECS_INTERNAL_ERROR, NULL);
            flecs_uni_observer_invoke(world, o, it, table, trav);

            ecs_assert(ecs_map_iter_valid(&oit), ECS_INVALID_OPERATION, 
                "observer list modified while notifying: "
                "cannot create observer from observer");
        }

        ecs_table_unlock(it->world, table);
    }
}

/* Invoke a multi-term observer: evaluate its full query against the event
 * table, and only fire the callback if all terms match. */
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

    ecs_table_t *table = it->table;
    ecs_table_t *prev_table = it->other_table;
    int8_t pivot_term = it->term_index;
    ecs_term_t *term = &o->query->terms[pivot_term];

    /* For Not terms, swap tables: evaluate the query against the destination
     * table (other_table), with the current table as previous. */
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
            /* Target table matches but entity hasn't moved yet. Use
             * not_query to populate data from the entity's current table. */
            user_it.flags |= EcsIterSkip;
            ecs_iter_fini(&user_it);
            match = ecs_query_has_table(impl->not_query, prev_table, &user_it);

            /* not_query replaces Not with Optional, so it must also match. */
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
        /* Monitors only fire on enter transitions, not when already matching. */
        if (impl->flags & EcsObserverIsMonitor) {
            ecs_iter_t table_it;
            if (ecs_query_has_table(o->query, prev_table, &table_it)) {
                /* Prevent change detection on fini */
                user_it.flags |= EcsIterSkip;
                table_it.flags |= EcsIterSkip;
                ecs_iter_fini(&table_it);
                ecs_iter_fini(&user_it);
                goto done;
            }
        }

        impl->last_event_id[0] = it->event_cur;

        /* Patch the pivot field with the actual event id. The query match may
         * have resolved wildcards differently than the triggering event. */
        int8_t pivot_field = term->field_index;
        ecs_assert(pivot_field >= 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(pivot_field < user_it.field_count, ECS_INTERNAL_ERROR, NULL);
        user_it.ids[pivot_field] = it->event_id;
        user_it.trs[pivot_field] = it->trs[0];
        user_it.sources[pivot_field] = it->sources[0];
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

        user_it.flags |= EcsIterSkip; /* Prevent change detection on fini */
        ecs_iter_fini(&user_it);

        ecs_table_unlock(it->world, table);
        flecs_stage_set_system(world->stages[0], old_system);
    } else {
        /* No match: undo eval_count so metrics reflect actual invocations. */
        o->query->eval_count --;
    }

done:
    return;
}

/* Invoke a multi-term observer without query evaluation. */
static
void flecs_multi_observer_invoke_no_query(
    ecs_iter_t *it) 
{
    ecs_observer_t *o = it->ctx;
    flecs_poly_assert(o, ecs_observer_t);

    ecs_world_t *world = it->real_world;
    ecs_table_t *table = it->table;
    ecs_iter_t user_it = *it;

    user_it.ctx = o->ctx;
    user_it.callback_ctx = o->callback_ctx;
    user_it.run_ctx = o->run_ctx;
    user_it.param = it->param;
    user_it.callback = o->callback;
    user_it.system = o->entity;
    user_it.event = it->event;

    ecs_entity_t old_system = flecs_stage_set_system(
        world->stages[0], o->entity);
    ecs_table_lock(it->world, table);

    if (o->run) {
        user_it.next = flecs_default_next_callback;
        o->run(&user_it);
    } else {
        user_it.callback(&user_it);
    }

    ecs_table_unlock(it->world, table);
    flecs_stage_set_system(world->stages[0], old_system);
}

/* One-shot next callback for observer run functions. Returns true once,
 * then false on subsequent calls. Uses interrupted_by as the sentinel. */
bool flecs_default_next_callback(ecs_iter_t *it) {
    if (it->interrupted_by) {
        return false;
    } else {
        ecs_assert(it->system != 0, ECS_INTERNAL_ERROR, NULL);
        it->interrupted_by = it->system;
        return true;
    }
}

/* Callback assigned to child observers of a multi-term observer. Delegates
 * to the user's run callback if set, otherwise evaluates the parent query. */
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

/* Invoke an observer for all existing entities that match its query. */
static
void flecs_observer_yield_existing(
    ecs_world_t *world,
    ecs_observer_t *o,
    bool yield_on_remove)
{
    ecs_run_action_t run = o->run;
    if (!run) {
        run = flecs_multi_observer_invoke_no_query;
    }

    ecs_defer_begin(world);

    /* Iterate existing matches and invoke the observer for each. */
    int i, count = o->event_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t event = o->events[i];

        /* We only yield for OnRemove events if the observer is deleted. */
        if (event == EcsOnRemove) {
            if (!yield_on_remove) {
                continue;
            }
        } else {
            if (yield_on_remove) {
                continue;
            }
        }

        ecs_iter_t it;
        if (o->query) {
            it = ecs_query_iter(world, o->query);
        } else {
            it = ecs_each_id(world, flecs_observer_impl(o)->register_id);
        }

        it.system = o->entity;
        it.ctx = o;
        it.callback = flecs_default_uni_observer_run_callback;
        it.callback_ctx = o->callback_ctx;
        it.run_ctx = o->run_ctx;
        it.event = o->events[i];
        while (o->query ? ecs_query_next(&it) : ecs_each_next(&it)) {
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

/* Initialize a single-term observer and register it with the observable. */
static
int flecs_uni_observer_init(
    ecs_world_t *world,
    ecs_observer_t *o,
    ecs_id_t component_id,
    const ecs_observer_desc_t *desc)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    impl->last_event_id = desc->last_event_id;    
    if (!impl->last_event_id) {
        impl->last_event_id = &impl->last_event_id_storage;
    }

    impl->register_id = component_id;

    if (ecs_id_is_tag(world, component_id)) {
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

/* Add a child single-term observer to a multi-term observer. */
static
int flecs_observer_add_child(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_observer_desc_t *child_desc)
{
    ecs_assert(child_desc->query.flags & EcsQueryNested, 
        ECS_INTERNAL_ERROR, NULL);

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

/* Initialize a multi-term observer by creating child observers for each query term. */
static
int flecs_multi_observer_init(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_observer_desc_t *desc)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);

    /* Shared event id deduplicates: when multiple child observers trigger
     * for the same event, the parent only fires once. */
    impl->last_event_id = ecs_os_calloc_t(int32_t);
    impl->flags |= EcsObserverIsMulti;

    /* Each query term gets its own single-term child observer */
    ecs_vec_init_t(&world->allocator, &impl->children, ecs_observer_t*, 2);
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
    child_desc.flags_ &= ~(EcsObserverYieldOnCreate|EcsObserverYieldOnDelete);
    ecs_os_zeromem(&child_desc.entity);
    ecs_os_zeromem(&child_desc.query.terms);
    ecs_os_zeromem(&child_desc.query);
    ecs_os_memcpy_n(child_desc.events, o->events, ecs_entity_t, o->event_count);

    child_desc.query.flags |= EcsQueryNested;

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

    /* Table events fire for all components at once; one child suffices. */
    bool only_table_events = true;
    for (i = 0; i < o->event_count; i ++) {
        ecs_entity_t e = o->events[i];
        if (e != EcsOnTableCreate && e != EcsOnTableDelete) {
            only_table_events = false;
            break;
        }
    }

    if (query->flags & EcsQueryMatchPrefab) {
        child_desc.query.flags |= EcsQueryMatchPrefab;
    }

    if (query->flags & EcsQueryMatchDisabled) {
        child_desc.query.flags |= EcsQueryMatchDisabled;
    }

    bool self_term_handled = false;
    for (i = 0; i < term_count; i ++) {
        if (query->terms[i].inout == EcsInOutFilter) {
            continue;
        }

        ecs_term_t *term = &child_desc.query.terms[0];
        child_desc.term_index_ = query->terms[i].field_index;
        *term = query->terms[i];

        /* Don't create observers for non-$this terms */
        if (!ecs_term_match_this(term) && term->src.id & EcsIsVariable) {
            continue;
        }

        int16_t oper = term->oper;
        ecs_id_t id = term->id;

        if (only_table_events) {
            /* Table events: only one $this|self term needed. */
            if (ecs_term_match_this(term) && 
               (term->src.id & EcsTraverseFlags) == EcsSelf) 
            {
                if (oper == EcsAnd) {
                    if (!self_term_handled) {
                        self_term_handled = true;
                    } else {
                        continue;
                    }
                }
            }
        }

        /* AndFrom/OrFrom: expand into one child observer per id in the type */
        if (oper == EcsAndFrom || oper == EcsOrFrom) {
            const ecs_type_t *type = ecs_get_type(world, id);
            if (!type) {
                continue;
            }

            int32_t ti, ti_count = type->count;
            ecs_id_t *ti_ids = type->array;

            /* Children use And; full semantics applied at query evaluation. */
            term->oper = EcsAnd;
            for (ti = 0; ti < ti_count; ti ++) {
                ecs_id_t ti_id = ti_ids[ti];
                ecs_component_record_t *cr = flecs_components_get(world, ti_id);
                if (cr->flags & EcsIdOnInstantiateDontInherit) {
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

        /* Child observers use And; Or semantics are handled by the parent */
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
            if (only_table_events || desc->events[0] == EcsMonitor) {
                /* For table events & monitors optional terms aren't necessary */
                continue;
            }
        }

        if (flecs_observer_add_child(world, o, &child_desc)) {
            goto error;
        }

        if (optional_only) {
            break;
        }
    }

    /* For Not terms, create a parallel query with Not replaced by Optional
     * so iterator data can be populated from the entity's pre-move table. */
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

/* Destructor callback for observer poly type. */
static
void flecs_observer_poly_fini(void *ptr) {
    flecs_observer_fini(ptr);
}

/* Initialize an observer from a descriptor, creating query and child observers as needed. */
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

    ecs_observer_impl_t *impl = flecs_calloc_t(
        &world->allocator, ecs_observer_impl_t);
    ecs_assert(impl != NULL, ECS_INTERNAL_ERROR, NULL);
    impl->id = ++ world->observable.last_observer_id;

    flecs_poly_init(impl, ecs_observer_t);
    ecs_observer_t *o = &impl->pub;
    o->world = world;
    impl->dtor = flecs_observer_poly_fini;

    ecs_query_desc_t query_desc = desc->query;
    query_desc.entity = 0;
    query_desc.cache_kind = EcsQueryCacheNone;

    ecs_query_t *query = NULL;
    
    /* Skip trivial observer optimization in sanitized mode to exercise
     * both query and non-query code paths in tests. */
#ifndef FLECS_SANITIZE
    /* Temporary arrays for placeholder query */
    ecs_term_t terms[FLECS_TERM_COUNT_MAX] = {0};
    ecs_size_t sizes[FLECS_TERM_COUNT_MAX] = {0};
    ecs_id_t ids[FLECS_TERM_COUNT_MAX] = {0};

    ecs_query_t dummy_query = {
        .terms = terms,
        .sizes = sizes,
        .ids = ids
    };

    if (desc->events[0] != EcsMonitor) {
        if (flecs_query_finalize_simple(world, &dummy_query, &query_desc)) {
            /* flecs_query_finalize_simple increments component keep_alive
             * counts. If we take the trivial path below, no real query is
             * created. Otherwise, the keep_alive must be undone. */
            bool trivial_observer = (dummy_query.term_count == 1) &&
                (dummy_query.flags & EcsQueryIsTrivial) &&
                (dummy_query.flags & EcsQueryMatchOnlySelf) &&
                !dummy_query.row_fields;
            if (trivial_observer) {
                if (ECS_PAIR_FIRST(dummy_query.terms[0].id) != EcsChildOf) {
                    dummy_query.flags |= desc->query.flags;
                    query = &dummy_query;
                }
            } else {
                /* Creating a real query, so undo the keep_alive increment
                 * from the placeholder query. */
                int32_t i, count = dummy_query.term_count;
                for (i = 0; i < count; i ++) {
                    ecs_term_t *term = &terms[i];
                    flecs_component_unlock(world, term->id);
                }
            }
        }
    }
#endif

    /* Create query */
    if (!query) {
        query = o->query = ecs_query_init(
            world, &query_desc);
        if (query == NULL) {
            flecs_observer_fini(o);
            return 0;
        }

        flecs_poly_assert(query, ecs_query_t);
    }

    ecs_check(query->term_count > 0, ECS_INVALID_PARAMETER,
        "observer must have at least one term");

    int i, var_count = 0;
    for (i = 0; i < query->term_count; i ++) {
        ecs_term_t *term = &query->terms[i];
        if (!ecs_term_match_this(term)) {
            if (term->src.id & EcsIsVariable) {
                /* Term has a non-$this variable source */
                var_count ++;
            }
        }
    }

    ecs_check(query->term_count > var_count, ECS_UNSUPPORTED,
        "observers with only non-$this variable sources are not yet supported");
    (void)var_count;

    o->run = desc->run;
    o->callback = desc->callback;
    o->ctx = desc->ctx;
    o->callback_ctx = desc->callback_ctx;
    o->run_ctx = desc->run_ctx;
    o->ctx_free = desc->ctx_free;
    o->callback_ctx_free = desc->callback_ctx_free;
    o->run_ctx_free = desc->run_ctx_free;
    o->observable = flecs_get_observable(world);
    o->entity = entity;
    o->world = world;
    impl->term_index = desc->term_index_;
    impl->flags |= desc->flags_ | 
        (query->flags & (EcsQueryMatchPrefab|EcsQueryMatchDisabled));

    ecs_check(!(desc->yield_existing && 
        (desc->flags_ & (EcsObserverYieldOnCreate|EcsObserverYieldOnDelete))), 
        ECS_INVALID_PARAMETER,
         "cannot set yield_existing and YieldOn* flags at the same time");

    /* Monitors need multi-observer infrastructure for enter/leave detection. */
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
            if (desc->yield_existing) {
                impl->flags |= EcsObserverYieldOnCreate;
                impl->flags |= EcsObserverYieldOnDelete;
            }
        } else {
            o->events[i] = event;
            if (desc->yield_existing) {
                if (event == EcsOnRemove) {
                    impl->flags |= EcsObserverYieldOnDelete;
                } else {
                    impl->flags |= EcsObserverYieldOnCreate;
                }
            }
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
        ecs_term_t *term = &query->terms[0];
        term->field_index = flecs_ito(int8_t, desc->term_index_);
        if (flecs_uni_observer_init(world, o, term->id, desc)) {
            goto error;
        }
    } else {
        if (flecs_multi_observer_init(world, o, desc)) {
            goto error;
        }
    }

    if (impl->flags & EcsObserverYieldOnCreate) {
        flecs_observer_yield_existing(world, o, false);
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
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_observer_desc_t was not initialized to zero");
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, 
        "cannot create observer while world is being deleted");

    entity = desc->entity;
    if (!entity && !desc->global_observer) {
        entity = ecs_entity(world, {0});
    }

    EcsPoly *poly = NULL;
    if (!entity) {
        ecs_observer_t *o = flecs_observer_init(world, entity, desc);\
        if (!o) {
            goto error;
        }

        ecs_vec_append_t(NULL, &world->observable.global_observers, 
            ecs_observer_t*)[0] = o;
    } else {
        poly = flecs_poly_bind(world, entity, ecs_observer_t);

        if (!poly->poly) {
            ecs_observer_t *o = flecs_observer_init(world, entity, desc);\
            if (!o) {
                goto error;
            }

            ecs_assert(o->entity == entity, ECS_INTERNAL_ERROR, NULL);
            poly->poly = o;

            if (ecs_get_name(world, entity)) {
                ecs_trace("#[green]observer#[reset] %s created", 
                    ecs_get_name(world, entity));
            }
        } else {
            flecs_poly_assert(poly->poly, ecs_observer_t);
            ecs_observer_t *o = (ecs_observer_t*)poly->poly;

            if (o->ctx_free) {
                if (o->ctx && o->ctx != desc->ctx) {
                    o->ctx_free(o->ctx);
                }
            }

            if (o->callback_ctx_free) {
                if (o->callback_ctx && o->callback_ctx != desc->callback_ctx) {
                    o->callback_ctx_free(o->callback_ctx);
                    o->callback_ctx_free = NULL;
                    o->callback_ctx = NULL;
                }
            }

            if (o->run_ctx_free) {
                if (o->run_ctx && o->run_ctx != desc->run_ctx) {
                    o->run_ctx_free(o->run_ctx);
                    o->run_ctx_free = NULL;
                    o->run_ctx = NULL;
                }
            }

            if (desc->run) {
                o->run = desc->run;
                if (!desc->callback) {
                    o->callback = NULL;
                }
            }

            if (desc->callback) {
                o->callback = desc->callback;
                if (!desc->run) {
                    o->run = NULL;
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
    }

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

/* Clean up an observer, unregistering it and freeing all resources. */
void flecs_observer_fini(
    ecs_observer_t *o)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = o->world;

    flecs_poly_assert(world, ecs_world_t);
    ecs_observer_impl_t *impl = flecs_observer_impl(o);

    if (impl->flags & EcsObserverYieldOnDelete) {
        flecs_observer_yield_existing(world, o, true);
    }

    if (impl->flags & EcsObserverIsMulti) {
        ecs_observer_t **children = ecs_vec_first(&impl->children);
        int32_t i, children_count = ecs_vec_count(&impl->children);

        for (i = 0; i < children_count; i ++) {
            flecs_observer_fini(children[i]);
        }

        ecs_os_free(impl->last_event_id);
    } else {
        flecs_unregister_observer(world, o->observable, o);
    }

    ecs_vec_fini_t(&world->allocator, &impl->children, ecs_observer_t*);

    /* Cleanup queries */
    if (o->query) {
        ecs_query_fini(o->query);
    } else if (impl->register_id) {
        flecs_component_unlock(world, impl->register_id);
    }

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
    flecs_free_t(&world->allocator, ecs_observer_impl_t, o);
}

/* Set or clear a disable bit on an observer and its children. */
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
