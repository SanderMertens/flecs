/**
 * @file addons/script/reactivity/refs.c
 * @brief Script component reference tracking.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

ECS_COMPONENT_DECLARE(EcsScriptUpdateEvent);

void flecs_script_ref_on_set(
    ecs_iter_t *it)
{
    ecs_script_ref_ctx_t *ctx = it->ctx;
    ecs_entity_t script = ctx->script;
    ecs_world_t *world = it->real_world;

    if (!ecs_is_alive(world, script)) {
        return;
    }

    const EcsScript *s = ecs_get(world, script, EcsScript);
    if (!s || !s->script || !s->code) {
        return;
    }

    if (flecs_script_impl(s->script)->evaluating) {
        return;
    }

    if (ecs_is_deferred(it->world)) {
        EcsScriptUpdateEvent evt = { .script = script };
        ecs_enqueue(it->world, &(ecs_event_desc_t){
            .event = ecs_id(EcsScriptUpdateEvent),
            .entity = EcsAny,
            .param = &evt
        });
        return;
    }

    char *code = ecs_os_strdup(s->code);
    ecs_script_update(world, script, 0, code);
    ecs_os_free(code);
}

static void flecs_script_on_update_event(
    ecs_iter_t *it)
{
    ecs_assert(ecs_is_deferred(it->world), ECS_INTERNAL_ERROR, NULL);

    EcsScriptUpdateEvent *evt = it->param;
    ecs_world_t *world = it->real_world;
    ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);

    if (!ecs_is_alive(world, evt->script)) {
        return;
    }

    const EcsScript *s = ecs_get(world, evt->script, EcsScript);
    if (!s || !s->script || !s->code) {
        return;
    }

    if (flecs_script_impl(s->script)->evaluating) {
        return;
    }

    char *code = ecs_os_strdup(s->code);
    ecs_script_update(world, evt->script, 0, code);
    ecs_os_free(code);
}

static void flecs_script_ref_ctx_free(
    void *ptr)
{
    ecs_os_free(ptr);
}

ecs_entity_t flecs_script_create_ref_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_entity_t entity,
    ecs_id_t component,
    ecs_iter_action_t callback)
{
    ecs_entity_t prev_scope = ecs_set_scope(world, script);

    ecs_observer_desc_t desc = {
        .query.terms = {{ .id = component, .src.id = entity }},
        .events = { EcsOnSet },
        .callback = callback
    };

    ecs_script_ref_ctx_t *ctx = ecs_os_malloc_t(ecs_script_ref_ctx_t);
    ctx->script = script;
    ctx->instance = instance;
    desc.ctx = ctx;
    desc.ctx_free = flecs_script_ref_ctx_free;

    ecs_entity_t observer = ecs_observer_init(world, &desc);

    ecs_set_scope(world, prev_scope);

    return observer;
}

void flecs_script_update_ref_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_vec_t *refs,
    ecs_vec_t *observers,
    ecs_iter_action_t callback)
{
    ecs_script_ref_t *new_refs = ecs_vec_first(refs);
    int32_t i, new_count = ecs_vec_count(refs);

    ecs_script_ref_t *old_refs = ecs_vec_first(observers);
    int32_t j, old_count = ecs_vec_count(observers);

    ecs_vec_t result;
    ecs_vec_init_t(NULL, &result, ecs_script_ref_t, new_count);

    for (i = 0; i < new_count; i ++) {
        ecs_entity_t entity = new_refs[i].entity;
        ecs_id_t component = new_refs[i].component;
        ecs_entity_t observer = 0;

        for (j = 0; j < old_count; j ++) {
            if (old_refs[j].observer &&
                old_refs[j].entity == entity &&
                old_refs[j].component == component)
            {
                observer = old_refs[j].observer;
                old_refs[j].observer = 0;
                break;
            }
        }

        if (!observer) {
            observer = flecs_script_create_ref_observer(
                world, script, instance, entity, component, callback);
        }

        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &result, ecs_script_ref_t);
        ref->entity = entity;
        ref->name = NULL;
        ref->component = component;
        ref->observer = observer;
    }

    for (j = 0; j < old_count; j ++) {
        if (old_refs[j].observer) {
            ecs_delete(world, old_refs[j].observer);
        }
    }

    ecs_vec_fini_t(NULL, observers, ecs_script_ref_t);
    *observers = result;
}

void flecs_script_refs_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptUpdateEvent);

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "ScriptUpdateObserver" }),
        .query.terms = {{ .id = EcsAny }},
        .events = { ecs_id(EcsScriptUpdateEvent) },
        .callback = flecs_script_on_update_event
    });
}

#endif
