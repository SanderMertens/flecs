/**
 * @file addons/script/requirements.c
 * @brief Track requirements (external references) of scripts.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ECS_COMPONENT_DECLARE(EcsScriptRequirements);
ECS_COMPONENT_DECLARE(EcsScriptRefChangedEvent);

void flecs_script_refs_fini(
    ecs_vec_t *refs)
{
    int32_t i, count = ecs_vec_count(refs);
    ecs_script_requirement_t *elems = ecs_vec_first(refs);
    for (i = 0; i < count; i ++) {
        ecs_os_free(elems[i].entity);
        ecs_os_free(elems[i].var);
        ecs_os_free(elems[i].scope);
        ecs_os_free(elems[i].component);
    }
    ecs_vec_fini_t(NULL, refs, ecs_script_requirement_t);
}

void flecs_script_strings_fini(
    ecs_vec_t *strings)
{
    int32_t i, count = ecs_vec_count(strings);
    char **elems = ecs_vec_first(strings);
    for (i = 0; i < count; i ++) {
        ecs_os_free(elems[i]);
    }
    ecs_vec_fini_t(NULL, strings, char*);
}

static
void flecs_script_requirements_free(
    EcsScriptRequirements *ptr)
{
    flecs_script_refs_fini(&ptr->requirements);
    flecs_script_strings_fini(&ptr->usings);
    ecs_vec_fini_t(NULL, &ptr->observers, ecs_script_ref_observer_t);
}

static
ECS_MOVE(EcsScriptRequirements, dst, src, {
    flecs_script_requirements_free(dst);
    dst->requirements = src->requirements;
    dst->observers = src->observers;
    dst->usings = src->usings;
    dst->met = src->met;
    ecs_os_zeromem(&src->requirements);
    ecs_os_zeromem(&src->observers);
    ecs_os_zeromem(&src->usings);
})

static
ECS_DTOR(EcsScriptRequirements, ptr, {
    flecs_script_requirements_free(ptr);
})

void flecs_script_requirements_delete_observers(
    ecs_world_t *world,
    EcsScriptRequirements *reqs)
{
    int32_t i, count = ecs_vec_count(&reqs->observers);
    ecs_script_ref_observer_t *observers = ecs_vec_first(&reqs->observers);
    for (i = 0; i < count; i ++) {
        if (observers[i].observer &&
            ecs_is_alive(world, observers[i].observer))
        {
            ecs_delete(world, observers[i].observer);
        }
        observers[i].observer = 0;
    }
}

static
void flecs_script_requirements_on_remove(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    EcsScriptRequirements *ptr = ecs_field_w_size(
        it, sizeof(EcsScriptRequirements), 0);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        flecs_script_requirements_delete_observers(world, &ptr[i]);
    }
}

static
ecs_entity_t flecs_script_requirement_lookup(
    const ecs_world_t *world,
    const char *scope,
    const char *name,
    const ecs_vec_t *usings)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t result = 0;
    ecs_size_t name_len = ecs_os_strlen(name);

    if (scope) {
        ecs_size_t len = ecs_os_strlen(scope);
        char *path = ecs_os_malloc(len + name_len + 2);
        while (len) {
            ecs_os_memcpy(path, scope, len);
            path[len] = '.';
            ecs_os_memcpy(&path[len + 1], name, name_len + 1);
            result = ecs_lookup(world, path);
            if (result) {
                break;
            }

            while (len && scope[len - 1] != '.') {
                len --;
            }
            if (len) {
                len --;
            }
        }
        ecs_os_free(path);
    }

    if (!result) {
        result = ecs_lookup(world, name);
    }

    if (!result && usings) {
        int32_t i, count = ecs_vec_count(usings);
        char **elems = ecs_vec_first(usings);
        for (i = count - 1; i >= 0 && !result; i --) {
            const char *using_name = elems[i];
            ecs_size_t len = ecs_os_strlen(using_name);
            if (len > 2 && !ecs_os_strcmp(&using_name[len - 2], ".*")) {
                len -= 2;
            }

            char *path = ecs_os_malloc(len + name_len + 2);
            ecs_os_memcpy(path, using_name, len);
            path[len] = '.';
            ecs_os_memcpy(&path[len + 1], name, name_len + 1);
            result = ecs_lookup(world, path);
            ecs_os_free(path);
        }
    }

    return result;
}

static
ecs_id_t flecs_script_requirement_component(
    const ecs_world_t *world,
    const ecs_script_requirement_t *req,
    const ecs_vec_t *usings)
{
    if (req->component_id) {
        return req->component_id;
    }
    if (!req->component) {
        return 0;
    }
    return flecs_script_requirement_lookup(
        world, req->scope, req->component, usings);
}

static
bool flecs_script_requirement_met(
    const ecs_world_t *world,
    const ecs_script_requirement_t *req,
    const ecs_script_vars_t *vars,
    const ecs_vec_t *usings)
{
    ecs_id_t component = flecs_script_requirement_component(
        world, req, usings);
    if (!component) {
        /* Unresolved component. If the reference was made from a scope that
         * can't be statically resolved, let evaluation decide. */
        return req->dynamic_scope;
    }

    ecs_entity_t entity = 0;
    if (req->var) {
        if (!vars) {
            return true;
        }

        const ecs_script_var_t *var = ecs_script_vars_lookup(vars, req->var);
        if (!var || var->value.type != ecs_id(ecs_entity_t) ||
            !var->value.ptr)
        {
            return true;
        }

        entity = *(ecs_entity_t*)var->value.ptr;
        if (!entity) {
            return true;
        }

        if (!ecs_is_alive(world, entity)) {
            return false;
        }
    } else if (req->entity) {
        entity = flecs_script_requirement_lookup(
            world, req->scope, req->entity, usings);
        if (!entity) {
            return req->dynamic_scope;
        }
    } else {
        /* Reference can't be statically resolved, let evaluation decide. */
        return true;
    }

    return ecs_has_id(world, entity, component);
}

bool flecs_script_requirements_met(
    const ecs_world_t *world,
    const ecs_vec_t *requirements,
    const ecs_script_vars_t *vars,
    const ecs_vec_t *usings)
{
    int32_t i, count = ecs_vec_count(requirements);
    const ecs_script_requirement_t *elems = ecs_vec_first(requirements);
    for (i = 0; i < count; i ++) {
        if (!flecs_script_requirement_met(world, &elems[i], vars, usings)) {
            return false;
        }
    }
    return true;
}

static
ecs_entity_t flecs_script_create_ref_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_id_t component,
    ecs_iter_action_t callback)
{
    ecs_entity_t prev_scope = ecs_set_scope(world, script);
    ecs_entity_t prev_with = ecs_set_with(world, 0);

    ecs_observer_desc_t desc = {
        .query.terms = {{ .id = component }},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = callback,
        .ctx = (void*)(uintptr_t)script
    };

    ecs_entity_t observer = ecs_observer_init(world, &desc);

    ecs_set_with(world, prev_with);
    ecs_set_scope(world, prev_scope);

    return observer;
}

void flecs_script_requirements_update(
    ecs_world_t *world,
    ecs_entity_t script,
    const ecs_vec_t *refs,
    const ecs_vec_t *usings,
    bool met,
    ecs_iter_action_t callback)
{
    EcsScriptRequirements *reqs = ecs_ensure(
        world, script, EcsScriptRequirements);
    ecs_assert(reqs != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Collect unique component ids from requirements */
    ecs_vec_t components;
    ecs_vec_init_t(NULL, &components, ecs_id_t, 0);

    int32_t i, j, ref_count = ecs_vec_count(refs);
    const ecs_script_requirement_t *ref_elems = ecs_vec_first(refs);
    for (i = 0; i < ref_count; i ++) {
        ecs_id_t component = flecs_script_requirement_component(
            world, &ref_elems[i], usings);
        if (!component) {
            continue;
        }

        int32_t component_count = ecs_vec_count(&components);
        ecs_id_t *component_elems = ecs_vec_first(&components);
        for (j = 0; j < component_count; j ++) {
            if (component_elems[j] == component) {
                break;
            }
        }
        if (j == component_count) {
            ecs_vec_append_t(NULL, &components, ecs_id_t)[0] = component;
        }
    }

    /* Reuse existing observers for components that are still referenced,
     * create observers for new components, delete stale observers. */
    ecs_script_ref_observer_t *old_observers = ecs_vec_first(&reqs->observers);
    int32_t old_count = ecs_vec_count(&reqs->observers);

    ecs_vec_t observers;
    int32_t component_count = ecs_vec_count(&components);
    ecs_id_t *component_elems = ecs_vec_first(&components);
    ecs_vec_init_t(NULL, &observers, ecs_script_ref_observer_t,
        component_count);

    for (i = 0; i < component_count; i ++) {
        ecs_id_t component = component_elems[i];
        ecs_entity_t observer = 0;

        for (j = 0; j < old_count; j ++) {
            if (old_observers[j].observer &&
                old_observers[j].component == component)
            {
                observer = old_observers[j].observer;
                old_observers[j].observer = 0;
                break;
            }
        }

        if (!observer) {
            observer = flecs_script_create_ref_observer(
                world, script, component, callback);
        }

        ecs_script_ref_observer_t *elem = ecs_vec_append_t(
            NULL, &observers, ecs_script_ref_observer_t);
        elem->component = component;
        elem->observer = observer;
    }

    for (j = 0; j < old_count; j ++) {
        if (old_observers[j].observer) {
            ecs_delete(world, old_observers[j].observer);
        }
    }

    ecs_vec_fini_t(NULL, &components, ecs_id_t);

    /* Copy requirements & usings into component */
    ecs_vec_t requirements;
    ecs_vec_init_t(NULL, &requirements, ecs_script_requirement_t, ref_count);
    for (i = 0; i < ref_count; i ++) {
        const ecs_script_requirement_t *src = &ref_elems[i];
        ecs_script_requirement_t *dst = ecs_vec_append_t(
            NULL, &requirements, ecs_script_requirement_t);
        dst->entity = src->entity ? ecs_os_strdup(src->entity) : NULL;
        dst->var = src->var ? ecs_os_strdup(src->var) : NULL;
        dst->scope = src->scope ? ecs_os_strdup(src->scope) : NULL;
        dst->component = src->component ? ecs_os_strdup(src->component) : NULL;
        dst->component_id = src->component_id;
        dst->dynamic_scope = src->dynamic_scope;
    }

    ecs_vec_t using_names;
    int32_t using_count = usings ? ecs_vec_count(usings) : 0;
    ecs_vec_init_t(NULL, &using_names, char*, using_count);
    for (i = 0; i < using_count; i ++) {
        ecs_vec_append_t(NULL, &using_names, char*)[0] =
            ecs_os_strdup(ecs_vec_get_t(usings, char*, i)[0]);
    }

    flecs_script_requirements_free(reqs);
    reqs->requirements = requirements;
    reqs->observers = observers;
    reqs->usings = using_names;
    reqs->met = met;

    ecs_modified(world, script, EcsScriptRequirements);
}

static
bool flecs_script_requirement_created_by(
    const ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t template_component,
    ecs_entity_t entity)
{
    if (template_component) {
        return ecs_has_pair(world, entity, EcsScriptTemplate,
            template_component);
    }
    return ecs_has_pair(world, entity, ecs_id(EcsScript), script);
}

bool flecs_script_requirements_should_reeval(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t template_component,
    ecs_iter_t *it)
{
    const EcsScriptRequirements *reqs = ecs_get(
        world, script, EcsScriptRequirements);
    if (!reqs) {
        return true;
    }

    ecs_id_t component = ecs_field_id(it, 0);

    int32_t i, e, count = ecs_vec_count(&reqs->requirements);
    const ecs_script_requirement_t *elems = ecs_vec_first(&reqs->requirements);
    for (i = 0; i < count; i ++) {
        const ecs_script_requirement_t *req = &elems[i];
        ecs_id_t req_component = flecs_script_requirement_component(
            world, req, &reqs->usings);
        if (req_component != component) {
            continue;
        }

        if (req->var) {
            if (template_component && !ecs_os_strcmp(req->var, "this")) {
                /* Reference to template instance: only reevaluate if the
                 * component changed for an instance of the template. */
                for (e = 0; e < it->count; e ++) {
                    if (ecs_has_id(world, it->entities[e],
                        template_component))
                    {
                        return true;
                    }
                }
                continue;
            }

            /* Reference through variable, can't be statically resolved. Only
             * skip changes for entities the script created itself, so that
             * cleaning up script output doesn't retrigger evaluation. */
            for (e = 0; e < it->count; e ++) {
                if (!flecs_script_requirement_created_by(
                    world, script, template_component, it->entities[e]))
                {
                    return true;
                }
            }
            continue;
        }

        if (req->entity) {
            ecs_entity_t entity = flecs_script_requirement_lookup(
                world, req->scope, req->entity, &reqs->usings);
            if (!entity) {
                return true;
            }

            for (e = 0; e < it->count; e ++) {
                if (it->entities[e] == entity) {
                    return true;
                }
            }
            continue;
        }

        return true;
    }

    return false;
}

static
void flecs_script_ref_changed_handle(
    ecs_world_t *world,
    ecs_entity_t script);

void flecs_script_enqueue_ref_changed(
    ecs_world_t *world,
    ecs_entity_t script)
{
    if (!ecs_is_deferred(world)) {
        flecs_script_ref_changed_handle(ECS_CONST_CAST(ecs_world_t*,
            ecs_get_world(world)), script);
        return;
    }

    EcsScriptRefChangedEvent evt = {
        .script = script
    };

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(EcsScriptRefChangedEvent),
        .entity = EcsAny,
        .param = &evt
    });
}

static
void flecs_script_ref_changed_handle(
    ecs_world_t *world,
    ecs_entity_t script)
{
    if (!ecs_is_alive(world, script)) {
        return;
    }

    const EcsScript *s = ecs_get(world, script, EcsScript);
    if (!s) {
        return;
    }

    if (s->template_) {
        flecs_script_template_reinstantiate_all(world, script);
        return;
    }

    if (!s->code) {
        return;
    }

    if (s->script && flecs_script_impl(s->script)->evaluating) {
        return;
    }

    char *code = ecs_os_strdup(s->code);
    ecs_script_update(world, script, 0, code);
    ecs_os_free(code);
}

static
void flecs_script_on_ref_changed_event(
    ecs_iter_t *it)
{
    EcsScriptRefChangedEvent *evt = it->param;
    ecs_world_t *world = it->real_world;

    if (ecs_is_fini(world)) {
        return;
    }

    flecs_script_ref_changed_handle(world, evt->script);
}

void flecs_script_ref_on_set(
    ecs_iter_t *it)
{
    ecs_entity_t script = (ecs_entity_t)(uintptr_t)it->ctx;
    ecs_world_t *world = it->real_world;

    if (ecs_is_fini(world)) {
        return;
    }

    if (!ecs_is_alive(world, script)) {
        return;
    }

    const EcsScript *s = ecs_get(world, script, EcsScript);
    if (!s || !s->code) {
        return;
    }

    if (s->script && flecs_script_impl(s->script)->evaluating) {
        return;
    }

    if (!flecs_script_requirements_should_reeval(world, script, 0, it)) {
        return;
    }

    flecs_script_enqueue_ref_changed(it->world, script);
}

void flecs_script_requirements_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptRequirements);
    ECS_COMPONENT_DEFINE(world, EcsScriptRefChangedEvent);

    ecs_set_hooks(world, EcsScriptRequirements, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScriptRequirements),
        .dtor = ecs_dtor(EcsScriptRequirements),
        .on_remove = flecs_script_requirements_on_remove,
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_add_pair(world, ecs_id(EcsScriptRequirements),
        EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t requirement_type = ecs_struct(world, {
        .entity = ecs_entity(world, {
            .name = "ecs_script_requirement_t"
        }),
        .members = {
            { .name = "entity", .type = ecs_id(ecs_string_t) },
            { .name = "var", .type = ecs_id(ecs_string_t) },
            { .name = "scope", .type = ecs_id(ecs_string_t) },
            { .name = "component", .type = ecs_id(ecs_string_t) },
            { .name = "component_id", .type = ecs_id(ecs_id_t) },
            { .name = "dynamic_scope", .type = ecs_id(ecs_bool_t) }
        }
    });

    ecs_entity_t requirement_vec = ecs_vector(world, {
        .entity = ecs_entity(world, {
            .name = "ecs_script_requirement_vec_t"
        }),
        .type = requirement_type
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptRequirements),
        .members = {
            {
                .name = "requirements",
                .type = requirement_vec,
                .offset = offsetof(EcsScriptRequirements, requirements)
            },
            {
                .name = "met",
                .type = ecs_id(ecs_bool_t),
                .offset = offsetof(EcsScriptRequirements, met)
            }
        }
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "ScriptRefChangedObserver" }),
        .query.terms = {{ .id = EcsAny }},
        .events = { ecs_id(EcsScriptRefChangedEvent) },
        .callback = flecs_script_on_ref_changed_event
    });
}

#endif
