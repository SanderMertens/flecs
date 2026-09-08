/**
 * @file observer.c
 * @brief Observer implementation.
 */

#include "private_api.h"

static void flecs_multi_observer_builtin_run(ecs_iter_t *it);

static ecs_entity_t flecs_get_observer_event(
    const ecs_observer_subscription_t *sub,
    ecs_entity_t event)
{
    if (sub->tag && event == EcsOnSet) {
        event = EcsOnAdd;
    }
    if (sub->oper == EcsNot) {
        if (event == EcsOnAdd || event == EcsOnSet) {
            event = EcsOnRemove;
        } else if (event == EcsOnRemove) {
            event = EcsOnAdd;
        }
    }
    return event;
}

static ecs_flags32_t flecs_id_flag_for_event(
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

static void flecs_observer_set_table_flags(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_flags32_t flags)
{
    table->flags |= flags;
    if (id && (flags == EcsTableHasOnAdd || flags == EcsTableHasOnRemove ||
        flags == EcsTableHasUpNotify))
    {
        flecs_table_edges_add_flags(world, table, id, flags);
    }
}

static void flecs_observer_set_tables_flags(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_flags32_t flags)
{
    if (!flags || (world->flags & EcsWorldFini)) {
        return;
    }

    if (!id || id == EcsAny) {
        ecs_sparse_t *tables = &world->store.tables;
        int32_t count = flecs_sparse_count(tables);
        for (int32_t i = 0; i < count; i ++) {
            flecs_observer_set_table_flags(world,
                flecs_sparse_get_dense_t(tables, ecs_table_t, i), id, flags);
        }
    } else {
        ecs_component_record_t *cr = flecs_components_get(world, id);
        if (cr) {
            ecs_table_cache_iter_t it;
            flecs_table_cache_iter(&cr->cache, &it, EcsTableEmpty|EcsTableNotEmpty);
            const ecs_table_cache_elem_t *elem;
            while ((elem = flecs_table_cache_next(&it))) {
                flecs_observer_set_table_flags(world, elem->table, id, flags);
            }
        }
    }
}

static void flecs_inc_observer_count(
    ecs_world_t *world,
    ecs_entity_t event,
    ecs_event_record_t *evt,
    ecs_id_t id,
    int32_t value,
    bool up_notify)
{
    ecs_event_id_record_t *idt = flecs_event_id_record_ensure(world, evt, id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t result = idt->observer_count += value;
    int32_t category_result;
    if (up_notify) {
        category_result = idt->up_notify_count += value;
    } else {
        category_result = result - idt->up_notify_count;
    }

    ecs_flags32_t flags = up_notify
        ? EcsIdHasUpNotify : flecs_id_flag_for_event(event);
    if ((category_result == value && value > 0) ||
        (category_result == 0 && value < 0))
    {
        ecs_component_record_t *cr = flecs_components_get(world, id);
        if (value > 0) {
            flecs_observer_set_tables_flags(world, id, flags);
            if (cr) {
                cr->flags |= flags;
            }
            if (!up_notify && (event == EcsOnSet || event == EcsWildcard)) {
                if (id < FLECS_HI_COMPONENT_ID) {
                    world->non_trivial_set[id] = true;
                }
                if (id == EcsWildcard || id == EcsAny) {
                    ecs_os_memset_n(world->non_trivial_set, true, bool,
                        FLECS_HI_COMPONENT_ID);
                }
            }
        } else if (cr) {
            cr->flags &= ~flags;
        }
    }

    if (result == 0) {
        flecs_event_id_record_remove(evt, id);
        ecs_os_free(idt);
    }

    if (ECS_PAIR_FIRST(id) == EcsChildOf) {
        ecs_observable_t *observable = &world->observable;
        if (event == EcsOnAdd) {
            ecs_event_record_t *er_onset =
                flecs_event_record_ensure(observable, EcsOnSet);
            flecs_inc_observer_count(
                world, EcsOnSet, er_onset, ecs_id(EcsParent), value, false);
        } else {
            flecs_inc_observer_count(
                world, event, evt, ecs_id(EcsParent), value, up_notify);
        }
    }
}

static ecs_id_t flecs_observer_id(
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

static void flecs_observer_update_id(
    ecs_world_t *world,
    ecs_observer_subscription_t *sub,
    size_t offset,
    ecs_id_t id,
    int32_t delta)
{
    ecs_observer_t *o = sub->observer;
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_entity_t trav = sub->trav;
    bool up_notify = (impl->flags & EcsObserverIsUpNotify) != 0;

    for (int32_t i = 0; i < o->event_count; i ++) {
        ecs_entity_t event = flecs_get_observer_event(sub, o->events[i]);
        int32_t j;
        for (j = 0; j < i; j ++) {
            if (event == flecs_get_observer_event(sub, o->events[j])) {
                break;
            }
        }
        if (i != j) {
            continue;
        }

        ecs_event_record_t *er = flecs_event_record_ensure(o->observable, event);
        ecs_event_id_record_t *idt = flecs_event_id_record_ensure(world, er, id);
        ecs_map_t *observers = ECS_OFFSET(idt, offset);
        if (delta > 0) {
            ecs_map_init_if(observers, &world->allocator);
            ecs_map_insert_ptr(observers, sub->id, sub);
        } else {
            ecs_map_remove(observers, sub->id);
            if (!ecs_map_count(observers)) {
                ecs_map_fini(observers);
            }
        }

        flecs_inc_observer_count(world, event, er, id, delta, up_notify);
        if (trav && id != ecs_id(EcsParent)) {
            flecs_inc_observer_count(world, event, er,
                ecs_pair(trav, EcsWildcard), delta, up_notify);
        }
    }
}

static void flecs_observer_update_registration(
    ecs_world_t *world,
    ecs_observer_subscription_t *sub,
    int32_t delta)
{
    if (delta > 0) {
        sub->id = ++ world->observable.last_observer_id;
        flecs_component_lock(world, sub->register_id);
    } else if (!sub->id) {
        return;
    }
    ecs_flags64_t flags = sub->src & EcsTermRefFlags;
    ecs_id_t id = flecs_observer_id(sub->register_id);
    if (flags & (EcsSelf|EcsUp)) {
        size_t offset = flags & EcsSelf
            ? (flags & EcsUp ? offsetof(ecs_event_id_record_t, self_up)
                            : offsetof(ecs_event_id_record_t, self))
            : offsetof(ecs_event_id_record_t, up);
        flecs_observer_update_id(world, sub, offset, id, delta);
    }

    if (sub->trav == EcsChildOf && sub->table_only) {
        flecs_observer_update_id(world, sub, offsetof(ecs_event_id_record_t, self),
            ecs_id(EcsParent), delta);
        flecs_observer_update_id(world, sub, offsetof(ecs_event_id_record_t, self),
            ecs_pair(EcsChildOf, EcsWildcard), delta);
    }
    if (delta < 0) {
        flecs_component_unlock(world, sub->register_id);
    }
}

static bool flecs_ignore_observer(
    ecs_observer_t *o,
    ecs_table_t *table)
{
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_observer_impl_t *impl = flecs_observer_impl(o);

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

static void flecs_default_uni_observer_run_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    it->ctx = o->ctx;
    it->callback = o->callback;
    o->callback(it);
}

static bool flecs_term_ref_is_named_var(
    const ecs_term_ref_t *ref)
{
    return (ref->id & EcsIsVariable) && ref->name != NULL;
}

static bool flecs_observer_query_has_range(
    const ecs_query_t *query,
    ecs_table_range_t *range,
    const ecs_term_t *term,
    ecs_id_t event_id,
    ecs_iter_t *it)
{
#ifdef FLECS_QUERY_PLANS
    bool first_var = flecs_term_ref_is_named_var(&term->first);
    bool second_var = flecs_term_ref_is_named_var(&term->second);
    if (!first_var && !second_var) {
        return ecs_query_has_range(query, range, it);
    }

    ecs_world_t *world = query->real_world;
    ecs_entity_t first = 0, second = 0;
    if (first_var) {
        first = flecs_entities_get_alive(world,
            ECS_IS_PAIR(event_id) ? ECS_PAIR_FIRST(event_id) : event_id);
        if (!first) {
            return false;
        }
    }
    if (second_var) {
        if (!ECS_IS_PAIR(event_id)) {
            return false;
        }
        second = flecs_entities_get_alive(world, ECS_PAIR_SECOND(event_id));
        if (!second) {
            return false;
        }
    }

    *it = ecs_query_iter(query->world, query);
    ecs_iter_set_var_as_range(it, 0, range);
    if (first_var) {
        ecs_iter_set_var(it, ecs_query_find_var(query, term->first.name), first);
    }
    if (second_var) {
        ecs_iter_set_var(it, ecs_query_find_var(query, term->second.name), second);
    }

    return ecs_query_next(it);
#else
    (void)term;
    (void)event_id;
    return ecs_query_has_range(query, range, it);
#endif
}

static void flecs_observer_invoke(
    ecs_observer_t *o,
    ecs_iter_t *it)
{
    if (flecs_observer_impl(o)->flags & EcsObserverIsMulti) {
        it->ctx = o;
        flecs_multi_observer_builtin_run(it);
    } else if (o->run) {
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

static void flecs_uni_observer_invoke(
    ecs_world_t *world,
    ecs_observer_subscription_t *sub,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    ecs_observer_t *o = sub->observer;
    if (flecs_ignore_observer(o, table) ||
        (!sub->trivial && trav && sub->trav != trav))
    {
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
    it->term_index = sub->term_index;

    ecs_entity_t event = it->event;
    int32_t event_cur = it->event_cur;
    ecs_entity_t old_system = flecs_stage_set_system(
        world->stages[0], o->entity);
    ecs_flags32_t set_fields_cur = it->set_fields;
    it->set_fields = 1;

    ecs_query_t *query = o->query;
    it->query = query;

    if (sub->trivial) {
        /* Invoke trivial observer */
        it->event = event;
        flecs_observer_invoke(o, it);
    } else {
        ECS_BIT_COND(it->flags, EcsIterNoData, sub->filter);
        it->ref_fields = sub->row_field ||
            ((sub->src & EcsIsEntity) && (sub->src & ~EcsTermRefFlags));
        ecs_termset_t row_fields = it->row_fields;
        it->row_fields = sub->row_field;
        it->event = flecs_get_observer_event(sub, event);
        bool match_this = (sub->src & EcsIsVariable) &&
            ((sub->src & ~EcsTermRefFlags) == EcsThis);

        if (match_this) {
            /* Invoke observer for $this field */
            flecs_observer_invoke(o, it);
            if (!(impl->flags & EcsObserverIsMulti)) {
                ecs_os_inc(&query->eval_count);
            }
        } else {
            /* Not a $this field, translate the iterator data from a $this field to
             * a field with it->sources set. */
            ecs_entity_t observer_src = (sub->src & ~EcsTermRefFlags);
            ecs_assert(observer_src != 0, ECS_INTERNAL_ERROR, NULL);
            const ecs_entity_t *entities = it->entities;
            int32_t i, count = it->count;
            ecs_entity_t src = it->sources[0];
            ecs_table_t *old_table = it->table;
            int16_t old_column = it->columns[0];

            it->entities = NULL;
            it->count = 0;
            it->table = NULL;
            ECS_CONST_CAST(int16_t*, it->columns)[0] = -1;

            /* Loop all entities for which the event was emitted. Usually this is
            * just one, but it is possible to emit events for a table range. */
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];

                /* Filter on the source of the observer field */
                if (observer_src == e) {
                    if (!src) {
                        /* Only overwrite source if event wasn't forwarded or
                        * propagated from another entity. */
                        it->sources[0] = e;
                    }

                    flecs_observer_invoke(o, it);
                    if (!(impl->flags & EcsObserverIsMulti)) {
                        ecs_os_inc(&query->eval_count);
                    }

                    /* Restore source */
                    it->sources[0] = src;

                    /* Observer can only match one source explicitly, so we don't
                    * have to check any other entities. */
                    break;
                }
            }

            it->entities = entities;
            it->count = count;
            it->table = old_table;
            ECS_CONST_CAST(int16_t*, it->columns)[0] = old_column;
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

typedef enum flecs_observers_invoke_mode_t {
    FlecsObserversInvokeAll,
    FlecsObserversInvokeUpNotifyOnly,
    FlecsObserversInvokeSkipUpNotify
} flecs_observers_invoke_mode_t;

static void flecs_observers_invoke_intern(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav,
    flecs_observers_invoke_mode_t mode)
{
    if (ecs_map_is_init(observers)) {
        ECS_TABLE_LOCK(it->world, table);

        ecs_map_iter_t oit = ecs_map_iter(observers);
        while (ecs_map_next(&oit)) {
            ecs_observer_subscription_t *sub = ecs_map_ptr(&oit);
            ecs_observer_t *o = sub->observer;
            bool up_notify = (flecs_observer_impl(o)->flags & 
                EcsObserverIsUpNotify) != 0;
            if (mode == FlecsObserversInvokeUpNotifyOnly && !up_notify) {
                continue;
            }
            if (mode == FlecsObserversInvokeSkipUpNotify && up_notify) {
                continue;
            }

            ecs_assert(it->table == table, ECS_INTERNAL_ERROR, NULL);
            flecs_uni_observer_invoke(world, sub, it, table, trav);

            ecs_assert(ecs_map_iter_valid(&oit), ECS_INVALID_OPERATION,
                "observer list modified while notifying: "
                "cannot create observer from observer");
        }

        ECS_TABLE_UNLOCK(it->world, table);
    }
}

void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    flecs_observers_invoke_intern(world, observers, it, table, trav,
        FlecsObserversInvokeAll);
}

void flecs_observers_invoke_up_notify(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    flecs_observers_invoke_intern(world, observers, it, table, trav,
        FlecsObserversInvokeUpNotifyOnly);
}

void flecs_observers_invoke_skip_up_notify(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav)
{
    flecs_observers_invoke_intern(world, observers, it, table, trav,
        FlecsObserversInvokeSkipUpNotify);
}

static void flecs_multi_observer_invoke(
    ecs_iter_t *it) 
{
    ecs_observer_t *o = it->ctx;
    flecs_poly_assert(o, ecs_observer_t);

    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_world_t *world = it->real_world;
    
    int8_t pivot_term = it->term_index;
    ecs_term_t *term = &o->query->terms[pivot_term];
    int8_t pivot_field = term->field_index;
    ecs_termset_t pivot_field_bit = ((ecs_termset_t)1 << pivot_field);

    if (impl->last_event_id[0] == it->event_cur &&
        !(impl->last_event_field & pivot_field_bit))
    {
        /* Already handled this event for a different field */
        return;
    }

    ecs_table_t *table = it->table;
    ecs_table_t *prev_table = it->other_table;

    bool is_not = term->oper == EcsNot;
    if (is_not) {
        table = it->other_table;
        prev_table = it->table;
    }

    ecs_table_t *lock_table = table;
    (void)lock_table;
    table = table ? table : &world->store.root;
    prev_table = prev_table ? prev_table : &world->store.root;

    bool memoizable = !is_not && !(impl->flags & EcsObserverIsMonitor);
    uint64_t epoch = flecs_ito(uint64_t, world->info.table_delete_total);
    if (memoizable && impl->nomatch_table == table &&
        impl->nomatch_table_id == table->id && impl->nomatch_epoch == epoch)
    {
        return;
    }

    ecs_iter_t user_it;

    bool match;
    if (is_not) {
        ecs_table_range_t range = { .table = table };
        match = flecs_observer_query_has_range(
            o->query, &range, term, it->event_id, &user_it);
        if (match) {
            /* The target table matches but the entity hasn't moved to it yet. 
             * Now match the not_query, which will populate the iterator with
             * data from the table the entity is still stored in. */
            user_it.flags |= EcsIterSkip; /* Prevent change detection on fini */
            ecs_iter_fini(&user_it);
            ecs_table_range_t prev_range = { .table = prev_table };
            match = flecs_observer_query_has_range(
                impl->not_query, &prev_range, term, it->event_id, &user_it);

            /* A not query replaces Not terms with Optional terms, so if the 
             * regular query matches, the not_query should also match. */
            ecs_assert(match, ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        int trivial = -1;
        bool type_mismatch = false;
        if (!(impl->flags & EcsObserverIsMonitor)) {
            trivial = flecs_query_trivial_has_range(o->query, &user_it,
                it->world, table, it->offset, it->count, &type_mismatch);
        }

        if (trivial >= 0) {
            match = trivial != 0;
            if (type_mismatch && memoizable) {
                impl->nomatch_table = table;
                impl->nomatch_table_id = table->id;
                impl->nomatch_epoch = epoch;
            }
        } else {
            ecs_table_range_t range = {
                .table = table,
                .offset = it->offset,
                .count = it->count
            };

            match = flecs_observer_query_has_range(
                o->query, &range, term, it->event_id, &user_it);
        }
    }

    if (match) {
        /* Monitor observers only invoke when the query matches for the first
         * time with an entity */
        if (impl->flags & EcsObserverIsMonitor) {
            ecs_iter_t table_it;
            ecs_table_range_t prev_range = { .table = prev_table };
            if (flecs_observer_query_has_range(
                o->query, &prev_range, term, it->event_id, &table_it)) 
            {
                /* Prevent change detection on fini */
                user_it.flags |= EcsIterSkip;
                table_it.flags |= EcsIterSkip;
                ecs_iter_fini(&table_it);
                ecs_iter_fini(&user_it);
                goto done;
            }
        }

        impl->last_event_id[0] = it->event_cur;
        impl->last_event_field = pivot_field_bit;

        /* Patch data from original iterator. If the observer query has
         * wildcards which triggered the original event, the component id that
         * got matched by ecs_query_has_range may not be the same as the one
         * that caused the event. We need to make sure to communicate the
         * component id that actually triggered the observer. */
        ecs_assert(pivot_field >= 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(pivot_field < user_it.field_count, ECS_INTERNAL_ERROR, NULL);
        user_it.ids[pivot_field] = it->event_id;
        user_it.trs[pivot_field] = it->trs[0];
        user_it.sources[pivot_field] = it->sources[0];
        ECS_CONST_CAST(int16_t*, user_it.columns)[pivot_field] =
            it->sources[0] ? -1 : it->columns[0];
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
        ECS_TABLE_LOCK(it->world, lock_table);

        if (o->run) {
            user_it.next = flecs_default_next_callback;
            o->run(&user_it);
        } else {
            user_it.callback(&user_it);
        }

        user_it.flags |= EcsIterSkip; /* Prevent change detection on fini */
        ecs_iter_fini(&user_it);

        ECS_TABLE_UNLOCK(it->world, lock_table);
        flecs_stage_set_system(world->stages[0], old_system);
    } else {
        /* While the observer query was strictly speaking evaluated, it's more
         * useful to measure how often the observer was actually invoked. */
        o->query->eval_count --;
    }

done:
    return;
}

static void flecs_multi_observer_invoke_no_query(
    ecs_iter_t *it) 
{
    ecs_observer_t *o = it->ctx;
    flecs_poly_assert(o, ecs_observer_t);

    ecs_world_t *world = it->real_world;
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
    ECS_TABLE_LOCK(it->world, it->table);

    if (o->run) {
        user_it.next = flecs_default_next_callback;
        o->run(&user_it);
    } else {
        user_it.callback(&user_it);
    }

    ECS_TABLE_UNLOCK(it->world, it->table);
    flecs_stage_set_system(world->stages[0], old_system);
}

/* For convenience, so applications can use a single run callback that uses 
 * ecs_iter_next to iterate results for systems/queries and observers. */
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

static void flecs_multi_observer_builtin_run(ecs_iter_t *it) {
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

static void flecs_observer_yield_existing(
    ecs_world_t *world,
    ecs_observer_t *o,
    bool yield_on_remove)
{
    ecs_run_action_t run = o->run;
    if (!run) {
        run = flecs_multi_observer_invoke_no_query;
    }

    ecs_defer_begin(world);

    /* If yield existing is enabled, invoke for each thing that matches
     * the event, if the event is iterable. */
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
            it = ecs_each_id(world, flecs_observer_impl(o)->subscription.register_id);
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

static void flecs_observer_add_subscription(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_term_t *term,
    ecs_id_t id)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);
    ecs_observer_subscription_t *sub = &impl->subscription;
    if (impl->flags & EcsObserverIsMulti) {
        sub = ecs_vec_append_t(&world->allocator, &impl->subscriptions,
            ecs_observer_subscription_t);
    }
    *sub = (ecs_observer_subscription_t){
        .observer = o, .register_id = id, .src = term->src.id,
        .trav = term->trav, .oper = term->oper,
        .term_index = term->field_index,
        .filter = term->inout == EcsInOutNone,
        .tag = ecs_id_is_tag(world, id),
        .trivial = !o->query,
        .table_only = o->query && (o->query->flags & EcsQueryTableOnly),
        .row_field = o->query &&
            ((o->query->row_fields & (1u << term->field_index)) != 0)
    };
}

static int flecs_multi_observer_init(
    ecs_world_t *world,
    ecs_observer_t *o,
    const ecs_observer_desc_t *desc)
{
    ecs_observer_impl_t *impl = flecs_observer_impl(o);

    impl->flags |= EcsObserverIsMulti;
    ecs_vec_init_t(&world->allocator, &impl->subscriptions,
        ecs_observer_subscription_t, o->query->term_count);
    ecs_query_t *query = o->query;

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

    /* If an observer is only interested in table events, we only need to
     * observe a single component, as each table event will be emitted for all
     * components of the source table. */
    bool only_table_events = true;
    for (i = 0; i < o->event_count; i ++) {
        ecs_entity_t e = o->events[i];
        if (e != EcsOnTableCreate && e != EcsOnTableDelete) {
            only_table_events = false;
            break;
        }
    }

    bool self_term_handled = false;
    for (i = 0; i < term_count; i ++) {
        if (query->terms[i].inout == EcsInOutFilter && !only_table_events) {
            continue;
        }

        ecs_term_t term_value = query->terms[i];
        ecs_term_t *term = &term_value;

        /* Don't create observers for non-$this terms */
        if (!ecs_term_match_this(term) && term->src.id & EcsIsVariable) {
            continue;
        }

        int16_t oper = term->oper;
        ecs_id_t id = term->id;

        if (only_table_events) {
            /* For table event observers, only observe a single $this|self 
             * term. Make sure to create observers for non-self terms, as those
             * require event propagation. */
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
                ecs_component_record_t *cr = flecs_components_get(world, ti_id);
                if (cr->flags & EcsIdOnInstantiateDontInherit) {
                    continue;
                }

                flecs_observer_add_subscription(world, o, term, ti_id);
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
            if (only_table_events || desc->events[0] == EcsMonitor) {
                /* For table events & monitors optional terms aren't necessary */
                continue;
            }
        }

        flecs_observer_add_subscription(world, o, term, term->id);

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

    ecs_observer_subscription_t *subs = ecs_vec_first(&impl->subscriptions);
    for (i = 0; i < ecs_vec_count(&impl->subscriptions); i ++) {
        flecs_observer_update_registration(world, &subs[i], 1);
    }
    return 0;
}

static void flecs_observer_poly_fini(void *ptr) {
    flecs_observer_fini(ptr);
}

static bool flecs_observer_init_trivial(
    ecs_world_t *world,
    const ecs_query_desc_t *desc,
    ecs_term_t *term,
    ecs_flags32_t *flags)
{
    if (desc->expr || desc->order_by_callback || desc->group_by_callback ||
        !ecs_term_is_initialized(&desc->terms[0]) ||
        ecs_term_is_initialized(&desc->terms[1]))
    {
        return false;
    }
    *term = desc->terms[0];
    ecs_query_validator_ctx_t ctx = {
        .world = world, .desc = desc, .term = term
    };
    if (flecs_term_finalize(world, term, &ctx)) {
        return false;
    }
    bool wildcard = ecs_id_is_wildcard(term->id);
    bool disabled = term->id == EcsDisabled;
#ifdef FLECS_PREFAB
    bool prefab = term->id == EcsPrefab;
#else
    bool prefab = false;
#endif
    bool special = wildcard || disabled || prefab;
#ifdef FLECS_SANITIZE
    if (!special) {
        return false;
    }
#endif
    if (term->oper != EcsAnd || !ecs_term_match_this(term) ||
        ((term->src.id & EcsTraverseFlags) != EcsSelf) || term->trav ||
        ECS_PAIR_FIRST(term->id) == EcsChildOf ||
        (term->flags_ & (EcsTermIsToggle|EcsTermDontFragment|EcsTermIsSparse|
            EcsTermTransitive|EcsTermReflexive|EcsTermIdInherited|EcsTermIsMember)) ||
        (!special && (!(term->flags_ & EcsTermIsTrivial) || term->inout)) ||
        (wildcard && term->id != EcsAny && !(term->flags_ & EcsTermIsCacheable)))
    {
        return false;
    }
    ecs_component_record_t *cr = flecs_components_get(world, term->id);
    ecs_flags32_t cr_flags = cr ? cr->flags :
        flecs_component_get_flags(world, term->id);
    if (cr_flags & (EcsIdSparse|EcsIdDontFragment)) {
        return false;
    }
#ifdef FLECS_CONSTRAINT_TRAITS
    if (!desc->terms[0].src.id && !desc->terms[0].src.name &&
        (cr_flags & EcsIdSingleton))
    {
        return false;
    }
#endif
    *flags = desc->flags;
    if (disabled) {
        *flags |= EcsQueryMatchDisabled;
    }
    if (prefab) {
        *flags |= EcsQueryMatchPrefab;
    }
    return true;
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

    ecs_observer_impl_t *impl = flecs_calloc_t(
        &world->allocator, ecs_observer_impl_t);
    ecs_assert(impl != NULL, ECS_INTERNAL_ERROR, NULL);
    impl->last_event_id = desc->last_event_id
        ? desc->last_event_id : &impl->last_event_id_storage;

    flecs_poly_init(impl, ecs_observer_t);
    ecs_observer_t *o = &impl->pub;
    o->world = world;
    impl->dtor = flecs_observer_poly_fini;

    /* Make writable copy of query desc so that we can set name. This will
     * make debugging easier, as any error messages related to creating the
     * query will have the name of the observer. */
    ecs_query_desc_t query_desc = desc->query;
    query_desc.entity = 0;
    query_desc.cache_kind = EcsQueryCacheNone;

    ecs_term_t trivial_term;
    ecs_term_t *terms = &trivial_term;
    int32_t term_count = 1;
    ecs_flags32_t query_flags = 0;
    bool trivial = desc->events[0] != EcsMonitor &&
        flecs_observer_init_trivial(world, &query_desc, &trivial_term, &query_flags);
    if (!trivial) {
        ecs_query_t *query = o->query = ecs_query_init(world, &query_desc);
        if (!query) {
            flecs_observer_fini(o);
            return NULL;
        }
        terms = query->terms;
        term_count = query->term_count;
        query_flags = query->flags;
    }

    ecs_check(term_count > 0, ECS_INVALID_PARAMETER,
        "observer must have at least one term");

    int i;
#ifdef FLECS_QUERY_PLANS
    int var_count = 0;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (!ecs_term_match_this(term)) {
            if (term->src.id & EcsIsVariable) {
                /* Term has a non-$this variable source */
                var_count ++;
            }
        }
    }

    ecs_check(term_count > var_count, ECS_UNSUPPORTED,
        "observers with only non-$this variable sources are not yet supported");
    (void)var_count;
#endif

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
    impl->flags |= desc->flags_ | 
        (query_flags & (EcsQueryMatchPrefab|EcsQueryMatchDisabled));

    ecs_check(!(desc->yield_existing && 
        (desc->flags_ & (EcsObserverYieldOnCreate|EcsObserverYieldOnDelete))), 
        ECS_INVALID_PARAMETER,
         "cannot set yield_existing and YieldOn* flags at the same time");

    /* Check if observer is monitor. Monitors are created as multi observers
     * since they require pre/post checking of the query to test if the
     * entity is entering/leaving the monitor. */
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

    if (term_count == 1 && !desc->last_event_id) {
        ecs_term_t *term = &terms[0];
        /* If the query has a single term but it is a *From operator, we
         * need to create a multi observer */
        multi |= (term->oper == EcsAndFrom) || (term->oper == EcsOrFrom);
        
        /* An observer with only optional terms is a special case that is
         * only handled by multi observers */
        multi |= term->oper == EcsOptional;
    }

    if (term_count == 1) {
        ecs_term_t *term = &terms[0];
        multi |= flecs_term_ref_is_named_var(&term->first) ||
            flecs_term_ref_is_named_var(&term->second);
    }

    bool is_monitor = impl->flags & EcsObserverIsMonitor;
    if (term_count == 1 && !is_monitor && !multi) {
        ecs_term_t *term = &terms[0];
        flecs_observer_add_subscription(world, o, term, term->id);
        impl->subscription.term_index = flecs_ito(int8_t, desc->term_index_);
        if (impl->subscription.tag) {
            for (i = 0; i < o->event_count; i ++) {
                if (o->events[i] == EcsOnSet) {
                    o->events[i] = EcsOnAdd;
                }
            }
        }
        flecs_observer_update_registration(world, &impl->subscription, 1);
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

    bool entity_created = false;
    entity = desc->entity;
    if (!entity && !desc->global_observer) {
        entity = ecs_entity(world, {0});
        entity_created = true;
    }

    if (!entity) {
        ecs_observer_t *o = flecs_observer_init(world, entity, desc);
        if (!o) {
            goto error;
        }

        ecs_vec_append_t(NULL, &world->observable.global_observers,
            ecs_observer_t*)[0] = o;
    } else {
        EcsPoly *poly = flecs_poly_bind(world, entity, ecs_observer_t);
        ecs_check(poly->poly == NULL, ECS_INVALID_OPERATION,
            "entity %s already is an observer, use ecs_observer_update() "
                "to modify",
                    flecs_errstr(ecs_get_path(world, entity)));

        ecs_observer_t *o = flecs_observer_init(world, entity, desc);
        if (!o) {
            goto error;
        }

        ecs_assert(o->entity == entity, ECS_INTERNAL_ERROR, NULL);
        poly->poly = o;

        /* EcsObserver is added before the poly object is installed. If the
         * entity was already disabled, initialize the observer flag now. */
        if (ecs_has_id(world, entity, EcsDisabled)) {
            flecs_observer_set_disable_bit(
                world, entity, EcsObserverIsDisabled, true);
        }

        if (ecs_get_name(world, entity)) {
            ecs_trace("#[green]observer#[reset] %s created",
                ecs_get_name(world, entity));
        }

        flecs_poly_modified(world, entity, ecs_observer_t);
    }

    return entity;
error:
    /* Only delete the entity if we created it ourselves; entities provided by
     * the caller must be preserved on failure. */
    if (entity_created) {
        ecs_delete(world, entity);
    }
    return 0;
}

ecs_entity_t ecs_observer_update(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_observer_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_observer_desc_t was not initialized to zero");
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!desc->entity || desc->entity == entity, ECS_INVALID_PARAMETER,
        "ecs_observer_desc_t::entity does not match observer entity");
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION,
        "cannot update observer while world is being deleted");

    ecs_observer_t *o = flecs_poly_get(world, entity, ecs_observer_t);
    ecs_check(o != NULL, ECS_INVALID_PARAMETER,
        "entity %s is not an observer, use ecs_observer_init() to create it",
            flecs_errstr(ecs_get_path(world, entity)));

    flecs_poly_update_ctx(&o->ctx, &o->ctx_free,
        desc->ctx, desc->ctx_free, true);
    flecs_poly_update_ctx(&o->callback_ctx, &o->callback_ctx_free,
        desc->callback_ctx, desc->callback_ctx_free, false);
    flecs_poly_update_ctx(&o->run_ctx, &o->run_ctx_free,
        desc->run_ctx, desc->run_ctx_free, false);

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


    flecs_poly_modified(world, entity, ecs_observer_t);

    return entity;
error:
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
    ecs_world_t *world = o->world;

    flecs_poly_assert(world, ecs_world_t);
    ecs_observer_impl_t *impl = flecs_observer_impl(o);

    if (impl->flags & EcsObserverYieldOnDelete) {
        flecs_observer_yield_existing(world, o, true);
    }

    ecs_observer_subscription_t *subs = ecs_vec_first(&impl->subscriptions);
    for (int32_t i = 0; i < ecs_vec_count(&impl->subscriptions); i ++) {
        flecs_observer_update_registration(world, &subs[i], -1);
    }
    flecs_observer_update_registration(world, &impl->subscription, -1);
    ecs_vec_fini_t(&world->allocator, &impl->subscriptions,
        ecs_observer_subscription_t);
    if (o->query) {
        ecs_query_fini(o->query);
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
    ECS_BIT_COND(impl->flags, bit, cond);
}
