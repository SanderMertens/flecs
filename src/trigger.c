#include "private_api.h"

static
uint32_t get_event_mask(
    const ecs_entity_t *event_ids)
{
    uint32_t result = 0;
    int32_t i;

    for (i = 0; i < ECS_FILTER_DESC_EVENT_COUNT_MAX; i ++) {
        if (!event_ids[i]) {
            break;
        }

        switch(event_ids[i]) {
        case EcsOnAdd:
            result |= EcsEventAdd;
            break;
        case EcsOnRemove:
            result |= EcsEventRemove;
            break;
        default:
            /* Trigger not available for event kind */
            ecs_abort(ECS_UNSUPPORTED, NULL);
            break;
        }
    }

    return result;
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
    ecs_trigger_t *trigger)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(trigger != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_ensure(triggers, 
        ecs_id_trigger_t, trigger->term.id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

    if (trigger->event_mask & EcsEventAdd) {
        ecs_map_t *set = idt->on_add_triggers;
        if (!set) {
            set = idt->on_add_triggers = ecs_map_new(ecs_trigger_t*, 1);
        }
        register_id_trigger(set, trigger);
    }

    if (trigger->event_mask & EcsEventRemove) {
        ecs_map_t *set = idt->on_remove_triggers;
        if (!set) {
            set = idt->on_remove_triggers = ecs_map_new(ecs_trigger_t*, 1);
        }
        register_id_trigger(set, trigger);
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

    if (trigger->event_mask & EcsEventAdd) {
        ecs_map_t *set = idt->on_add_triggers;
        if (!set) {
            return;
        }
        idt->on_add_triggers = unregister_id_trigger(set, trigger);
    }

    if (trigger->event_mask & EcsEventRemove) {
        ecs_map_t *set = idt->on_remove_triggers;
        if (!set) {
            return;
        }
        idt->on_remove_triggers = unregister_id_trigger(set, trigger);
    }    
}

ecs_trigger_t* ecs_trigger_init(
    ecs_world_t *world,
    const ecs_trigger_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);

    const char *name = desc->name;
    const char *expr = desc->expr;

    ecs_term_t term;
    if (expr) {
#ifdef FLECS_PARSER
        const char *ptr = ecs_parse_term(world, name, expr, expr, &term);
        if (!ptr) {
            return NULL;
        }

        if (!ecs_term_is_set(&term)) {
            ecs_parser_error(name, expr, 0, "invalid empty trigger expression");
            return NULL;
        }

        if (ptr[0]) {
            ecs_parser_error(name, expr, 0, 
                "too many terms in trigger expression (expected 1)");
            return NULL;
        }
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    } else {
        term = desc->term;
    }

    if (ecs_term_finalize(world, name, expr, &term)) {
        return NULL;
    }

    /* Currently triggers are not supported for specific entities */
    ecs_assert(term.args[0].entity == EcsThis, ECS_UNSUPPORTED, NULL);

    ecs_trigger_t *trigger = ecs_sparse_add(world->triggers, ecs_trigger_t);
    trigger->world = world;
    trigger->term = term;
    trigger->callback = desc->callback;
    trigger->ctx = desc->ctx;
    trigger->event_mask = get_event_mask(desc->events);
    trigger->id = ecs_sparse_last_id(world->triggers);

    register_trigger(world, trigger);

    return trigger;
}

void ecs_trigger_fini(
    ecs_trigger_t *trigger)
{
    unregister_trigger(trigger->world, trigger);
    ecs_term_fini(&trigger->term);
    ecs_sparse_remove(trigger->world->triggers, trigger->id);
}
