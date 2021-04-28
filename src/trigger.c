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
void register_id_trigger(
    ecs_map_t *set,
    ecs_trigger_t *trigger)
{
    ecs_trigger_t **t = ecs_map_ensure(set, ecs_trigger_t*, trigger->id);
    ecs_assert(t != NULL, ECS_INTERNAL_ERROR, NULL);
    *t = trigger;
}

static
ecs_map_t* unregister_id_trigger(
    ecs_map_t *set,
    ecs_trigger_t *trigger)
{
    ecs_map_remove(set, trigger->id);

    if (!ecs_map_count(set)) {
        ecs_map_free(set);
        return NULL;
    }

    return set;
}

static
void register_trigger(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_trigger_t *trigger)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(trigger != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_ensure(triggers, 
        ecs_id_trigger_t, id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        if (trigger->events[i] == EcsOnAdd) {
            ecs_map_t *set = idt->on_add_triggers;
            if (!set) {
                set = idt->on_add_triggers = ecs_map_new(ecs_trigger_t*, 1);

                // First OnAdd trigger, send table notification
                ecs_notify_tables(world, id, &(ecs_table_event_t){
                    .kind = EcsTableTriggerMatch,
                    .event = EcsOnAdd
                });
            }
            register_id_trigger(set, trigger);
        } else
        if (trigger->events[i] == EcsOnRemove) {
            ecs_map_t *set = idt->on_remove_triggers;
            if (!set) {
                set = idt->on_remove_triggers = ecs_map_new(ecs_trigger_t*, 1);

                // First OnRemove trigger, send table notification
                ecs_notify_tables(world, id, &(ecs_table_event_t){
                    .kind = EcsTableTriggerMatch,
                    .event = EcsOnRemove
                });
            }

            register_id_trigger(set, trigger);            
        }
    }
}

static
void unregister_trigger(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(trigger != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_get(
        triggers, ecs_id_trigger_t, trigger->id);
    if (!idt) {
        return;
    }

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        if (trigger->events[i] == EcsOnAdd) {
            ecs_map_t *set = idt->on_add_triggers;
            if (!set) {
                return;
            }
            idt->on_add_triggers = unregister_id_trigger(set, trigger);
        } else
        if (trigger->events[i] == EcsOnRemove) {
            ecs_map_t *set = idt->on_remove_triggers;
            if (!set) {
                return;
            }
            idt->on_remove_triggers = unregister_id_trigger(set, trigger);
        }          
    }  
}

ecs_map_t* ecs_triggers_get(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_get(triggers, ecs_id_trigger_t, id);
    if (!idt) {
        return NULL;
    }

    switch(event) {
    case EcsOnAdd:
        if (idt->on_add_triggers && ecs_map_count(idt->on_add_triggers)) {
            return idt->on_add_triggers;
        }
        break;
    case EcsOnRemove:
        if (idt->on_remove_triggers && ecs_map_count(idt->on_remove_triggers)) {
            return idt->on_remove_triggers;
        }
        break;
    }

    return NULL;
}

static
void notify_trigger_set(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t event,
    const ecs_map_t *triggers,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    if (!triggers) {
        return;
    }

    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), 
        ECS_INTERNAL_ERROR, NULL);
    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    int32_t index = ecs_type_index_of(table->type, id);
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    index ++;

    ecs_entity_t ids[1] = { id };
    int32_t columns[1] = { index };

    /* If there is no data, ensure that system won't try to get it */
    if (table->column_count < index) {
        columns[0] = 0;
    } else {
        ecs_column_t *column = &data->columns[index - 1];
        if (!column->size) {
            columns[0] = 0;
        }
    }

    ecs_type_t types[1] = { ecs_type_from_id(world, id) };

    ecs_iter_table_t table_data = {
        .table = table,
        .columns = columns,
        .components = ids,
        .types = types
    };

    ecs_iter_t it = {
        .world = world,
        .event = event,
        .table = &table_data,
        .table_count = 1,
        .inactive_table_count = 0,
        .column_count = 1,
        .table_columns = data->columns,
        .entities = entities,
        .offset = row,
        .count = count,
    }; 

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        it.system = t->entity;
        it.param = t->ctx;
        t->action(&it);                   
    }
}

void ecs_triggers_notify(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    notify_trigger_set(world, id, event,
        ecs_triggers_get(world, id, event), 
            table, data, row, count);

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t pred = ECS_PAIR_RELATION(id);
        ecs_entity_t obj = ECS_PAIR_OBJECT(id);

        notify_trigger_set(world, id, event,
            ecs_triggers_get(world, ecs_pair(pred, EcsWildcard), event), 
                table, data, row, count);

        notify_trigger_set(world, id, event, 
            ecs_triggers_get(world, ecs_pair(EcsWildcard, obj), event), 
                table, data, row, count);

        notify_trigger_set(world, id, event, 
            ecs_triggers_get(world, ecs_pair(EcsWildcard, EcsWildcard), event), 
                table, data, row, count);                                
    } else {
        notify_trigger_set(world, id, event, 
            ecs_triggers_get(world, EcsWildcard, event), 
                table, data, row, count);
    }
}

ecs_entity_t ecs_trigger_init(
    ecs_world_t *world,
    const ecs_trigger_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);

    char *name = NULL;
    const char *expr = desc->expr;

    /* If entity is provided, create it */
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);

    /* Something went wrong with the construction of the entity */
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);
    name = ecs_get_fullpath(world, entity);

    ecs_term_t term;
    if (expr) {
#ifdef FLECS_PARSER
        const char *ptr = ecs_parse_term(world, name, expr, expr, &term);
        if (!ptr) {
            goto error;
        }

        if (!ecs_term_is_set(&term)) {
            ecs_parser_error(name, expr, 0, "invalid empty trigger expression");
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
        term = desc->term;
    }

    if (ecs_term_finalize(world, name, expr, &term)) {
        goto error;
    }

    /* Currently triggers are not supported for specific entities */
    ecs_assert(term.args[0].entity == EcsThis, ECS_UNSUPPORTED, NULL);

    ecs_trigger_t *trigger = ecs_sparse_add(world->triggers, ecs_trigger_t);
    trigger->term = term;
    trigger->action = desc->callback;
    trigger->ctx = desc->ctx;
    trigger->event_count = count_events(desc->events);
    ecs_os_memcpy(trigger->events, desc->events, 
        trigger->event_count * ECS_SIZEOF(ecs_entity_t));
    trigger->id = ecs_sparse_last_id(world->triggers);
    trigger->entity = entity;

    ecs_set(world, entity, EcsTrigger, {trigger});

    /* Trigger must have at least one event */
    ecs_assert(trigger->event_count != 0, ECS_INVALID_PARAMETER, NULL);

    register_trigger(world, trigger->term.id, trigger);

    ecs_term_fini(&term);

    ecs_os_free(name);
    return entity;
error:
    ecs_os_free(name);
    return 0;
}

void ecs_trigger_fini(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    unregister_trigger(world, trigger);
    ecs_term_fini(&trigger->term);
    ecs_sparse_remove(world->triggers, trigger->id);
}
