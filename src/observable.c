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
        (void)et;

        /* All triggers should've unregistered by now */
        ecs_assert(!ecs_map_is_initialized(&et->event_ids), 
            ECS_INTERNAL_ERROR, NULL);
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
    const ecs_type_t *ids)
{
    ecs_id_t pair = ecs_pair(EcsWildcard, entity);
    ecs_id_record_t *idr = flecs_id_record_get(world, pair);
    if (!idr) {
        return;
    }

    /* Iterate acyclic relations */
    ecs_id_record_t *cur = idr;
    while ((cur = cur->acyclic.next)) {
        flecs_process_pending_tables(world);

        ecs_table_cache_iter_t idt;
        if (!flecs_table_cache_iter(&cur->cache, &idt)) {
            return;
        }

        ecs_entity_t rel = ECS_PAIR_FIRST(cur->id);
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&idt, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;

            int32_t e, entity_count = ecs_table_count(table);
            it->table = table;
            it->other_table = NULL;
            it->offset = 0;
            it->count = entity_count;

            /* Treat as new event as this could invoke observers again for
            * different tables. */
            world->event_id ++;

            flecs_set_observers_notify(it, observable, ids, event,
                ecs_pair(rel, EcsWildcard));

            ecs_entity_t *entities = ecs_storage_first(&table->data.entities);
            ecs_record_t **records = ecs_storage_first(&table->data.records);

            for (e = 0; e < entity_count; e ++) {
                uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(records[e]->row);
                if (flags & EcsEntityObservedAcyclic) {
                    /* Only notify for entities that are used in pairs with
                    * acyclic relations */
                    notify_subset(world, it, observable, entities[e], event, ids);
                }
            }
        }
    }
}

void flecs_emit(
    ecs_world_t *world,
    ecs_world_t *stage,
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

    const ecs_type_t *ids = desc->ids;
    ecs_entity_t event = desc->event;
    ecs_table_t *table = desc->table;
    int32_t row = desc->offset;
    int32_t i, count = desc->count;
    ecs_entity_t relation = desc->relation;

    if (!count) {
        count = ecs_table_count(table) - row;
    }

    ecs_iter_t it = {
        .world = stage,
        .real_world = world,
        .table = table,
        .term_count = 1,
        .other_table = desc->other_table,
        .offset = row,
        .count = count,
        .param = (void*)desc->param,
        .flags = desc->table_event ? EcsIterTableOnly : 0
    };

    world->event_id ++;

    ecs_observable_t *observable = ecs_get_observable(desc->observable);
    ecs_check(observable != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!desc->relation) {
        flecs_observers_notify(&it, observable, ids, event);
    } else {
        flecs_set_observers_notify(&it, observable, ids, event, 
            ecs_pair(relation, EcsWildcard));
    }

    if (count && !desc->table_event) {
        ecs_record_t **recs = ecs_storage_get_t(
            &table->data.records, ecs_record_t*, row);

        for (i = 0; i < count; i ++) {
            ecs_record_t *r = recs[i];
            if (!r) {
                /* If the event is emitted after a bulk operation, it's possible
                 * that it hasn't been populate with entities yet. */
                continue;
            }

            uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(recs[i]->row);
            if (flags & EcsEntityObservedAcyclic) {
                notify_subset(world, &it, observable, ecs_storage_first_t(
                    &table->data.entities, ecs_entity_t)[row + i], event, ids);
            }
        }
    }
    
error:
    return;
}

void ecs_emit(
    ecs_world_t *stage,
    ecs_event_desc_t *desc)
{
    ecs_world_t *world = (ecs_world_t*)ecs_get_world(stage);
    flecs_emit(world, stage, desc);
}
