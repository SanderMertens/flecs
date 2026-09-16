/**
 * @file addons/script/reactivity/state.h
 * @brief Utilities for managing script state.
 */

#ifndef FLECS_SCRIPT_STATE_H
#define FLECS_SCRIPT_STATE_H

/* Storage for slots inside of for loops */
typedef struct ecs_script_for_component_t {
    ecs_id_t component;
    int32_t visit;
} ecs_script_for_component_t;

typedef struct ecs_script_for_entry_t {
    int32_t visit;
    ecs_vec_t components; /* vec<ecs_script_for_component_t> */
} ecs_script_for_entry_t;

typedef struct ecs_script_for_slot_t {
    ecs_vec_t entities; /* vec<ecs_entity_t>, anonymous entities */
    ecs_map_t named;
    ecs_entity_t cache_entity;
    ecs_script_for_entry_t *cache_entry;
    int32_t scope_slot;
} ecs_script_for_slot_t;

/* Stores reference to entity in same script */
typedef struct ecs_script_symbol_slot_t {
    ecs_entity_t entity;
    int32_t scope_slot;
} ecs_script_symbol_slot_t;

/* Stores reference to component in same script */
typedef struct ecs_script_component_slot_t {
    int32_t entity_slot;
    ecs_id_t component;
    int32_t scope_slot;
} ecs_script_component_slot_t;

/* Stores value of const variable so it can be diffed in reactive update */
typedef struct ecs_script_computed_t {
    void *ptr;
    const ecs_type_info_t *ti;
    bool valid;
} ecs_script_computed_t;

typedef struct ecs_script_state_t {
    ecs_vec_t symbol_slots;
    ecs_vec_t component_slots;
    ecs_vec_t scope_slots;
    ecs_vec_t for_slots;
    ecs_vec_t computed;
    int32_t visit;
    bool initialized;
} ecs_script_state_t;

void flecs_script_state_init(
    ecs_script_state_t *state);

void flecs_script_state_fini(
    ecs_script_state_t *state);

void flecs_script_state_clear_computed(
    ecs_script_state_t *state);

void flecs_script_state_resize_computed(
    ecs_script_state_t *state,
    int32_t count);

void flecs_script_state_resize(
    ecs_script_state_t *state,
    int32_t scope_count,
    int32_t component_count,
    int32_t for_count);

int32_t flecs_script_state_next(
    ecs_script_state_t *state);

void flecs_script_state_mark(
    ecs_script_state_t *state,
    const ecs_script_region_t *region,
    int32_t visit);

void flecs_script_for_slots_init(
    ecs_vec_t *for_slots,
    int32_t count);

void flecs_script_for_slots_fini(
    ecs_vec_t *for_slots);

void flecs_script_for_slot_clear(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    bool delete_named);

void flecs_script_for_slot_purge(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    int32_t visit);

void flecs_script_for_slot_mark(
    ecs_script_for_slot_t *slot,
    int32_t visit);

void flecs_script_for_slot_track(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    int32_t visit,
    bool *named);

void flecs_script_for_slot_track_component(
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    ecs_id_t component,
    int32_t visit);

#endif
