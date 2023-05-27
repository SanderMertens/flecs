/**
 * @file observable.h
 * @brief Functions for sending events.
 */

#ifndef FLECS_OBSERVABLE_H
#define FLECS_OBSERVABLE_H

ecs_event_record_t* flecs_event_record_get(
    const ecs_observable_t *o,
    ecs_entity_t event);

ecs_event_record_t* flecs_event_record_ensure(
    ecs_observable_t *o,
    ecs_entity_t event);

ecs_event_id_record_t* flecs_event_id_record_get(
    const ecs_event_record_t *er,
    ecs_id_t id);

ecs_event_id_record_t* flecs_event_id_record_ensure(
    ecs_world_t *world,
    ecs_event_record_t *er,
    ecs_id_t id);

void flecs_event_id_record_remove(
    ecs_event_record_t *er,
    ecs_id_t id);

void flecs_observable_init(
    ecs_observable_t *observable);

void flecs_observable_fini(
    ecs_observable_t *observable);

bool flecs_observers_exist(
    ecs_observable_t *observable,
    ecs_id_t id,
    ecs_entity_t event);

void flecs_observer_fini(
    ecs_observer_t *observer);

void flecs_emit( 
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_event_desc_t *desc);

bool flecs_default_observer_next_callback(
    ecs_iter_t *it);

void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav,
    int32_t evtx);

void flecs_emit_propagate_invalidate(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t offset,
    int32_t count);

#endif
