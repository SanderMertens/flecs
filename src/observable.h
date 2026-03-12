/**
 * @file observable.h
 * @brief Functions for emitting events.
 */

#ifndef FLECS_OBSERVABLE_H
#define FLECS_OBSERVABLE_H

/* All observers registered for a specific (event, component id) pair */
typedef struct ecs_event_id_record_t {
    ecs_map_t self;                  /* Observers matching Self only */
    ecs_map_t self_up;               /* Observers matching Self|Up */
    ecs_map_t up;                    /* Observers matching Up only */
    int32_t observer_count;          /* Total active observers across all maps */
} ecs_event_id_record_t;

typedef struct ecs_observer_impl_t {
    ecs_observer_t pub;

    int32_t *last_event_id;     /* Pointer to last handled event id (dedup) */
    int32_t last_event_id_storage;

    ecs_flags32_t flags;

    int8_t term_index;          /* Index of term in parent (single-term only) */
    ecs_id_t register_id;       /* Id registered with (single-term only) */
    uint64_t id;                /* Internal observer id (not entity id) */

    ecs_vec_t children;         /* Child observers (multi-term only) */

    ecs_query_t *not_query;     /* Not->Optional query to populate data from
                                 * entity's current table before move */

    flecs_poly_dtor_t dtor;
} ecs_observer_impl_t;

#define flecs_observer_impl(observer) (ECS_CONST_CAST(ecs_observer_impl_t*, observer))

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
    const ecs_observable_t *observable,
    ecs_id_t id,
    ecs_entity_t event);

ecs_observer_t* flecs_observer_init(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_observer_desc_t *desc);

void flecs_observer_fini(
    ecs_observer_t *observer);

void flecs_emit(
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_event_desc_t *desc);

bool flecs_default_next_callback(
    ecs_iter_t *it);

void flecs_observers_invoke(
    ecs_world_t *world,
    ecs_map_t *observers,
    ecs_iter_t *it,
    ecs_table_t *table,
    ecs_entity_t trav);

void flecs_emit_propagate_invalidate(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t offset,
    int32_t count);

void flecs_observer_set_disable_bit(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_flags32_t bit,
    bool cond);

#endif
