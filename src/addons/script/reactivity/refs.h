/**
 * @file addons/script/reactivity/refs.h
 * @brief Script component reference tracking.
 */

#ifndef FLECS_SCRIPT_REFS_H
#define FLECS_SCRIPT_REFS_H

typedef struct ecs_script_ref_t {
    ecs_entity_t entity;
    const char *name;
    ecs_id_t component;
    ecs_entity_t observer;
    uint64_t input;
    bool is_has;
    bool is_resolve;
} ecs_script_ref_t;

typedef struct ecs_script_ref_ctx_t {
    ecs_entity_t script;
    ecs_entity_t instance;
    char *name;
    uint64_t input;
} ecs_script_ref_ctx_t;

typedef struct ecs_script_pending_t {
    ecs_entity_t script;
    ecs_entity_t instance;
    uint64_t input;
    int32_t depth;
} ecs_script_pending_t;

ecs_script_ref_t* flecs_script_ref_ensure(
    ecs_vec_t *refs,
    const ecs_script_ref_t *value);

const char* flecs_script_name_leaf(
    const char *name);

void flecs_script_observe_refs(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_vec_t *refs,
    ecs_vec_t *observers);

void flecs_script_record_dyn_ref(
    ecs_vec_t *run_refs,
    ecs_entity_t entity,
    ecs_id_t component,
    uint64_t input,
    bool is_has);

void flecs_script_update_dyn_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_vec_t *run_refs,
    ecs_vec_t *observers,
    uint64_t input);

void flecs_script_update_resolve_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_script_impl_t *impl,
    ecs_vec_t *observers);

void flecs_script_ref_observers_fini(
    ecs_vec_t *observers);

void flecs_script_ref_observers_clear(
    ecs_world_t *world,
    ecs_vec_t *observers);

void flecs_script_ref_enqueue(
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_entity_t script,
    ecs_entity_t instance,
    uint64_t input);

void flecs_script_run_pending_resolves(
    ecs_world_t *world);

void flecs_script_pending_resolve_remove(
    ecs_world_t *world,
    ecs_entity_t script);

void flecs_script_refs_import(
    ecs_world_t *world);

#endif
