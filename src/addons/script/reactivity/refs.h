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

typedef struct EcsScriptUpdateEvent {
    ecs_entity_t script;
    uint64_t input;
} EcsScriptUpdateEvent;

extern ECS_COMPONENT_DECLARE(EcsScriptUpdateEvent);

ecs_entity_t flecs_script_create_ref_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_entity_t entity,
    ecs_id_t component,
    uint64_t input,
    bool is_has,
    ecs_iter_action_t callback);

void flecs_script_update_ref_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_vec_t *refs,
    ecs_vec_t *observers,
    ecs_iter_action_t callback);

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

void flecs_script_ref_on_set(
    ecs_iter_t *it);

void flecs_script_run_pending_resolves(
    ecs_world_t *world);

void flecs_script_refs_import(
    ecs_world_t *world);

#endif
