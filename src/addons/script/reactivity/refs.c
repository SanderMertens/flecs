/**
 * @file addons/script/reactivity/refs.c
 * @brief Script component reference tracking.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

ECS_COMPONENT_DECLARE(EcsScriptUpdateEvent);

static void flecs_script_ref_eval(
    ecs_world_t *world,
    ecs_entity_t script,
    uint64_t input)
{
    EcsScript *s = ecs_ensure(world, script, EcsScript);
    if (!s || !s->script || !input) {
        return;
    }
    ecs_script_impl_t *impl = flecs_script_impl(s->script);
    if (impl->evaluating) {
        return;
    }
    bool is_deferred = ecs_is_deferred(world);
    if (is_deferred) {
        ecs_defer_suspend(world);
    }
    impl->evaluating = true;
    ecs_script_runtime_t *runtime = ecs_script_runtime_new();
    ecs_script_eval_desc_t desc = { .runtime = runtime };
    ecs_script_eval_result_t result = {0};
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_vec_clear(&impl->run_refs);
    int rc = flecs_script_eval(s->script, &desc,
        ecs_pair_t(EcsScript, script), input, &result);
    ecs_set_scope(world, prev_scope);
    impl->evaluating = false;
    s = ecs_ensure(world, script, EcsScript);
    if (s && s->script == &impl->pub && !rc) {
        flecs_script_update_dyn_observers(
            world, script, impl, &s->dyn_observers, input);
        s = ecs_ensure(world, script, EcsScript);
    }
    ecs_vec_clear(&impl->run_refs);
    if (s && s->error) {
        ecs_os_free(s->error);
        s->error = NULL;
    }
    if (s && rc) {
        s->error = result.error;
    } else {
        ecs_os_free(result.error);
    }
    ecs_script_runtime_free(runtime);
    if (is_deferred) {
        ecs_defer_resume(world);
    }
}

static const char* flecs_script_name_leaf(
    const char *name)
{
    const char *last = name, *ptr = name;
    while ((ptr = strchr(ptr, '.'))) {
        if (ptr == name || ptr[-1] != '\\') {
            last = ptr + 1;
        }
        ptr ++;
    }
    return last;
}

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
        evt.input = ctx->input;
        ecs_enqueue(it->world, &(ecs_event_desc_t){
            .event = ecs_id(EcsScriptUpdateEvent),
            .entity = EcsAny,
            .param = &evt
        });
        return;
    }

    flecs_script_ref_eval(world, script, ctx->input);
}

static void flecs_script_resolve_run(
    ecs_world_t *world,
    ecs_entity_t script)
{
    const EcsScript *s = ecs_get(world, script, EcsScript);
    if (!s || !s->code) {
        return;
    }

    if (s->script && flecs_script_impl(s->script)->evaluating) {
        return;
    }

    char *code = ecs_os_strdup(s->code);
    ecs_script_runtime_t *runtime = ecs_script_runtime_new();
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    flecs_script_update(world, script, 0, code, runtime);
    ecs_set_scope(world, prev_scope);
    ecs_script_runtime_free(runtime);
    ecs_os_free(code);
}

static bool flecs_script_defer_resolve(
    ecs_world_t *world,
    ecs_entity_t script)
{
    ecs_script_runtime_t *runtime = flecs_script_runtime_get(world);
    if (!runtime->include_depth && !runtime->resolving) {
        return false;
    }

    ecs_vec_t *pending = &runtime->pending_resolves;
    ecs_entity_t *elems = ecs_vec_first(pending);
    int32_t i, count = ecs_vec_count(pending);
    for (i = 0; i < count; i ++) {
        if (elems[i] == script) {
            return true;
        }
    }

    ecs_vec_append_t(&runtime->allocator, pending, ecs_entity_t)[0] = script;

    return true;
}

void flecs_script_run_pending_resolves(
    ecs_world_t *world)
{
    ecs_script_runtime_t *runtime = flecs_script_runtime_get(world);
    if (runtime->resolving) {
        return;
    }

    runtime->resolving = true;

    ecs_vec_t *pending = &runtime->pending_resolves;

    while (ecs_vec_count(pending)) {
        ecs_entity_t script = ecs_vec_first_t(pending, ecs_entity_t)[0];
        ecs_vec_remove_t(pending, ecs_entity_t, 0);

        if (!ecs_is_alive(world, script)) {
            continue;
        }

        flecs_script_resolve_run(world, script);
    }

    runtime->resolving = false;
}

static void flecs_script_resolve_on_set(
    ecs_iter_t *it)
{
    ecs_script_ref_ctx_t *ctx = it->ctx;
    ecs_entity_t script = ctx->script;
    ecs_world_t *world = it->real_world;

    if (!ecs_is_alive(world, script)) {
        return;
    }

    if (ctx->name) {
        int32_t i;
        for (i = 0; i < it->count; i ++) {
            const char *name = ecs_get_name(world, it->entities[i]);
            if (name && !ecs_os_strcmp(name, ctx->name)) {
                break;
            }
        }
        if (i == it->count) {
            return;
        }
    }

    const EcsScript *s = ecs_get(world, script, EcsScript);
    if (!s || !s->code) {
        return;
    }

    if (s->script && flecs_script_impl(s->script)->evaluating) {
        return;
    }

    if (flecs_script_defer_resolve(world, script)) {
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

    flecs_script_resolve_run(world, script);
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
    if (!s || !s->code) {
        return;
    }

    if (s->script && flecs_script_impl(s->script)->evaluating) {
        return;
    }

    if (evt->input) {
        flecs_script_ref_eval(world, evt->script, evt->input);
    } else {
        char *code = ecs_os_strdup(s->code);
        ecs_script_runtime_t *runtime = ecs_script_runtime_new();
        ecs_entity_t prev_scope = ecs_set_scope(world, 0);
        flecs_script_update(world, evt->script, 0, code, runtime);
        ecs_set_scope(world, prev_scope);
        ecs_script_runtime_free(runtime);
        ecs_os_free(code);
    }
}

static void flecs_script_ref_ctx_free(
    void *ptr)
{
    ecs_script_ref_ctx_t *ctx = ptr;
    ecs_os_free(ctx->name);
    ecs_os_free(ctx);
}

ecs_entity_t flecs_script_create_ref_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_entity_t entity,
    ecs_id_t component,
    uint64_t input,
    bool is_has,
    ecs_iter_action_t callback)
{
    ecs_entity_t prev_scope = ecs_set_scope(world, script);

    ecs_observer_desc_t desc = {
        .query.terms = {{ .id = component, .src.id = entity }},
        .events = { EcsOnSet },
        .callback = callback
    };

    if (is_has) {
        desc.events[0] = EcsOnAdd;
        desc.events[1] = EcsOnRemove;
    }

    ecs_script_ref_ctx_t *ctx = ecs_os_malloc_t(ecs_script_ref_ctx_t);
    ctx->script = script;
    ctx->instance = instance;
    ctx->name = NULL;
    ctx->input = input;
    desc.ctx = ctx;
    desc.ctx_free = flecs_script_ref_ctx_free;

    ecs_entity_t observer = ecs_observer_init(world, &desc);

    ecs_set_scope(world, prev_scope);

    return observer;
}

static ecs_entity_t flecs_script_create_resolve_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    const ecs_script_ref_t *ref)
{
    ecs_entity_t prev_scope = ecs_set_scope(world, script);

    ecs_observer_desc_t desc = {
        .events = { EcsOnSet },
        .callback = flecs_script_resolve_on_set
    };

    ecs_script_ref_ctx_t *ctx = ecs_os_malloc_t(ecs_script_ref_ctx_t);
    ctx->script = script;
    ctx->instance = instance;
    ctx->name = NULL;
    ctx->input = 0;

    if (ref->name) {
        desc.query.terms[0].id = ecs_pair_t(EcsIdentifier, EcsName);
        if (ref->component) {
            desc.query.terms[1].id = ref->component;
            desc.query.terms[2].id = ecs_id(EcsTypeSerializer);
            desc.query.terms[2].oper = EcsOptional;
        }
        ctx->name = ecs_os_strdup(ref->name);
    } else {
        desc.query.terms[0].id = ref->component;
        desc.query.terms[0].src.id = ref->entity;
        if (ref->is_has) {
            desc.events[0] = EcsOnAdd;
        }
    }

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
        bool is_has = new_refs[i].is_has;
        bool is_resolve = new_refs[i].is_resolve;
        uint64_t input = new_refs[i].input;
        const char *name = is_resolve ? new_refs[i].name : NULL;
        ecs_entity_t observer = 0;

        for (j = 0; j < old_count; j ++) {
            const char *old_name = old_refs[j].name;
            if (old_refs[j].observer &&
                old_refs[j].entity == entity &&
                old_refs[j].component == component &&
                old_refs[j].input == input &&
                old_refs[j].is_has == is_has &&
                old_refs[j].is_resolve == is_resolve &&
                ((!old_name && !name) || (old_name && name &&
                    !ecs_os_strcmp(old_name, name))))
            {
                observer = old_refs[j].observer;
                old_refs[j].observer = 0;
                break;
            }
        }

        if (!observer) {
            if (is_resolve) {
                observer = flecs_script_create_resolve_observer(
                    world, script, instance, &new_refs[i]);
            } else {
                observer = flecs_script_create_ref_observer(
                    world, script, instance, entity, component, input,
                    is_has, callback);
            }
        }

        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &result, ecs_script_ref_t);
        ref->entity = entity;
        ref->name = name ? ecs_os_strdup(name) : NULL;
        ref->component = component;
        ref->observer = observer;
        ref->input = input;
        ref->is_has = is_has;
        ref->is_resolve = is_resolve;
    }

    for (j = 0; j < old_count; j ++) {
        if (old_refs[j].observer) {
            ecs_delete(world, old_refs[j].observer);
        }
        if (old_refs[j].is_resolve) {
            ecs_os_free(ECS_CONST_CAST(char*, old_refs[j].name));
        }
    }

    ecs_vec_fini_t(NULL, observers, ecs_script_ref_t);
    *observers = result;
}

static void flecs_script_ref_observers_discard(
    ecs_world_t *world,
    ecs_vec_t *observers)
{
    int32_t i, count = ecs_vec_count(observers);
    ecs_script_ref_t *refs = ecs_vec_first(observers);
    for (i = 0; i < count; i ++) {
        if (refs[i].observer && ecs_is_alive(world, refs[i].observer)) {
            ecs_delete(world, refs[i].observer);
        }
        refs[i].observer = 0;
    }
}

void flecs_script_record_dyn_ref(
    ecs_script_impl_t *impl,
    ecs_entity_t entity,
    ecs_id_t component,
    uint64_t input,
    bool is_has)
{
    ecs_vec_t *refs = &impl->run_refs;
    ecs_script_ref_t *elems = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (elems[i].entity == entity && elems[i].component == component &&
            elems[i].input == input && elems[i].is_has == is_has)
        {
            return;
        }
    }

    ecs_script_ref_t *ref = ecs_vec_append_t(NULL, refs, ecs_script_ref_t);
    ref->entity = entity;
    ref->name = NULL;
    ref->component = component;
    ref->observer = 0;
    ref->input = input;
    ref->is_has = is_has;
    ref->is_resolve = false;
}

void flecs_script_update_dyn_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_script_impl_t *impl,
    ecs_vec_t *observers,
    uint64_t input)
{
    ecs_vec_t *run_refs = &impl->run_refs;
    ecs_script_ref_t *new_refs = ecs_vec_first(run_refs);
    int32_t i, new_count = ecs_vec_count(run_refs);

    if (!new_count && !ecs_vec_count(observers)) {
        return;
    }

    uint64_t recorded = 0;
    for (i = 0; i < new_count; i ++) {
        recorded |= new_refs[i].input;
    }

    uint64_t discard = input | recorded;

    ecs_vec_t refs;
    ecs_vec_init_t(NULL, &refs, ecs_script_ref_t, new_count);

    ecs_script_ref_t *old_refs = ecs_vec_first(observers);
    int32_t old_count = ecs_vec_count(observers);
    for (i = 0; i < old_count; i ++) {
        if (old_refs[i].input & discard) {
            continue;
        }
        if (!ecs_is_alive(world, old_refs[i].entity)) {
            continue;
        }
        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &refs, ecs_script_ref_t);
        *ref = old_refs[i];
        ref->observer = 0;
    }

    for (i = 0; i < new_count; i ++) {
        if (!ecs_is_alive(world, new_refs[i].entity)) {
            continue;
        }
        if (ecs_has_pair(world, new_refs[i].entity, ecs_id(EcsScript),
            script))
        {
            continue;
        }
        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &refs, ecs_script_ref_t);
        *ref = new_refs[i];
        ref->observer = 0;
    }

    flecs_script_update_ref_observers(world, script, 0, &refs, observers,
        flecs_script_ref_on_set);

    ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
}

void flecs_script_update_resolve_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_script_impl_t *impl,
    ecs_vec_t *observers)
{
    int32_t i, j, ref_count = ecs_vec_count(&impl->unresolved_refs);
    ecs_script_unresolved_ref_t *refs = ecs_vec_first(
        &impl->unresolved_refs);
    int32_t component_ref_count = ecs_vec_count(
        &impl->unresolved_component_refs);
    ecs_script_unresolved_component_ref_t *component_refs = ecs_vec_first(
        &impl->unresolved_component_refs);

    ecs_vec_t resolve_refs;
    ecs_vec_init_t(NULL, &resolve_refs, ecs_script_ref_t, 0);

    for (i = 0; i < ref_count; i ++) {
        if (refs[i].kind == FlecsScriptUnresolvedVariable) {
            continue;
        }

        const char *name = flecs_script_name_leaf(refs[i].name);
        ecs_id_t component = 0;
        if (refs[i].kind == FlecsScriptUnresolvedComponent) {
            component = ecs_id(EcsComponent);
        }

        ecs_script_ref_t *elems = ecs_vec_first(&resolve_refs);
        int32_t elem_count = ecs_vec_count(&resolve_refs);
        for (j = 0; j < elem_count; j ++) {
            if (elems[j].name && elems[j].component == component &&
                !ecs_os_strcmp(elems[j].name, name))
            {
                break;
            }
        }
        if (j != elem_count) {
            continue;
        }

        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &resolve_refs, ecs_script_ref_t);
        ref->entity = 0;
        ref->name = name;
        ref->component = component;
        ref->observer = 0;
        ref->input = 0;
        ref->is_has = false;
        ref->is_resolve = true;
    }

    for (i = 0; i < component_ref_count; i ++) {
        ecs_entity_t entity = component_refs[i].entity;
        ecs_id_t component = component_refs[i].component;
        bool is_has = component_refs[i].is_has;

        ecs_script_ref_t *elems = ecs_vec_first(&resolve_refs);
        int32_t elem_count = ecs_vec_count(&resolve_refs);
        for (j = 0; j < elem_count; j ++) {
            if (!elems[j].name && elems[j].entity == entity &&
                elems[j].component == component &&
                elems[j].is_has == is_has)
            {
                break;
            }
        }
        if (j != elem_count) {
            continue;
        }

        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &resolve_refs, ecs_script_ref_t);
        ref->entity = entity;
        ref->name = NULL;
        ref->component = component;
        ref->observer = 0;
        ref->input = 0;
        ref->is_has = is_has;
        ref->is_resolve = true;
    }

    if (!ecs_vec_count(&resolve_refs)) {
        ecs_script_ref_t *old_refs = ecs_vec_first(observers);
        int32_t old_count = ecs_vec_count(observers);
        for (i = 0; i < old_count; i ++) {
            if (old_refs[i].is_resolve) {
                ecs_script_ref_t *ref = ecs_vec_append_t(
                    NULL, &resolve_refs, ecs_script_ref_t);
                *ref = old_refs[i];
                ref->observer = 0;
            }
        }
    }

    flecs_script_ref_observers_discard(world, observers);
    flecs_script_update_ref_observers(world, script, 0, &resolve_refs,
        observers, flecs_script_ref_on_set);

    ecs_vec_fini_t(NULL, &resolve_refs, ecs_script_ref_t);
}

void flecs_script_ref_observers_fini(
    ecs_vec_t *observers)
{
    int32_t i, count = ecs_vec_count(observers);
    ecs_script_ref_t *refs = ecs_vec_first(observers);
    for (i = 0; i < count; i ++) {
        if (refs[i].is_resolve) {
            ecs_os_free(ECS_CONST_CAST(char*, refs[i].name));
        }
    }
    ecs_vec_fini_t(NULL, observers, ecs_script_ref_t);
}

void flecs_script_ref_observers_clear(
    ecs_world_t *world,
    ecs_vec_t *observers)
{
    flecs_script_ref_observers_discard(world, observers);
    flecs_script_ref_observers_fini(observers);
    ecs_vec_init_t(NULL, observers, ecs_script_ref_t, 0);
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
