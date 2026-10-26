/**
 * @file addons/script/reactivity/refs.c
 * @brief Script component reference tracking.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static ECS_TAG_DECLARE(EcsScriptUpdateEvent);

static bool flecs_script_ref_matches(
    const ecs_script_ref_t *first,
    const ecs_script_ref_t *second)
{
    return first->entity == second->entity &&
        first->component == second->component &&
        first->is_has == second->is_has &&
        ((!first->name && !second->name) ||
         (first->name && second->name &&
            !ecs_os_strcmp(first->name, second->name)));
}

static ecs_script_ref_t* flecs_script_ref_add(
    ecs_vec_t *refs,
    const ecs_script_ref_t *value,
    bool match_input)
{
    ecs_script_ref_t *array = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (flecs_script_ref_matches(&array[i], value) &&
            (!match_input || array[i].input == value->input))
        {
            return &array[i];
        }
    }

    ecs_script_ref_t *ref = ecs_vec_append_t(NULL, refs, ecs_script_ref_t);
    *ref = *value;
    ref->observer = 0;
    return ref;
}

ecs_script_ref_t* flecs_script_ref_ensure(
    ecs_vec_t *refs,
    const ecs_script_ref_t *value)
{
    return flecs_script_ref_add(refs, value, false);
}

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
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));
    ecs_script_runtime_t *pool = flecs_script_runtime_get(world);
    impl->refcount ++;
    impl->evaluating = true;

    ecs_script_runtime_t *runtime = flecs_script_runtime_acquire_call(pool);
    ecs_script_eval_desc_t desc = { .runtime = runtime };
    ecs_script_eval_result_t result = {0};
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_vec_clear(&impl->run_refs);
    int rc = flecs_script_eval(s->script, &desc,
        ecs_pair_t(EcsScript, script), input, &result);
    ecs_set_scope(world, prev_scope);
    impl->evaluating = false;
    s = ecs_is_alive(world, script)
        ? ecs_ensure(world, script, EcsScript) : NULL;
    if (s && s->script == &impl->pub && !rc) {
        flecs_script_update_dyn_observers(world, script, 0,
            &impl->run_refs, &s->dyn_observers, input);
        s = ecs_is_alive(world, script)
            ? ecs_ensure(world, script, EcsScript) : NULL;
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

    flecs_script_runtime_release_call(pool, runtime);
    ecs_script_free(&impl->pub);
}

static bool flecs_script_ref_is_owned(
    const ecs_world_t *world,
    ecs_entity_t script,
    const ecs_script_ref_t *ref)
{
    return ref->entity && ecs_is_alive(world, ref->entity) &&
        ref->component != ecs_id(EcsScriptMutVar) &&
        !flecs_script_template_is_mut_type(world, ref->component) &&
        ecs_has_pair(world, ref->entity, ecs_id(EcsScript), script);
}

const char* flecs_script_name_leaf(
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
    ecs_script_runtime_t *pool = flecs_script_runtime_get(world);
    ecs_script_runtime_t *runtime = flecs_script_runtime_acquire_call(pool);
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    flecs_script_update(world, script, 0, code, runtime);
    ecs_set_scope(world, prev_scope);
    flecs_script_runtime_release_call(pool, runtime);
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

    ecs_vec_append_t(runtime->allocator, pending, ecs_entity_t)[0] = script;

    return true;
}

void flecs_script_pending_resolve_remove(
    ecs_world_t *world,
    ecs_entity_t script)
{
    ecs_script_runtime_t *runtime = flecs_script_runtime_get(world);
    ecs_vec_t *pending = &runtime->pending_resolves;
    ecs_entity_t *elems = ecs_vec_first(pending);
    int32_t i, count = ecs_vec_count(pending);
    for (i = 0; i < count; i ++) {
        if (elems[i] == script) {
            ecs_vec_remove_t(pending, ecs_entity_t, i);
            return;
        }
    }
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

static void flecs_script_ref_dispatch(
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_entity_t script,
    ecs_entity_t instance,
    uint64_t input)
{
    if (!ecs_is_alive(world, script)) {
        return;
    }

    const EcsScript *s = ecs_get(world, script, EcsScript);
    if (!s || (!s->template_ && (!s->code || (input && !s->script)))) {
        return;
    }

    if (stage) {
        if (!input && !s->template_ &&
            flecs_script_defer_resolve(world, script))
        {
            return;
        }

        if (ecs_is_stage(stage)) {
            flecs_script_ref_enqueue(world, stage, script, instance, input);
            return;
        }
    }

    if (s->template_) {
        flecs_script_template_reeval(world, script, instance, input);
    } else if (s->script && flecs_script_impl(s->script)->evaluating) {
        return;
    } else if (input) {
        flecs_script_ref_eval(world, script, input);
    } else {
        flecs_script_resolve_run(world, script);
    }
}

static void flecs_script_ref_on_set(
    ecs_iter_t *it)
{
    ecs_script_ref_ctx_t *ctx = it->ctx;
    flecs_script_ref_dispatch(
        it->world, it->stage, ctx->script, ctx->instance, ctx->input);
}

static void flecs_script_resolve_on_set(
    ecs_iter_t *it)
{
    ecs_script_ref_ctx_t *ctx = it->ctx;
    if (ctx->name) {
        int32_t i;
        for (i = 0; i < it->count; i ++) {
            const char *name = ecs_get_name(it->world, it->entities[i]);
            if (name && !ecs_os_strcmp(name, ctx->name)) {
                break;
            }
        }

        if (i == it->count) {
            return;
        }
    }

    flecs_script_ref_dispatch(it->world, it->stage, ctx->script, 0, 0);
}

void flecs_script_ref_enqueue(
    ecs_world_t *world,
    ecs_world_t *stage,
    ecs_entity_t script,
    ecs_entity_t instance,
    uint64_t input)
{
    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    ecs_vec_append_t(NULL, &rt->pending, ecs_script_pending_t)[0] =
        (ecs_script_pending_t){
            .script = script,
            .instance = instance,
            .input = input,
            .depth = rt->template_depth
        };

    if (rt->pending_marker) {
        return;
    }

    rt->pending_marker = true;
    ecs_enqueue(stage, &(ecs_event_desc_t){
        .event = EcsScriptUpdateEvent,
        .entity = EcsAny
    });
}

static void flecs_script_on_update_event(
    ecs_iter_t *it)
{
    ecs_assert(ecs_is_stage(it->stage), ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = it->world;
    ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);

    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    rt->pending_marker = false;
    if (rt->pending_active) {
        return;
    }

    int32_t i, prev_depth = rt->template_depth;
    rt->pending_active = true;
    for (i = 0; i < ecs_vec_count(&rt->pending); i ++) {
        ecs_script_pending_t p = ecs_vec_get_t(
            &rt->pending, ecs_script_pending_t, i)[0];
        rt->template_depth = p.depth;
        flecs_script_ref_dispatch(world, NULL, p.script, p.instance, p.input);
    }

    ecs_vec_clear(&rt->pending);
    rt->pending_active = false;
    rt->template_depth = prev_depth;
}

static void flecs_script_ref_ctx_free(
    void *ptr)
{
    ecs_script_ref_ctx_t *ctx = ptr;
    ecs_os_free(ctx->name);
    ecs_os_free(ctx);
}

static ecs_entity_t flecs_script_create_ref_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    const ecs_script_ref_t *ref)
{
    ecs_entity_t prev_scope = ecs_set_scope(world, script);

    ecs_observer_desc_t desc = {
        .events = { EcsOnSet },
        .callback = ref->is_resolve
            ? flecs_script_resolve_on_set
            : flecs_script_ref_on_set
    };

    ecs_script_ref_ctx_t *ctx = ecs_os_malloc_t(ecs_script_ref_ctx_t);
    ctx->script = script;
    ctx->instance = instance;
    ctx->name = NULL;
    ctx->input = ref->is_resolve ? 0 : ref->input;

    if (ref->is_resolve && ref->name) {
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
            if (!ref->is_resolve) {
                desc.events[1] = EcsOnRemove;
            }
        }
    }

    desc.ctx = ctx;
    desc.ctx_free = flecs_script_ref_ctx_free;

    ecs_entity_t observer = ecs_observer_init(world, &desc);
    if (!observer) {
        flecs_script_ref_ctx_free(ctx);
    }

    ecs_set_scope(world, prev_scope);

    return observer;
}

static void flecs_script_update_ref_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_vec_t *refs,
    ecs_vec_t *observers)
{
    ecs_vec_init_if_t(observers, ecs_script_ref_t);
    ecs_script_ref_t *new_refs = ecs_vec_first(refs);
    int32_t i, new_count = ecs_vec_count(refs);

    for (i = 0; i < new_count; i ++) {
        ecs_script_ref_t value = new_refs[i];
        value.name = value.is_resolve ? value.name : NULL;
        ecs_script_ref_t *old_refs = ecs_vec_first(observers);
        int32_t j, old_count = ecs_vec_count(observers);
        for (j = i; j < old_count; j ++) {
            if (old_refs[j].observer &&
                old_refs[j].input == value.input &&
                old_refs[j].is_resolve == value.is_resolve &&
                flecs_script_ref_matches(&old_refs[j], &value))
            {
                break;
            }
        }

        if (j == old_count) {
            value.observer = flecs_script_create_ref_observer(
                world, script, instance, &value);

            value.name = value.name ? ecs_os_strdup(value.name) : NULL;
            ecs_vec_append_t(NULL, observers, ecs_script_ref_t)[0] = value;
            old_refs = ecs_vec_first(observers);
        }

        value = old_refs[i];
        old_refs[i] = old_refs[j];
        old_refs[j] = value;
    }

    ecs_script_ref_t *old_refs = ecs_vec_first(observers);
    int32_t old_count = ecs_vec_count(observers);
    for (i = new_count; i < old_count; i ++) {
        if (old_refs[i].observer) {
            ecs_delete(world, old_refs[i].observer);
        }

        if (old_refs[i].is_resolve) {
            ecs_os_free(ECS_CONST_CAST(char*, old_refs[i].name));
        }
    }

    ecs_vec_set_count_t(NULL, observers, ecs_script_ref_t, new_count);
}

void flecs_script_observe_refs(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_vec_t *refs,
    ecs_vec_t *observers)
{
    ecs_script_ref_t *array = ecs_vec_first(refs);
    int32_t i;
    for (i = ecs_vec_count(refs) - 1; i >= 0; i --) {
        if (flecs_script_ref_is_owned(world, script, &array[i])) {
            ecs_vec_remove_t(refs, ecs_script_ref_t, i);
        }
    }

    flecs_script_ref_observers_clear(world, observers);
    flecs_script_update_ref_observers(world, script, 0, refs, observers);
}

void flecs_script_record_dyn_ref(
    ecs_vec_t *run_refs,
    ecs_entity_t entity,
    ecs_id_t component,
    uint64_t input,
    bool is_has)
{
    flecs_script_ref_add(run_refs, &(ecs_script_ref_t){
        .entity = entity,
        .component = component,
        .input = input,
        .is_has = is_has
    }, true);
}

void flecs_script_update_dyn_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_vec_t *run_refs,
    ecs_vec_t *observers,
    uint64_t input)
{
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

        if (flecs_script_ref_is_owned(world, script, &new_refs[i])) {
            continue;
        }

        ecs_script_ref_t *ref = ecs_vec_append_t(
            NULL, &refs, ecs_script_ref_t);
        *ref = new_refs[i];
        ref->observer = 0;
    }

    flecs_script_update_ref_observers(
        world, script, instance, &refs, observers);

    ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
}

void flecs_script_update_resolve_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_script_impl_t *impl,
    ecs_vec_t *observers)
{
    int32_t i, ref_count = ecs_vec_count(&impl->unresolved_refs);
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

        flecs_script_ref_ensure(&resolve_refs, &(ecs_script_ref_t){
            .name = name,
            .component = component
        })->is_resolve = true;
    }

    for (i = 0; i < component_ref_count; i ++) {
        flecs_script_ref_ensure(&resolve_refs, &(ecs_script_ref_t){
            .entity = component_refs[i].entity,
            .component = component_refs[i].component,
            .is_has = component_refs[i].is_has
        })->is_resolve = true;
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

    flecs_script_update_ref_observers(
        world, script, 0, &resolve_refs, observers);

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
    int32_t i, count = ecs_vec_count(observers);
    ecs_script_ref_t *refs = ecs_vec_first(observers);
    for (i = 0; i < count; i ++) {
        if (refs[i].observer && ecs_is_alive(world, refs[i].observer)) {
            ecs_delete(world, refs[i].observer);
        }
    }

    flecs_script_ref_observers_fini(observers);
    ecs_vec_init_t(NULL, observers, ecs_script_ref_t, 0);
}

void flecs_script_refs_import(
    ecs_world_t *world)
{
    ECS_TAG_DEFINE(world, EcsScriptUpdateEvent);

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "ScriptUpdateObserver" }),
        .query.terms = {{ .id = EcsAny }},
        .events = { EcsScriptUpdateEvent },
        .callback = flecs_script_on_update_event
    });
}

#endif
