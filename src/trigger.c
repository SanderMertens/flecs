#include "private_api.h"

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
void register_trigger_for_id(
    ecs_world_t *world,
    ecs_observable_t *observable,
    ecs_trigger_t *trigger,
    ecs_id_t id,
    bool register_for_set)
{
    ecs_sparse_t *triggers = observable->triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        ecs_entity_t event = get_actual_event(trigger, trigger->events[i]);

        /* Get triggers for event */
        ecs_event_triggers_t *evt = flecs_sparse_ensure(
            triggers, ecs_event_triggers_t, event);
        ecs_assert(evt != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!evt->triggers) {
            evt->triggers = ecs_map_new(ecs_id_triggers_t, 1);
        }

        /* Get triggers for (component) id */
        ecs_id_triggers_t *idt = ecs_map_ensure(
            evt->triggers, ecs_id_triggers_t, id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *id_triggers = NULL;

        if (!register_for_set) {
            if (!(id_triggers = idt->triggers)) {
                id_triggers = idt->triggers = ecs_map_new(ecs_trigger_t*, 1);
            }
        } else {
            if (!(id_triggers = idt->set_triggers)) {
                id_triggers = idt->set_triggers = 
                    ecs_map_new(ecs_trigger_t*, 1);
            }
        }

        ecs_trigger_t **elem = ecs_map_ensure(
            id_triggers, ecs_trigger_t*, trigger->id);
        *elem = trigger;

        // First trigger of its kind, send table notification
        flecs_notify_tables(world, id, &(ecs_table_event_t){
            .kind = EcsTableTriggerMatch,
            .event = trigger->events[i]
        });
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
        register_trigger_for_id(world, observable, trigger, term->id, false);
    }
    if (trigger->term.subj.set.mask & EcsSuperSet) {
        ecs_id_t pair = ecs_pair(term->subj.set.relation, EcsWildcard);
        register_trigger_for_id(world, observable, trigger, pair, true);
    }
}

static
void unregister_trigger_for_id(
    ecs_observable_t *observable,
    ecs_trigger_t *trigger,
    ecs_id_t id,
    bool unregister_for_set)
{
    ecs_sparse_t *triggers = observable->triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        ecs_entity_t event = get_actual_event(trigger, trigger->events[i]);

        /* Get triggers for event */
        ecs_event_triggers_t *evt = flecs_sparse_get(
            triggers, ecs_event_triggers_t, event);
        ecs_assert(evt != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Get triggers for (component) id */
        ecs_id_triggers_t *idt = ecs_map_get(
            evt->triggers, ecs_id_triggers_t, id);
        ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_t *id_triggers;

        if (unregister_for_set) {
            id_triggers = idt->set_triggers;
        } else {
            id_triggers = idt->triggers;
        }

        ecs_map_remove(id_triggers, trigger->id);

        if (!ecs_map_count(id_triggers)) {
            ecs_map_free(id_triggers);

            if (unregister_for_set) {
                idt->set_triggers = NULL;
            } else {
                idt->triggers = NULL;
            }

            if (!idt->triggers && !idt->set_triggers) {
                ecs_map_remove(evt->triggers, id);
                if (!ecs_map_count(evt->triggers)) {
                    ecs_map_free(evt->triggers);
                    evt->triggers = NULL;
                }
            }
        }
    }
}

static
void unregister_trigger(
    ecs_observable_t *observable,
    ecs_trigger_t *trigger)
{    
    ecs_term_t *term = &trigger->term;
    if (term->subj.set.mask & EcsSelf) {
        unregister_trigger_for_id(observable, trigger, term->id, false);
    } else {
        ecs_id_t pair = ecs_pair(term->subj.set.relation, EcsWildcard);
        unregister_trigger_for_id(observable, trigger, pair, true);
    }
}

static
ecs_map_t* get_triggers_for_event(
    const ecs_poly_t *object,
    ecs_entity_t event)
{
    ecs_assert(object != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(event != 0, ECS_INTERNAL_ERROR, NULL);

    /* Get triggers for event */
    ecs_observable_t *observable = ecs_get_observable(object);
    ecs_check(observable != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_sparse_t *triggers = observable->triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    const ecs_event_triggers_t *evt = flecs_sparse_get(
        triggers, ecs_event_triggers_t, event);
    
    if (evt) {
        return evt->triggers;
    }

error:
    return NULL;
}

static
ecs_id_triggers_t* get_triggers_for_id(
    const ecs_map_t *evt,
    ecs_id_t id)
{
    return ecs_map_get(evt, ecs_id_triggers_t, id);
}

ecs_id_triggers_t* flecs_triggers_for_id(
    const ecs_poly_t *object,
    ecs_id_t id,
    ecs_entity_t event)
{
    const ecs_map_t *evt = get_triggers_for_event(object, event);
    if (!evt) {
        return NULL;
    }

    return get_triggers_for_id(evt, id);
}

static
void init_iter(
    ecs_iter_t *it,
    ecs_entity_t *entity,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    bool *iter_set)
{
    ecs_assert(it != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (*iter_set) {
        return;
    }

    flecs_iter_init(it);

    *iter_set = true;

    it->ids[0] = it->event_id;

    if (count) {
        if (table) {
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(!it->world->is_readonly, ECS_INTERNAL_ERROR, NULL);
            ecs_data_t *data = &table->storage;
            ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);        
            ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(row < ecs_vector_count(data->entities), 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert((row + count) <= ecs_vector_count(data->entities), 
                ECS_INTERNAL_ERROR, NULL);

            it->entities = &entities[row];

            ecs_entity_t subject = 0;
            int32_t index = ecs_type_match(it->world, table, table->type, 0, 
                it->event_id, EcsIsA, 0, 0, &subject, NULL, NULL);

            ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
            int32_t storage_index = ecs_table_type_to_storage_index(
                table, index);
            
            index ++;
            it->columns[0] = index;

            if (storage_index == -1) {
                it->columns[0] = 0;
            }

            if (!it->is_filter) {
                it->sizes[0] = 0;
                if (!subject && it->columns[0] && data && data->columns) {
                    ecs_column_t *col = &data->columns[storage_index];
                    it->ptrs[0] = ecs_vector_get_t(
                        col->data, col->size, col->alignment, row);
                    it->sizes[0] = col->size;
                } else if (subject) {
                    it->ptrs[0] = (void*)ecs_get_id(
                        it->world, subject, it->event_id);
                    ecs_entity_t e = ecs_get_typeid(it->world, it->event_id);
                    const EcsComponent *comp = ecs_get(
                        it->world, e, EcsComponent);
                    if (comp) {
                        it->sizes[0] = comp->size;
                    } else {
                        it->sizes[0] = 0;
                    }
                }
            }

            it->subjects[0] = subject;
        } else {
            it->entities = entity;
        }
    }
}

static
bool ignore_table(
    ecs_trigger_t *t,
    ecs_table_t *table)
{
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
    ecs_iter_t *it,
    const ecs_map_t *triggers)
{
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    void **ptrs = it->ptrs;
    ecs_size_t *sizes = it->sizes;

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        if (ignore_table(t, it->table)) {
            continue;
        }

        bool is_filter = t->term.inout == EcsInOutFilter;

        it->system = t->entity;
        it->self = t->self;
        it->ctx = t->ctx;
        it->binding_ctx = t->binding_ctx;
        it->term_index = t->term.index;
        it->terms = &t->term;
        it->is_filter = is_filter;

        if (is_filter) {
            it->ptrs = NULL;
            it->sizes = NULL;
        }

        t->action(it);

        it->ptrs = ptrs;
        it->sizes = sizes;
    }
}

static
void notify_set_triggers(
    ecs_iter_t *it,
    const ecs_map_t *triggers)
{
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        if (ignore_table(t, it->table)) {
            continue;
        }

        if (flecs_term_match_table(it->world, &t->term, it->table, it->type, 
            it->ids, it->columns, it->subjects, NULL, true))
        {
            if (!it->subjects[0]) {
                /* Do not match owned components */
                continue;
            }

            ecs_entity_t event_id = it->event_id;
            it->event_id = t->term.id;

            it->ids[0] = t->term.id;
            it->system = t->entity;
            it->self = t->self;
            it->ctx = t->ctx;
            it->binding_ctx = t->binding_ctx;
            it->term_index = t->term.index;
            it->terms = &t->term;
            t->action(it);

            it->event_id = event_id;
        }                
    }
}

static
void notify_triggers_for_id(
    const ecs_map_t *evt,
    ecs_id_t event_id,
    ecs_iter_t *it,
    ecs_entity_t *entity,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    bool *iter_set)
{
    const ecs_id_triggers_t *idt = get_triggers_for_id(evt, event_id);
    if (idt) {
        if (idt->triggers) {
            init_iter(it, entity, table, row, count, iter_set);
            notify_self_triggers(it, idt->triggers);
        }
        if (idt->set_triggers) {
            init_iter(it, entity, table, row, count, iter_set);
            notify_set_triggers(it, idt->set_triggers);
        }
    }
}

static
void trigger_yield_existing(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    ecs_iter_action_t callback = trigger->action;

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

        ecs_iter_next_action_t next = it.next;
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        while (next(&it)) {
            callback(&it);
        }
    }
}

void flecs_triggers_notify(
    ecs_world_t *world,
    ecs_poly_t *observable,
    ecs_ids_t *ids,
    ecs_entity_t event,
    ecs_entity_t entity,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    void *param)
{
    if (!ids || !ids->count) {
        return;
    }

    ecs_assert(ids->array != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!observable) {
        observable = world;
    }

    ecs_entity_t trigger_event = event; 

    do {
        const ecs_map_t *evt = get_triggers_for_event(observable, trigger_event);
        if (!evt && trigger_event != EcsWildcard) {
            trigger_event = EcsWildcard;
            continue;
        }

        if (!evt) {
            return;
        }

        ecs_iter_t it = {
            .world = world,
            .event = event,
            .term_count = 1,
            .table = table,
            .type = table ? table->type : NULL,
            .other_table = other_table,
            .offset = row,
            .count = count,
            .param = param
        };

        int32_t i, ids_count = ids->count;
        ecs_id_t *ids_array = ids->array;

        for (i = 0; i < ids_count; i ++) {
            ecs_id_t id = ids_array[i];
            bool iter_set = false;

            it.event_id = id;

            notify_triggers_for_id(
                evt, id, &it, &entity, table, row, count, &iter_set);

            if (ECS_HAS_ROLE(id, PAIR)) {
                ecs_entity_t pred = ECS_PAIR_RELATION(id);
                ecs_entity_t obj = ECS_PAIR_OBJECT(id);

                notify_triggers_for_id(evt, ecs_pair(pred, EcsWildcard), 
                    &it, &entity, table, row, count, &iter_set);

                notify_triggers_for_id(evt, ecs_pair(EcsWildcard, obj), 
                    &it, &entity, table, row, count, &iter_set);

                notify_triggers_for_id(evt, ecs_pair(EcsWildcard, EcsWildcard), 
                    &it, &entity, table, row, count, &iter_set);
            } else {
                notify_triggers_for_id(evt, EcsWildcard, 
                    &it, &entity, table, row, count, &iter_set);
            }
        }

        if (trigger_event == EcsWildcard) {
            break;
        }
        
        trigger_event = EcsWildcard;
    } while (true);
}

ecs_entity_t ecs_trigger_init(
    ecs_world_t *world,
    const ecs_trigger_desc_t *desc)
{
    char *name = NULL;
    
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    const char *expr = desc->expr;
    
    ecs_observable_t *observable = desc->observable;
    if (!observable) {
        observable = ecs_get_observable(world);
    }

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);

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
            goto error;
        }

        /* Currently triggers are not supported for specific entities */
        ecs_check(term.subj.entity == EcsThis, ECS_UNSUPPORTED, NULL);

        ecs_trigger_t *trigger = flecs_sparse_add(world->triggers, ecs_trigger_t);
        trigger->id = flecs_sparse_last_id(world->triggers);
        trigger->term = ecs_term_move(&term);
        trigger->action = desc->callback;
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
                ((ecs_trigger_t*)comp->trigger)->action = desc->callback;
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
    unregister_trigger(trigger->observable, trigger);
    ecs_term_fini(&trigger->term);

    if (trigger->ctx_free) {
        trigger->ctx_free(trigger->ctx);
    }

    if (trigger->binding_ctx_free) {
        trigger->binding_ctx_free(trigger->binding_ctx);
    }

    flecs_sparse_remove(world->triggers, trigger->id);
}
