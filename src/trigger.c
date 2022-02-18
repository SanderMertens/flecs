#include "private_api.h"
#include <stddef.h>

static
int32_t count_events(
    const ecs_entity_t *events) 
{
    int32_t i;

    for (i = 0; i < ECS_TRIGGER_DESC_EVENT_COUNT_MAX; i ++) {
        if (!events[i]) {
            break;
        }
    }

    return i;
}

static
ecs_entity_t get_actual_event(
    ecs_trigger_t *trigger, 
    ecs_entity_t event)
{
    /* If operator is Not, reverse the event */
    if (trigger->term.oper == EcsNot) {
        if (event == EcsOnAdd) {
            event = EcsOnRemove;
        } else if (event == EcsOnRemove) {
            event = EcsOnAdd;
        }
    }

    return event;
}

static
void unregister_event_trigger(
    ecs_event_record_t *evt,
    ecs_id_t id)
{
    if (ecs_map_remove(evt->event_ids, id) == 0) {
        ecs_map_free(evt->event_ids);
        evt->event_ids = NULL;
    }
}

static
void inc_trigger_count(
    ecs_world_t *world,
    ecs_entity_t event,
    ecs_event_record_t *evt,
    ecs_id_t id,
    int32_t value)
{
    ecs_event_id_record_t *idt = ecs_map_ensure(
        evt->event_ids, ecs_event_id_record_t, id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t result = idt->trigger_count += value;
    if (result == 1) {
        /* Notify framework that there are triggers for the event/id. This 
         * allows parts of the code to skip event evaluation early */
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableTriggersForId,
            .event = event
        });
    } else if (result == 0) {
        /* Ditto, but the reverse */
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableNoTriggersForId,
            .event = event
        });

        /* Remove admin for id for event */
        if (!idt->triggers && !idt->set_triggers) {
            unregister_event_trigger(evt, id);
        }
    }
}

static
void register_trigger_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_trigger_t *trigger,
    ecs_id_t id,
    size_t triggers_offset)
{
    ecs_sparse_t *events = observable->events;
    ecs_assert(events != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t term_id = trigger->term.id;

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        ecs_entity_t event = get_actual_event(trigger, trigger->events[i]);

        /* Get triggers for event */
        ecs_event_record_t *evt = flecs_sparse_ensure(
            events, ecs_event_record_t, event);
        ecs_assert(evt != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!evt->event_ids) {
            evt->event_ids = ecs_map_new(ecs_event_id_record_t, 1);
        }

        /* Get triggers for (component) id for event */
        ecs_event_id_record_t *idt = ecs_map_ensure(
            evt->event_ids, ecs_event_id_record_t, id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t **triggers = ECS_OFFSET(idt, triggers_offset);
        if (!triggers[0]) {
            triggers[0] = ecs_map_new(ecs_trigger_t*, 1);
        }

        ecs_map_ensure(triggers[0], ecs_trigger_t*, trigger->id)[0] = trigger;

        inc_trigger_count(world, event, evt, term_id, 1);
        if (term_id != id) {
            inc_trigger_count(world, event, evt, id, 1);
        }
    }
}

static
void register_trigger(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_trigger_t *trigger)
{
    ecs_term_t *term = &trigger->term;

    if (term->subj.set.mask & EcsSelf) {
        if (term->subj.entity == EcsThis) {
            register_trigger_for_id(world, observable, trigger, term->id, 
                offsetof(ecs_event_id_record_t, triggers));
        } else {
            register_trigger_for_id(world, observable, trigger, term->id,
                offsetof(ecs_event_id_record_t, entity_triggers));
        }
    }

    if (trigger->term.subj.set.mask & EcsSuperSet) {
        ecs_id_t pair = ecs_pair(term->subj.set.relation, EcsWildcard);
        register_trigger_for_id(world, observable, trigger, pair,
            offsetof(ecs_event_id_record_t, set_triggers));
    }

    if (ECS_HAS_ROLE(term->id, SWITCH)) {
        ecs_entity_t sw = term->id & ECS_COMPONENT_MASK;
        ecs_id_t sw_case = ecs_case(sw, EcsWildcard);
        register_trigger_for_id(world, observable, trigger, sw_case, 
            offsetof(ecs_event_id_record_t, triggers));
    }

    if (ECS_HAS_ROLE(term->id, CASE)) {
        ecs_entity_t sw = ECS_PAIR_FIRST(term->id);
        register_trigger_for_id(world, observable, trigger, ECS_SWITCH | sw, 
            offsetof(ecs_event_id_record_t, triggers));
    }
}

static
void unregister_trigger_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_trigger_t *trigger,
    ecs_id_t id,
    size_t triggers_offset)
{
    ecs_sparse_t *events = observable->events;
    ecs_assert(events != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t term_id = trigger->term.id;

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        ecs_entity_t event = get_actual_event(trigger, trigger->events[i]);

        /* Get triggers for event */
        ecs_event_record_t *evt = flecs_sparse_get(
            events, ecs_event_record_t, event);
        ecs_assert(evt != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get triggers for (component) id */
        ecs_event_id_record_t *idt = ecs_map_get(
            evt->event_ids, ecs_event_id_record_t, id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t **id_triggers = ECS_OFFSET(idt, triggers_offset);

        if (ecs_map_remove(id_triggers[0], trigger->id) == 0) {
            ecs_map_free(id_triggers[0]);
            id_triggers[0] = NULL;
        }

        inc_trigger_count(world, event, evt, term_id, -1);

        if (id != term_id) {
            /* Id is different from term_id in case of a set trigger. If they're
             * the same, inc_trigger_count could already have done cleanup */
            if (!idt->triggers && !idt->set_triggers && !idt->trigger_count) {
                unregister_event_trigger(evt, id);
            }

            inc_trigger_count(world, event, evt, id, -1);
        }
    }
}

static
void unregister_trigger(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_trigger_t *trigger)
{    
    ecs_term_t *term = &trigger->term;

    if (term->subj.set.mask & EcsSelf) {
        if (term->subj.entity == EcsThis) {
            unregister_trigger_for_id(world, observable, trigger, term->id, 
                offsetof(ecs_event_id_record_t, triggers));
        } else {
            unregister_trigger_for_id(world, observable, trigger, term->id,
                offsetof(ecs_event_id_record_t, entity_triggers));
        }
    }

    if (term->subj.set.mask & EcsSuperSet) {
        ecs_id_t pair = ecs_pair(term->subj.set.relation, EcsWildcard);
        unregister_trigger_for_id(world, observable, trigger, pair,
            offsetof(ecs_event_id_record_t, set_triggers));
    }

    if (ECS_HAS_ROLE(term->id, SWITCH)) {
        ecs_entity_t sw = term->id & ECS_COMPONENT_MASK;
        ecs_id_t sw_case = ecs_case(sw, EcsWildcard);
        unregister_trigger_for_id(world, observable, trigger, sw_case, 
            offsetof(ecs_event_id_record_t, triggers));
    }

    if (ECS_HAS_ROLE(term->id, CASE)) {
        ecs_entity_t sw = ECS_PAIR_FIRST(term->id);
        unregister_trigger_for_id(world, observable, trigger, ECS_SWITCH | sw, 
            offsetof(ecs_event_id_record_t, triggers));
    }
}

static
ecs_map_t* get_triggers_for_event(
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
        return evt->event_ids;
    }

error:
    return NULL;
}

static
ecs_event_id_record_t* get_triggers_for_id(
    const ecs_map_t *evt,
    ecs_id_t id)
{
    return ecs_map_get(evt, ecs_event_id_record_t, id);
}

bool flecs_check_triggers_for_event(
    const ecs_poly_t *object,
    ecs_id_t id,
    ecs_entity_t event)
{    
    ecs_observable_t *observable = ecs_get_observable(object);
    const ecs_map_t *evt = get_triggers_for_event(observable, event);
    if (!evt) {
        return false;
    }

    ecs_event_id_record_t *edr = get_triggers_for_id(evt, id);
    if (edr) {
        return edr->trigger_count != 0;
    } else {
        return false;
    }
}

static
void init_iter(
    ecs_iter_t *it,
    bool *iter_set)
{
    ecs_assert(it != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (*iter_set) {
        return;
    }

    if (it->table_only) {
        it->ids = it->priv.cache.ids;
        it->ids[0] = it->event_id;
        return;
    }

    flecs_iter_init(it);

    *iter_set = true;

    it->ids[0] = it->event_id;

    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!it->count || it->offset < ecs_table_count(it->table), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert((it->offset + it->count) <= ecs_table_count(it->table), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t index = ecs_search_relation(it->world, it->table, 0, 
        it->event_id, EcsIsA, 0, 0, it->subjects, NULL, NULL);
    
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

    it->terms = &term;
    it->term_count = 1;
    flecs_iter_populate_data(it->world, it, it->table, it->offset, 
        it->count, it->ptrs, it->sizes);
}

static
bool ignore_trigger(
    ecs_world_t *world,
    ecs_trigger_t *t,
    ecs_table_t *table)
{
    int32_t *last_event_id = t->last_event_id;
    if (last_event_id && last_event_id[0] == world->event_id) {
        return true;
    }

    if (!table) {
        return false;
    }

    if (!t->match_prefab && (table->flags & EcsTableIsPrefab)) {
        return true;
    }
    if (!t->match_disabled && (table->flags & EcsTableIsDisabled)) {
        return true;
    }
    
    return false;
}

static
void notify_self_triggers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *triggers)
{
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        if (ignore_trigger(world, t, it->table)) {
            continue;
        }

        it->is_filter = t->term.inout == EcsInOutFilter;
        it->system = t->entity;
        it->self = t->self;
        it->ctx = t->ctx;
        it->binding_ctx = t->binding_ctx;
        it->term_index = t->term.index;
        it->terms = &t->term;

        t->callback(it);
    }
}

static
void notify_entity_triggers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *triggers)
{
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    if (it->table_only) {
        return;
    }

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    int32_t offset = it->offset, count = it->count;
    ecs_entity_t *entities = it->entities;
    
    ecs_entity_t dummy = 0;
    it->entities = &dummy;

    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        if (ignore_trigger(world, t, it->table)) {
            continue;
        }

        int32_t i, entity_count = it->count;
        for (i = 0; i < entity_count; i ++) {
            if (entities[i] != t->term.subj.entity) {
                continue;
            }

            it->is_filter = t->term.inout == EcsInOutFilter;
            it->system = t->entity;
            it->self = t->self;
            it->ctx = t->ctx;
            it->binding_ctx = t->binding_ctx;
            it->term_index = t->term.index;
            it->terms = &t->term;
            it->offset = i;
            it->count = 1;
            it->subjects[0] = entities[i];

            t->callback(it);
        }
    }

    it->offset = offset;
    it->count = count;
    it->entities = entities;
    it->subjects[0] = 0;
}

static
void notify_set_base_triggers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *triggers)
{
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t event_id = it->event_id;
    ecs_entity_t rel = ECS_PAIR_FIRST(event_id);
    ecs_entity_t obj = ecs_pair_second(world, event_id);
    ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *obj_table = ecs_get_table(world, obj);
    if (!obj_table) {
        return;
    }

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        if (ignore_trigger(world, t, it->table)) {
            continue;
        }

        ecs_term_t *term = &t->term;
        ecs_id_t id = term->id;
        int32_t column = ecs_search_relation(world, obj_table, 0, id, rel, 
            0, 0, it->subjects, it->ids, 0);
        
        bool result = column != -1;
        if (term->oper == EcsNot) {
            result = !result;
        }
        if (!result) {
            continue;
        }

        if (!term->subj.set.min_depth && flecs_get_table_record(
            world, it->table, id) != NULL) 
        {
            continue;
        }

        if (!it->table_only) {
            if (!it->subjects[0]) {
                it->subjects[0] = obj;
            }

            if (column != -1) {
                it->columns[0] = -(column + 1);
            } else {
                it->columns[0] = 0;
            }
        }

        it->is_filter = t->term.inout == EcsInOutFilter;
        it->event_id = t->term.id;
        it->system = t->entity;
        it->self = t->self;
        it->ctx = t->ctx;
        it->binding_ctx = t->binding_ctx;
        it->term_index = t->term.index;
        it->terms = &t->term;
        
        t->callback(it);
    }
}

static
void notify_set_triggers(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_map_t *triggers)
{
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(it->count != 0, ECS_INTERNAL_ERROR, NULL);

    if (it->table_only) {
        return;
    }

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        if (!ecs_id_match(it->event_id, t->term.id)) {
            continue;
        }

        if (ignore_trigger(world, t, it->table)) {
            continue;
        }

        ecs_entity_t subj = it->entities[0];
        int32_t i, count = it->count;
        ecs_entity_t term_subj = t->term.subj.entity;

        /* If trigger is for a specific entity, make sure it is in the table
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

            /* If the entity matches, trigger for no other entities */
            it->entities[0] = 0;
            it->count = 1;
        }

        if (flecs_term_match_table(world, &t->term, it->table, it->type, 
            it->ids, it->columns, it->subjects, NULL, true))
        {
            if (!it->subjects[0]) {
                /* Do not match owned components */
                continue;
            }

            it->is_filter = t->term.inout == EcsInOutFilter;
            it->system = t->entity;
            it->self = t->self;
            it->ctx = t->ctx;
            it->binding_ctx = t->binding_ctx;
            it->term_index = t->term.index;
            it->terms = &t->term;

            /* Triggers for supersets can be instanced */
            if (it->count == 1 || t->instanced || it->is_filter || !it->sizes[0]) {
                it->is_instanced = t->instanced;
                t->callback(it);
                it->is_instanced = false;
            } else {
                ecs_entity_t *entities = it->entities;
                it->count = 1;
                for (i = 0; i < count; i ++) {
                    it->entities = &entities[i];
                    t->callback(it);
                }
                it->entities = entities;
            }
        }

        it->entities[0] = subj;
        it->count = count;            
    }
}

static
void notify_triggers_for_id(
    ecs_world_t *world,
    const ecs_map_t *evt,
    ecs_id_t event_id,
    ecs_iter_t *it,
    bool *iter_set)
{
    const ecs_event_id_record_t *idt = get_triggers_for_id(evt, event_id);
    if (!idt) {
        return;
    }

    ecs_map_t *triggers;
    if ((triggers = idt->triggers)) {
        init_iter(it, iter_set);
        notify_self_triggers(world, it, triggers);
    }
    if ((triggers = idt->entity_triggers)) {
        init_iter(it, iter_set);
        notify_entity_triggers(world, it, triggers);
    }
    if ((triggers = idt->set_triggers)) {
        init_iter(it, iter_set);
        notify_set_base_triggers(world, it, triggers);
    }
}

static
void notify_set_triggers_for_id(
    ecs_world_t *world,
    const ecs_map_t *evt,
    ecs_iter_t *it,
    bool *iter_set,
    ecs_id_t set_id)
{
    const ecs_event_id_record_t *idt = get_triggers_for_id(evt, set_id);
    if (idt && idt->set_triggers) {
        init_iter(it, iter_set);
        notify_set_triggers(world, it, idt->set_triggers);
    }
}

static
void trigger_yield_existing(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    ecs_iter_action_t callback = trigger->callback;

    /* If yield existing is enabled, trigger for each thing that matches
     * the event, if the event is iterable. */
    int i, count = trigger->event_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t evt = trigger->events[i];
        const EcsIterable *iterable = ecs_get(world, evt, EcsIterable);
        if (!iterable) {
            continue;
        }

        ecs_iter_t it;
        iterable->init(world, world, &it, &trigger->term);
        it.system = trigger->entity;
        it.ctx = trigger->ctx;
        it.binding_ctx = trigger->binding_ctx;
        it.event = evt;

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            callback(&it);
        }
    }
}

void flecs_triggers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    ecs_ids_t *ids,
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
        const ecs_map_t *evt = get_triggers_for_event(observable, event);
        if (!evt) {
            continue;
        }

        it->event = event;

        for (i = 0; i < ids_count; i ++) {
            ecs_id_t id = ids_array[i];
            ecs_entity_t role = id & ECS_ROLE_MASK;
            bool iter_set = false;

            it->event_id = id;

            notify_triggers_for_id(world, evt, id, it, &iter_set);

            if (role == ECS_PAIR || role == ECS_CASE) {
                ecs_entity_t pred = ECS_PAIR_FIRST(id);
                ecs_entity_t obj = ECS_PAIR_SECOND(id);

                ecs_id_t tid = role | ecs_entity_t_comb(EcsWildcard, pred);
                notify_triggers_for_id(world, evt, tid, it, &iter_set);

                tid = role | ecs_entity_t_comb(obj, EcsWildcard);
                notify_triggers_for_id(world, evt, tid, it, &iter_set);
                
                tid = role | ecs_entity_t_comb(EcsWildcard, EcsWildcard);
                notify_triggers_for_id(world, evt, tid, it, &iter_set);
            } else {
                notify_triggers_for_id(world, evt, EcsWildcard, it, &iter_set);
            }
        }
    }
}

void flecs_set_triggers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    ecs_ids_t *ids,
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
        const ecs_map_t *evt = get_triggers_for_event(observable, event);
        if (!evt) {
            continue;
        }

        it->event = event;

        for (i = 0; i < ids_count; i ++) {
            ecs_id_t id = ids_array[i];
            bool iter_set = false;

            it->event_id = id;

            notify_set_triggers_for_id(world, evt, it, &iter_set, set_id);
        }
    }
}

ecs_entity_t ecs_trigger_init(
    ecs_world_t *world,
    const ecs_trigger_desc_t *desc)
{
    char *name = NULL;
    
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    const char *expr = desc->expr;
    ecs_trigger_t *trigger = NULL;
    
    ecs_observable_t *observable = desc->observable;
    if (!observable) {
        observable = ecs_get_observable(world);
    }

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);
    if (!existing && !desc->entity.name) {
        ecs_add_pair(world, entity, EcsChildOf, EcsFlecsHidden);
    }

    bool added = false;
    EcsTrigger *comp = ecs_get_mut(world, entity, EcsTrigger, &added);
    if (added) {
        ecs_check(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);
        
        /* Something went wrong with the construction of the entity */
        ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
        name = ecs_get_fullpath(world, entity);

        ecs_term_t term;
        if (expr) {
    #ifdef FLECS_PARSER
            const char *ptr = ecs_parse_term(world, name, expr, expr, &term);
            if (!ptr) {
                goto error;
            }

            if (!ecs_term_is_initialized(&term)) {
                ecs_parser_error(
                    name, expr, 0, "invalid empty trigger expression");
                goto error;
            }

            if (ptr[0]) {
                ecs_parser_error(name, expr, 0, 
                    "too many terms in trigger expression (expected 1)");
                goto error;
            }
    #else
            ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
    #endif
        } else {
            term = ecs_term_copy(&desc->term);
        }

        if (ecs_term_finalize(world, name, &term)) {
            ecs_term_fini(&term);
            goto error;
        }

        trigger = flecs_sparse_add(world->triggers, ecs_trigger_t);
        trigger->id = flecs_sparse_last_id(world->triggers);

        trigger->term = ecs_term_move(&term);
        trigger->callback = desc->callback;
        trigger->ctx = desc->ctx;
        trigger->binding_ctx = desc->binding_ctx;
        trigger->ctx_free = desc->ctx_free;
        trigger->binding_ctx_free = desc->binding_ctx_free;
        trigger->event_count = count_events(desc->events);
        ecs_os_memcpy(trigger->events, desc->events, 
            trigger->event_count * ECS_SIZEOF(ecs_entity_t));
        trigger->entity = entity;
        trigger->self = desc->self;
        trigger->observable = observable;
        trigger->match_prefab = desc->match_prefab;
        trigger->match_disabled = desc->match_disabled;
        trigger->instanced = desc->instanced;
        trigger->last_event_id = desc->last_event_id;

        if (trigger->term.id == EcsPrefab) {
            trigger->match_prefab = true;
        }
        if (trigger->term.id == EcsDisabled) {
            trigger->match_disabled = true;
        }

        comp->trigger = trigger;

        /* Trigger must have at least one event */
        ecs_check(trigger->event_count != 0, ECS_INVALID_PARAMETER, NULL);

        register_trigger(world, observable, trigger);

        ecs_term_fini(&term);

        if (desc->entity.name) {
            ecs_trace("#[green]trigger#[reset] %s created", 
                ecs_get_name(world, entity));
        }

        if (desc->yield_existing) {
            trigger_yield_existing(world, trigger);
        }
    } else {
        ecs_assert(comp->trigger != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If existing entity handle was provided, override existing params */
        if (existing) {
            if (desc->callback) {
                ((ecs_trigger_t*)comp->trigger)->callback = desc->callback;
            }
            if (desc->ctx) {
                ((ecs_trigger_t*)comp->trigger)->ctx = desc->ctx;
            }
            if (desc->binding_ctx) {
                ((ecs_trigger_t*)comp->trigger)->binding_ctx = desc->binding_ctx;
            }
        }
    }

    ecs_os_free(name);
    return entity;
error:
    ecs_os_free(name);
    ecs_delete(world, entity);
    return 0;
}

void* ecs_get_trigger_ctx(
    const ecs_world_t *world,
    ecs_entity_t trigger)
{
    const EcsTrigger *t = ecs_get(world, trigger, EcsTrigger);
    if (t) {
        return t->trigger->ctx;
    } else {
        return NULL;
    }     
}

void* ecs_get_trigger_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t trigger)
{
    const EcsTrigger *t = ecs_get(world, trigger, EcsTrigger);
    if (t) {
        return t->trigger->binding_ctx;
    } else {
        return NULL;
    }      
}

void flecs_trigger_fini(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{    
    unregister_trigger(world, trigger->observable, trigger);
    ecs_term_fini(&trigger->term);

    if (trigger->ctx_free) {
        trigger->ctx_free(trigger->ctx);
    }

    if (trigger->binding_ctx_free) {
        trigger->binding_ctx_free(trigger->binding_ctx);
    }

    flecs_sparse_remove(world->triggers, trigger->id);
}
