#include "private_api.h"
#include <stddef.h>

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
    user_it.term_count = o->filter.term_count_actual;
    user_it.terms = o->filter.terms;
    user_it.flags = 0;
    ECS_BIT_COND(user_it.flags, EcsIterIsFilter,    
        ECS_BIT_IS_SET(o->filter.flags, EcsFilterIsFilter));
    user_it.ids = NULL;
    user_it.columns = NULL;
    user_it.subjects = NULL;
    user_it.sizes = NULL;
    user_it.ptrs = NULL;
    flecs_iter_init(&user_it, flecs_iter_cache_all);

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

    if (flecs_filter_match_table(world, &o->filter, table, user_it.ids, 
        user_it.columns, user_it.subjects, NULL, NULL, false, -1, 
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
         * correct column ids and subjects, which we need for populate_data */
        if (term->oper == EcsNot) {
            flecs_filter_match_table(world, &o->filter, prev_table, user_it.ids, 
                user_it.columns, user_it.subjects, NULL, NULL, false, -1, 
                user_it.flags | EcsFilterPopulate);
        }

        flecs_iter_populate_data(world, &user_it, it->table, it->offset, 
            it->count, user_it.ptrs, user_it.sizes);

        user_it.ids[it->term_index] = it->event_id;
        user_it.system = o->entity;
        user_it.term_index = it->term_index;
        user_it.ctx = o->ctx;
        user_it.binding_ctx = o->binding_ctx;
        user_it.term_count = o->filter.term_count_actual;
        flecs_iter_validate(&user_it);

        ecs_assert(o->callback != NULL, ECS_INVALID_PARAMETER, NULL);
        o->callback(&user_it);

        ecs_iter_fini(&user_it);
        return true;
    }

done:
    ecs_iter_fini(&user_it);
    return false;
}

static
ecs_entity_t flecs_observer_get_actual_event(
    ecs_observer_t *observer, 
    ecs_entity_t event)
{
    /* If operator is Not, reverse the event */
    if (observer->filter.terms[0].oper == EcsNot) {
        if (event == EcsOnAdd) {
            event = EcsOnRemove;
        } else if (event == EcsOnRemove) {
            event = EcsOnAdd;
        }
    }

    return event;
}

static
void flecs_unregister_event_observer(
    ecs_event_record_t *evt,
    ecs_id_t id)
{
    if (ecs_map_remove(&evt->event_ids, id) == 0) {
        ecs_map_fini(&evt->event_ids);
    }
}

static
ecs_event_id_record_t* flecs_ensure_event_id_record(
    ecs_map_t *map,
    ecs_id_t id)
{
    ecs_event_id_record_t **idt = ecs_map_ensure(
        map, ecs_event_id_record_t*, id);
    if (!idt[0]) {
        idt[0] = ecs_os_calloc_t(ecs_event_id_record_t);
    }

    return idt[0];
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
    ecs_event_id_record_t *idt = flecs_ensure_event_id_record(&evt->event_ids, id);
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

        /* Remove admin for id for event */
        if (!ecs_map_is_initialized(&idt->observers) && 
            !ecs_map_is_initialized(&idt->set_observers)) 
        {
            flecs_unregister_event_observer(evt, id);
            ecs_os_free(idt);
        }
    }
}

static
void flecs_register_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer,
    ecs_id_t id,
    size_t offset)
{
    ecs_sparse_t *events = observable->events;
    ecs_assert(events != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t term_id = observer->register_id;

    int i;
    for (i = 0; i < observer->event_count; i ++) {
        ecs_entity_t event = flecs_observer_get_actual_event(
            observer, observer->events[i]);

        /* Get observers for event */
        ecs_event_record_t *evt = flecs_sparse_ensure(
            events, ecs_event_record_t, event);
        ecs_assert(evt != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!ecs_map_is_initialized(&evt->event_ids)) {
            ecs_map_init(&evt->event_ids, ecs_event_id_record_t*, 1);
        }

        /* Get observers for (component) id for event */
        ecs_event_id_record_t *idt = flecs_ensure_event_id_record(
            &evt->event_ids, id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *observers = ECS_OFFSET(idt, offset);
        if (!ecs_map_is_initialized(observers)) {
            ecs_map_init(observers, ecs_observer_t*, 1);
        }

        ecs_map_ensure(observers, ecs_observer_t*, 
            observer->entity)[0] = observer;

        flecs_inc_observer_count(world, event, evt, term_id, 1);
        if (term_id != id) {
            flecs_inc_observer_count(world, event, evt, id, 1);
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
    ecs_id_t id = observer->register_id;

    if (term->subj.set.mask & EcsSelf) {
        if (term->subj.entity == EcsThis) {
            flecs_register_observer_for_id(world, observable, observer, id,
                offsetof(ecs_event_id_record_t, observers));
        } else {
            flecs_register_observer_for_id(world, observable, observer, id,
                offsetof(ecs_event_id_record_t, entity_observers));
        }
    }

    if (observer->filter.terms[0].subj.set.mask & EcsSuperSet) {
        ecs_id_t pair = ecs_pair(term->subj.set.relation, EcsWildcard);
        flecs_register_observer_for_id(world, observable, observer, pair,
            offsetof(ecs_event_id_record_t, set_observers));
    }
}

static
void flecs_unregister_observer_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_observer_t *observer,
    ecs_id_t id,
    size_t offset)
{
    ecs_sparse_t *events = observable->events;
    ecs_assert(events != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t term_id = observer->register_id;

    int i;
    for (i = 0; i < observer->event_count; i ++) {
        ecs_entity_t event = flecs_observer_get_actual_event(
            observer, observer->events[i]);

        /* Get observers for event */
        ecs_event_record_t *evt = flecs_sparse_get(
            events, ecs_event_record_t, event);
        ecs_assert(evt != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get observers for (component) id */
        ecs_event_id_record_t *idt = ecs_map_get_ptr(
            &evt->event_ids, ecs_event_id_record_t*, id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *id_observers = ECS_OFFSET(idt, offset);

        if (ecs_map_remove(id_observers, observer->entity) == 0) {
            ecs_map_fini(id_observers);
        }

        flecs_inc_observer_count(world, event, evt, term_id, -1);

        if (id != term_id) {
            /* Id is different from term_id in case of a set observer. If they're
             * the same, flecs_inc_observer_count could already have done cleanup */
            if (!ecs_map_is_initialized(&idt->observers) && 
                !ecs_map_is_initialized(&idt->set_observers) && 
                !idt->observer_count) 
            {
                flecs_unregister_event_observer(evt, id);
            }

            flecs_inc_observer_count(world, event, evt, id, -1);
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
    ecs_id_t id = observer->register_id;

    if (term->subj.set.mask & EcsSelf) {
        if (term->subj.entity == EcsThis) {
            flecs_unregister_observer_for_id(world, observable, observer, id,
                offsetof(ecs_event_id_record_t, observers));
        } else {
            flecs_unregister_observer_for_id(world, observable, observer, id,
                offsetof(ecs_event_id_record_t, entity_observers));
        }
    }

    if (term->subj.set.mask & EcsSuperSet) {
        ecs_id_t pair = ecs_pair(term->subj.set.relation, EcsWildcard);
        flecs_unregister_observer_for_id(world, observable, observer, pair,
            offsetof(ecs_event_id_record_t, set_observers));
    }
}

static
ecs_map_t* flecs_get_observers_for_event(
    const ecs_observable_t *observable,
    ecs_entity_t event)
{
    ecs_check(observable != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(event != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_sparse_t *events = observable->events;
    ecs_assert(events != NULL, ECS_INTERNAL_ERROR, NULL);

    const ecs_event_record_t *evt = flecs_sparse_get(
        events, ecs_event_record_t, event);
    
    if (evt) {
        return (ecs_map_t*)&evt->event_ids;
    }

error:
    return NULL;
}

static
ecs_event_id_record_t* flecs_get_observers_for_id(
    const ecs_map_t *evt,
    ecs_id_t id)
{
    return ecs_map_get_ptr(evt, ecs_event_id_record_t*, id);
}

static
void flecs_init_observer_iter(
    ecs_iter_t *it,
    bool *iter_set)
{
    ecs_assert(it != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (*iter_set) {
        return;
    }

    if (ECS_BIT_IS_SET(it->flags, EcsIterTableOnly)) {
        it->ids = it->priv.cache.ids;
        it->ids[0] = it->event_id;
        return;
    }

    flecs_iter_init(it, flecs_iter_cache_all);
    flecs_iter_validate(it);

    *iter_set = true;

    it->ids[0] = it->event_id;

    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!it->count || it->offset < ecs_table_count(it->table), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert((it->offset + it->count) <= ecs_table_count(it->table), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t index = ecs_search_relation(it->real_world, it->table, 0, 
        it->event_id, EcsIsA, 0, 0, it->subjects, 0, 0, 0);
    
    if (index == -1) {
        it->columns[0] = 0;
    } else if (it->subjects[0]) {
        it->columns[0] = -index - 1;
    } else {
        it->columns[0] = index + 1;
    }

    ecs_term_t term = {
        .id = it->event_id
    };

    it->term_count = 1;
    it->terms = &term;
    flecs_iter_populate_data(it->real_world, it, it->table, it->offset, 
        it->count, it->ptrs, it->sizes);
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

bool ecs_observer_default_run_action(ecs_iter_t *it) {
    ecs_observer_t *observer = it->ctx;
    if (observer->is_multi) {
        return flecs_multi_observer_invoke(it);
    } else {
        it->callback(it);
        return true;
    }
}

static 
void flecs_default_multi_observer_run_callback(ecs_iter_t *it) {
    flecs_multi_observer_invoke(it);
}

static 
void flecs_default_uni_observer_run_callback(ecs_iter_t *it) {
    ecs_observer_t *observer = it->ctx;
    it->ctx = observer->ctx;
    it->callback = observer->callback;
    it->callback(it);
}

/* For convenience, so applications can (in theory) use a single run callback 
 * that uses ecs_iter_next to iterate results */
static 
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

/* Run action for uni observer */
static
void flecs_uni_observer_builtin_run(
    ecs_observer_t *observer,
    ecs_iter_t *it)
{
    ECS_BIT_COND(it->flags, EcsIterIsFilter, 
        observer->filter.terms[0].inout == EcsInOutFilter);

    it->system = observer->entity;
    it->ctx = observer->ctx;
    it->binding_ctx = observer->binding_ctx;
    it->term_index = observer->term_index;
    it->terms = observer->filter.terms;

    void *ptrs = it->ptrs;
    if (it->flags & EcsIterIsFilter) {
        it->ptrs = NULL;
    }

    ecs_entity_t event = it->event;
    it->event = flecs_observer_get_actual_event(observer, event);

    if (observer->run) {
        it->next = flecs_default_observer_next_callback;
        it->callback = flecs_default_uni_observer_run_callback;
        it->ctx = observer;
        observer->run(it);
    } else {
        observer->callback(it);
    }

    it->event = event;
    it->ptrs = ptrs;
}

static
void flecs_notify_self_observers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *observers)
{
    ecs_assert(observers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_iter_t mit = ecs_map_iter(observers);
    ecs_observer_t *observer;
    while ((observer = ecs_map_next_ptr(&mit, ecs_observer_t*, NULL))) {
        if (flecs_ignore_observer(world, observer, it->table)) {
            continue;
        }
        flecs_uni_observer_builtin_run(observer, it);
    }
}

static
void flecs_notify_entity_observers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *observers)
{
    ecs_assert(observers != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_BIT_IS_SET(it->flags, EcsIterTableOnly)) {
        return;
    }

    ecs_map_iter_t mit = ecs_map_iter(observers);
    ecs_observer_t *observer;
    int32_t offset = it->offset, count = it->count;
    ecs_entity_t *entities = it->entities;
    
    ecs_entity_t dummy = 0;
    it->entities = &dummy;

    while ((observer = ecs_map_next_ptr(&mit, ecs_observer_t*, NULL))) {
        if (flecs_ignore_observer(world, observer, it->table)) {
            continue;
        }

        int32_t i, entity_count = it->count;
        for (i = 0; i < entity_count; i ++) {
            if (entities[i] != observer->filter.terms[0].subj.entity) {
                continue;
            }

            it->offset = i;
            it->count = 1;
            it->subjects[0] = entities[i];
            flecs_uni_observer_builtin_run(observer, it);
        }
    }

    it->offset = offset;
    it->count = count;
    it->entities = entities;
    it->subjects[0] = 0;
}

static
void flecs_notify_set_base_observers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *observers)
{
    ecs_assert(observers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t event_id = it->event_id;
    ecs_entity_t rel = ECS_PAIR_FIRST(event_id);
    ecs_entity_t obj = ecs_pair_second(world, event_id);
    if (!obj) {
        /* Don't notify for deleted (or in progress of being deleted) object */
        return;
    }

    ecs_record_t *obj_record = flecs_entities_get(world, obj);
    if (!obj_record) {
        return;
    }

    ecs_table_t *obj_table = obj_record->table;
    if (!obj_table) {
        return;
    }

    ecs_map_iter_t mit = ecs_map_iter(observers);
    ecs_observer_t *observer;
    while ((observer = ecs_map_next_ptr(&mit, ecs_observer_t*, NULL))) {
        if (flecs_ignore_observer(world, observer, it->table)) {
            continue;
        }

        ecs_term_t *term = &observer->filter.terms[0];
        ecs_id_t id = term->id;
        int32_t column = ecs_search_relation(world, obj_table, 0, id, rel, 
            0, 0, it->subjects, it->ids, 0, 0);
        
        bool result = column != -1;
        if (term->oper == EcsNot) {
            result = !result;
        }
        if (!result) {
            continue;
        }

        if (!term->subj.set.min_depth && flecs_table_record_get(
            world, it->table, id) != NULL) 
        {
            continue;
        }

        if (!ECS_BIT_IS_SET(it->flags, EcsIterTableOnly)) {
            if (!it->subjects[0]) {
                it->subjects[0] = obj;
            }

            /* Populate pointer from object */
            int32_t s_column = ecs_table_type_to_storage_index(
                obj_table, column);
            if (s_column != -1) {
                ecs_column_t *c = &obj_table->data.columns[s_column];
                int32_t row = ECS_RECORD_TO_ROW(obj_record->row);
                ecs_type_info_t *ti = obj_table->type_info[s_column];
                void *ptr = ecs_storage_get(c, ti->size, row);
                it->ptrs[0] = ptr;
                it->sizes[0] = ti->size;
            }
        }

        it->event_id = observer->filter.terms[0].id;
        flecs_uni_observer_builtin_run(observer, it);
    }
}

static
void flecs_notify_set_observers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *observers)
{
    ecs_assert(observers != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(it->count != 0, ECS_INTERNAL_ERROR, NULL);

    if (ECS_BIT_IS_SET(it->flags, EcsIterTableOnly)) {
        return;
    }

    ecs_map_iter_t mit = ecs_map_iter(observers);
    ecs_observer_t *observer;
    while ((observer = ecs_map_next_ptr(&mit, ecs_observer_t*, NULL))) {
        if (!ecs_id_match(it->event_id, observer->filter.terms[0].id)) {
            continue;
        }

        if (flecs_ignore_observer(world, observer, it->table)) {
            continue;
        }

        ecs_entity_t subj = it->entities[0];
        int32_t i, count = it->count;
        ecs_filter_t *filter = &observer->filter;
        ecs_term_t *term = &filter->terms[0];
        ecs_entity_t term_subj = term->subj.entity;

        /* If observer is for a specific entity, make sure it is in the table
         * being triggered for */
        if (term_subj != EcsThis) {
            for (i = 0; i < count; i ++) {
                if (it->entities[i] == term_subj) {
                    break;
                }
            }

            if (i == count) {
                continue;
            }

            /* If the entity matches, observer for no other entities */
            it->entities[0] = 0;
            it->count = 1;
        }

        if (flecs_term_match_table(world, term, it->table, it->ids, 
            it->columns, it->subjects, NULL, true, it->flags))
        {
            if (!it->subjects[0]) {
                /* Do not match owned components */
                continue;
            }

            /* Triggers for supersets can be instanced */
            bool instanced = filter->flags & EcsFilterIsInstanced;
            bool is_filter = ECS_BIT_IS_SET(it->flags, EcsIterIsFilter);
            if (it->count == 1 || instanced || is_filter || !it->sizes[0]) {
                ECS_BIT_COND(it->flags, EcsIterIsInstanced, instanced);
                flecs_uni_observer_builtin_run(observer, it);
                ECS_BIT_CLEAR(it->flags, EcsIterIsInstanced);
            } else {
                ecs_entity_t *entities = it->entities;
                it->count = 1;
                for (i = 0; i < count; i ++) {
                    it->entities = &entities[i];
                    flecs_uni_observer_builtin_run(observer, it);
                }
                it->entities = entities;
            }
        }

        it->entities[0] = subj;
        it->count = count;            
    }
}

static
void flecs_notify_observers_for_id(
    ecs_world_t *world,
    const ecs_map_t *evt,
    ecs_id_t event_id,
    ecs_iter_t *it,
    bool *iter_set)
{
    const ecs_event_id_record_t *idt = flecs_get_observers_for_id(evt, event_id);
    if (!idt) {
        return;
    }

    if (ecs_map_is_initialized(&idt->observers)) {
        flecs_init_observer_iter(it, iter_set);
        flecs_notify_self_observers(world, it, &idt->observers);
    }
    if (ecs_map_is_initialized(&idt->entity_observers)) {
        flecs_init_observer_iter(it, iter_set);
        flecs_notify_entity_observers(world, it, &idt->entity_observers);
    }
    if (ecs_map_is_initialized(&idt->set_observers)) {
        flecs_init_observer_iter(it, iter_set);
        flecs_notify_set_base_observers(world, it, &idt->set_observers);
    }
}

static
void flecs_notify_set_observers_for_id(
    ecs_world_t *world,
    const ecs_map_t *evt,
    ecs_iter_t *it,
    bool *iter_set,
    ecs_id_t set_id)
{
    const ecs_event_id_record_t *idt = flecs_get_observers_for_id(evt, set_id);
    if (idt && ecs_map_is_initialized(&idt->set_observers)) {
        flecs_init_observer_iter(it, iter_set);
        flecs_notify_set_observers(world, it, &idt->set_observers);
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
        it.system = observer->entity;
        it.ctx = observer->ctx;
        it.binding_ctx = observer->binding_ctx;
        it.event = evt;

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            it.event_id = it.ids[0];
            callback(&it);
        }
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
        it.term_count = 1;
        it.term_index = pivot_term;
        it.system = observer->entity;
        it.ctx = observer;
        it.binding_ctx = observer->binding_ctx;
        it.event = evt;

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            run(&it);
            world->event_id ++;
        }
    }
}

bool flecs_check_observers_for_event(
    const ecs_poly_t *object,
    ecs_id_t id,
    ecs_entity_t event)
{    
    ecs_observable_t *observable = ecs_get_observable(object);
    const ecs_map_t *evt = flecs_get_observers_for_event(observable, event);
    if (!evt) {
        return false;
    }

    ecs_event_id_record_t *edr = flecs_get_observers_for_id(evt, id);
    if (edr) {
        return edr->observer_count != 0;
    } else {
        return false;
    }
}

void flecs_observers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    const ecs_type_t *ids,
    ecs_entity_t event)
{
    ecs_assert(ids != NULL && ids->count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ids->array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t events[2] = {event, EcsWildcard};
    int32_t e, i, ids_count = ids->count;
    ecs_id_t *ids_array = ids->array;
    ecs_world_t *world = it->real_world;

    for (e = 0; e < 2; e ++) {
        event = events[e];
        const ecs_map_t *evt = flecs_get_observers_for_event(observable, event);
        if (!evt) {
            continue;
        }

        it->event = event;

        for (i = 0; i < ids_count; i ++) {
            ecs_id_t id = ids_array[i];
            ecs_entity_t role = id & ECS_ROLE_MASK;
            bool iter_set = false;

            it->event_id = id;

            flecs_notify_observers_for_id(world, evt, id, it, &iter_set);

            if (role == ECS_PAIR) {
                ecs_entity_t r = ECS_PAIR_FIRST(id);
                ecs_entity_t o = ECS_PAIR_SECOND(id);

                ecs_id_t tid = ecs_pair(r, EcsWildcard);
                flecs_notify_observers_for_id(world, evt, tid, it, &iter_set);

                tid = ecs_pair(EcsWildcard, o);
                flecs_notify_observers_for_id(world, evt, tid, it, &iter_set);
                
                tid = ecs_pair(EcsWildcard, EcsWildcard);
                flecs_notify_observers_for_id(world, evt, tid, it, &iter_set);
            } else {
                flecs_notify_observers_for_id(
                    world, evt, EcsWildcard, it, &iter_set);
            }

            flecs_notify_observers_for_id(world, evt, EcsAny, it, &iter_set);

            if (iter_set) {
                ecs_iter_fini(it);
            }
        }
    }
}

void flecs_set_observers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    const ecs_type_t *ids,
    ecs_entity_t event,
    ecs_id_t set_id)
{
    ecs_assert(ids != NULL && ids->count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ids->array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t events[2] = {event, EcsWildcard};
    int32_t e, i, ids_count = ids->count;
    ecs_id_t *ids_array = ids->array;
    ecs_world_t *world = it->real_world;

    for (e = 0; e < 2; e ++) {
        event = events[e];
        const ecs_map_t *evt = flecs_get_observers_for_event(observable, event);
        if (!evt) {
            continue;
        }

        it->event = event;

        for (i = 0; i < ids_count; i ++) {
            ecs_id_t id = ids_array[i];
            bool iter_set = false;

            it->event_id = id;

            flecs_notify_set_observers_for_id(world, evt, it, &iter_set, set_id);

            if (iter_set) {
                ecs_iter_fini(it);
            }
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
    term->index = desc->term_index;

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
    child_desc.filter.expr = NULL;
    child_desc.filter.terms_buffer = NULL;
    child_desc.filter.terms_buffer_count = 0;
    child_desc.binding_ctx = NULL;
    child_desc.binding_ctx_free = NULL;
    ecs_os_zeromem(&child_desc.entity);
    ecs_os_zeromem(&child_desc.filter.terms);
    ecs_os_memcpy_n(child_desc.events, observer->events, 
        ecs_entity_t, observer->event_count);

    int i, term_count = filter->term_count;
    bool optional_only = filter->flags & EcsFilterMatchThis;
    for (i = 0; i < term_count; i ++) {
        if (filter->terms[i].oper != EcsOptional) {
            if (filter->terms[i].subj.entity == EcsThis) {
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
    ecs_entity_t old_scope = ecs_set_scope(world, observer->entity);

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &child_desc.filter.terms[0];
        child_desc.term_index = filter->terms[i].index;
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

                term->pred.name = NULL;
                term->pred.entity = ti_ids[ti];
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
            term->pred.entity = EcsAny;
            term->subj.entity = EcsThis;
            term->subj.var = EcsVarIsVariable;
            term->obj.entity = 0;
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

    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);
    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_observer_t);
    if (!poly->poly) {
        ecs_check(desc->callback != NULL || desc->run != NULL, 
            ECS_INVALID_OPERATION, NULL);

        ecs_observer_t *observer = ecs_poly_new(ecs_observer_t);
        ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
        
        observer->world = world;
        observer->dtor = (ecs_poly_dtor_t)flecs_observer_fini;
        observer->entity = entity;

        /* Make writeable copy of filter desc so that we can set name. This will
         * make debugging easier, as any error messages related to creating the
         * filter will have the name of the observer. */
        ecs_filter_desc_t filter_desc = desc->filter;
        filter_desc.name = desc->entity.name;
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
                observer->world, observer->observable, observer);
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
