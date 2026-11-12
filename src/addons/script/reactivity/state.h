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
    uint64_t key;
    ecs_vec_t components; /* vec<ecs_script_for_component_t> */
} ecs_script_for_entry_t;

typedef struct ecs_script_for_slot_t {
    ecs_map_t entries;
    ecs_map_t keys;
    ecs_entity_t cache_entity;
    ecs_script_for_entry_t *cache_entry;
    int32_t scope_slot;
} ecs_script_for_slot_t;

#ifndef FLECS_SCRIPT_QUERY_BIND_MAX
#define FLECS_SCRIPT_QUERY_BIND_MAX (8)
#endif

typedef struct EcsScriptQuery {
    const struct ecs_script_query_node_t *node;
    ecs_entity_t binds[FLECS_SCRIPT_QUERY_BIND_MAX];
} EcsScriptQuery;

typedef struct ecs_script_query_slot_t {
    EcsScriptQuery value;
    ecs_world_t *world;
    ecs_entity_t observers[2];
} ecs_script_query_slot_t;

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
    ecs_vec_t query_slots;
    ecs_vec_t computed;
    int32_t visit;
    bool initialized;
} ecs_script_state_t;

void flecs_script_slots_reset(
    ecs_vec_t *slots,
    ecs_size_t size,
    int32_t count);

#define flecs_script_slots_reset_t(slots, T, count)\
    flecs_script_slots_reset(slots, ECS_SIZEOF(T), count)

void flecs_script_state_init(
    ecs_script_state_t *state);

void flecs_script_state_fini(
    ecs_script_state_t *state);

void flecs_script_state_clear_computed(
    ecs_script_state_t *state);

void flecs_script_state_resize(
    ecs_script_state_t *state,
    int32_t scope_count,
    int32_t component_count,
    int32_t for_count,
    int32_t query_count,
    int32_t computed_count);

ecs_entity_t flecs_script_state_component_src(
    const ecs_world_t *world,
    const ecs_script_state_t *state,
    const ecs_script_component_slot_t *slot);

int32_t flecs_script_state_next(
    ecs_script_state_t *state);

void flecs_script_state_mark(
    ecs_script_state_t *state,
    const ecs_script_region_t *region,
    int32_t visit);

void flecs_script_for_slot_clear(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot);

void flecs_script_for_slot_remove_key(
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    uint64_t key);

void flecs_script_for_slot_purge(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    int32_t visit);

uint64_t flecs_script_for_key(
    uint64_t outer,
    uint64_t value);

ecs_entity_t flecs_script_for_slot_find(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    uint64_t *key,
    ecs_entity_t parent,
    int32_t visit);

bool flecs_script_for_slot_track(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    uint64_t key,
    int32_t visit);

void flecs_script_for_slot_track_component(
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    ecs_id_t component,
    int32_t visit);

void flecs_script_query_slots_clear(
    ecs_vec_t *query_slots);

#endif
