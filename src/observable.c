#include "private_api.h"

void flecs_observable_init(
    ecs_observable_t *observable)
{
    observable->events = ecs_sparse_new(ecs_event_record_t);
}

void flecs_observable_fini(
    ecs_observable_t *observable)
{
    ecs_sparse_t *triggers = observable->events;
    int32_t i, count = flecs_sparse_count(triggers);

    for (i = 0; i < count; i ++) {
        ecs_event_record_t *et = 
            ecs_sparse_get_dense(triggers, ecs_event_record_t, i);
        ecs_assert(et != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_map_iter_t it = ecs_map_iter(et->event_ids);
        ecs_event_id_record_t *idt;
        while ((idt = ecs_map_next(&it, ecs_event_id_record_t, NULL))) {
            ecs_map_free(idt->triggers);
            ecs_map_free(idt->set_triggers);
        }
        ecs_map_free(et->event_ids);
    }

    flecs_sparse_free(observable->events);
}

static
void notify_subset(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_observable_t *observable,
    ecs_entity_t entity,
    ecs_entity_t event,
    ecs_ids_t *ids)
{
    ecs_id_t pair = ecs_pair(EcsWildcard, entity);
    ecs_table_iter_t idt;
    ecs_id_record_t *idr = flecs_table_iter(world, pair, &idt);
    if (!idr) {
        return;
    }

    for (; idt.cur < idt.end; ++ idt.cur) {
        const ecs_table_record_t *tr = idt.cur;
        ecs_table_t *table = tr->table;
        ecs_id_t id = ecs_vector_get(table->type, ecs_id_t, tr->column)[0];
        ecs_entity_t rel = ECS_PAIR_FIRST(id);

        if (ecs_is_valid(world, rel) && !ecs_has_id(world, rel, EcsAcyclic)) {
            /* Only notify for acyclic relations */
            continue;
        }

        int32_t e, entity_count = ecs_table_count(table);
        it->table = table;
        it->type = table->type;
        it->other_table = NULL;
        it->offset = 0;
        it->count = entity_count;

        /* Treat as new event as this could trigger observers again for
         * different tables. */
        world->event_id ++;

        flecs_set_triggers_notify(it, observable, ids, event,
            ecs_pair(rel, EcsWildcard));

        ecs_entity_t *entities = ecs_vector_first(
            table->storage.entities, ecs_entity_t);
        ecs_record_t **records = ecs_vector_first(
            table->storage.record_ptrs, ecs_record_t*);

        for (e = 0; e < entity_count; e ++) {
            uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(records[e]->row);
            if (flags & ECS_FLAG_OBSERVED_ACYCLIC) {
                /* Only notify for entities that are used in pairs with
                 * acyclic relations */
                notify_subset(world, it, observable, entities[e], event, ids);
            }
        }
    }
}

void ecs_emit(
    ecs_world_t *world,
    ecs_event_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->event != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->event != EcsWildcard, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->ids != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->ids->count != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->observable != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t *ids = desc->ids;
    ecs_entity_t event = desc->event;
    ecs_table_t *table = desc->table;
    int32_t row = desc->offset;
    int32_t i, count = desc->count;
    ecs_entity_t relation = desc->relation;

    if (!count) {
        count = ecs_table_count(table) - row;
    }

    ecs_iter_t it = {
        .world = world,
        .real_world = world,
        .table = table,
        .type = table->type,
        .term_count = 1,
        .other_table = desc->other_table,
        .offset = row,
        .count = count,
        .param = (void*)desc->param,
        .table_only = desc->table_event
    };

    world->event_id ++;

    ecs_observable_t *observable = ecs_get_observable(desc->observable);
    ecs_check(observable != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!desc->relation) {
        flecs_triggers_notify(&it, observable, ids, event);
    } else {
        flecs_set_triggers_notify(&it, observable, ids, event, 
            ecs_pair(relation, EcsWildcard));
    }

    if (count && !desc->table_event) {
        ecs_record_t **recs = ecs_vector_get(
            table->storage.record_ptrs, ecs_record_t*, row);

        for (i = 0; i < count; i ++) {
            ecs_record_t *r = recs[i];
            if (!r) {
                /* If the event is emitted after a bulk operation, it's possible
                 * that it hasn't been populate with entities yet. */
                continue;
            }

            uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(recs[i]->row);
            if (flags & ECS_FLAG_OBSERVED_ACYCLIC) {
                notify_subset(world, &it, observable, ecs_vector_first(
                    table->storage.entities, ecs_entity_t)[row + i], 
                        event, ids);
            }
        }
    }
    
error:
    return;
}
