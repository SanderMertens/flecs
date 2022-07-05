/**
 * @file observable.h
 * @brief Functions for sending events.
 */

#ifndef FLECS_OBSERVABLE_H
#define FLECS_OBSERVABLE_H

void flecs_observable_init(
    ecs_observable_t *observable);

void flecs_observable_fini(
    ecs_observable_t *observable);

void flecs_observers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    const ecs_type_t *ids,
    ecs_entity_t event);

void flecs_set_observers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    const ecs_type_t *ids,
    ecs_entity_t event,
    ecs_id_t set_id);

bool flecs_check_observers_for_event(
    const ecs_poly_t *world,
    ecs_id_t id,
    ecs_entity_t event);

void flecs_observer_fini(
    ecs_observer_t *observer);

void flecs_emit( 
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_event_desc_t *desc);

#endif
