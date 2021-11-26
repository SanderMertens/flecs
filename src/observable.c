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
    ecs_entity_t entity,
    ecs_entity_t event,
    ecs_ids_t *ids,
    ecs_event_desc_t *desc)
{
    ecs_id_t pair = ecs_pair(EcsWildcard, entity);
    ecs_id_record_t *idr = flecs_get_id_record(world, pair);
    if (!idr) {
        return;
    }

    ecs_table_record_t *trs = ecs_table_cache_tables(
        &idr->cache, ecs_table_record_t);
    int32_t i, count = ecs_table_cache_count(&idr->cache);
    
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &trs[i];
        ecs_table_t *table = tr->table;
        ecs_id_t id = ecs_vector_get(table->type, ecs_id_t, tr->column)[0];
        ecs_entity_t rel = ECS_PAIR_RELATION(id);
        int32_t entity_count = ecs_table_count(table);

        flecs_triggers_notify(world, desc->observable, ids, event, 
            entity, table, table, 0, entity_count, 
            ecs_pair(rel, EcsWildcard), desc->param);
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

    ecs_ids_t ids_storage, *ids = desc->ids;
    ecs_table_t *table = NULL, *other_table = NULL;
    int32_t row = 0;
    int32_t i, count = 0;
    ecs_entity_t event = desc->event;
    ecs_entity_t entity = 0;
    bool skip_set_triggers = false;

    world->event_id ++;

    if (desc->payload_kind == EcsPayloadEntity) {
        entity = desc->payload.entity;
        ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_record_t *r = ecs_eis_get(world, entity);
        if (r) {
            bool is_watched;
            table = r->table;
            row = flecs_record_to_row(r->row, &is_watched);
            if (!is_watched) {
                skip_set_triggers = true;
            }
        }
        count = 1;

    } else if (desc->payload_kind == EcsPayloadTable) {
        ecs_assert(desc->payload.table.table != NULL, ECS_INTERNAL_ERROR, NULL);
        table = desc->payload.table.table;
        other_table = desc->payload.table.other_table;
        row = desc->payload.table.offset;
        int32_t payload_count = desc->payload.table.count;
        if (!payload_count) {
            count = ecs_table_count(table) - row;
        } else {
            count = payload_count;
        }
    }

    if (table) {
        if (!ids) {
            ids = &ids_storage;
            ids_storage.array = ecs_vector_first(table->type, ecs_id_t);
            ids_storage.count = ecs_vector_count(table->type);
        }
    }
    
    flecs_triggers_notify(world, desc->observable, ids, event, 
        entity, table, other_table, row, count, 0, desc->param);

    if (!skip_set_triggers && table) {
        ecs_entity_t *ents = ecs_vector_get(
            table->storage.entities, ecs_entity_t, row);
        ecs_record_t **recs = ecs_vector_get(
            table->storage.record_ptrs, ecs_record_t*, row);

        for (i = 0; i < count; i ++) {
            if (recs[i]->row < 0) {
                notify_subset(world, ents[i], event, ids, desc);
            }
        }
    }
error:
    return;
}
