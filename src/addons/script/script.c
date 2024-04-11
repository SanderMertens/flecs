#include "flecs.h"
#include "script.h"

ECS_COMPONENT_DECLARE(EcsScript);

static 
void flecs_dtor_script(EcsScript *ptr) {
    // ecs_vec_fini_t(NULL, &ptr->using_, ecs_entity_t);

    // int i, count = ptr->prop_defaults.count;
    // ecs_value_t *values = ptr->prop_defaults.array;
    // for (i = 0; i < count; i ++) {
    //     ecs_value_free(ptr->script->world, values[i].type, values[i].ptr);
    // }

    // ecs_vec_fini_t(&ptr->script->allocator, &ptr->prop_defaults, ecs_value_t);
}

static
ECS_MOVE(EcsScript, dst, src, {
    flecs_dtor_script(dst);
    dst->script = src->script;
    dst->assembly = src->assembly;
    src->script = NULL;
})

static
ECS_DTOR(EcsScript, ptr, {
    flecs_dtor_script(ptr);
})

static
ecs_id_t flecs_script_tag(
    ecs_entity_t script,
    ecs_entity_t instance)
{
    if (!instance) {
        return ecs_pair_t(EcsScript, script);
    } else {
        return ecs_pair(EcsChildOf, instance);
    }
}

static
int flecs_script_run(
    ecs_world_t *world,
    ecs_entity_t script_handle,
    ecs_entity_t instance,
    const char *name,
    const char *code)
{
    ecs_script_t *script = ecs_script_parse(world, name, code);
    if (!script) {
        return -1;
    }

    ecs_entity_t prev_scope = ecs_set_scope(world, 0);

    // printf("%s\n", ecs_script_to_str(script));

    if (ecs_script_eval(script)) {
        return -1;
    }

    ecs_set_scope(world, prev_scope);

    // ecs_script_free(script);
    return 0;
}

void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance)
{
    ecs_delete_with(world, flecs_script_tag(script, instance));
}

int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code,
    ecs_vars_t *vars)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(code != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *name = ecs_get_name(world, e);
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    s->script = ecs_script_parse(world, name, code);
    if (!s->script) {
        return -1;
    }

    int result = 0;
    bool is_defer = ecs_is_deferred(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
        ecs_assert(ecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_script_clear(world, e, instance);

    ecs_entity_t prev = ecs_set_with(world, flecs_script_tag(e, instance));

    if (ecs_script_eval(s->script)) {
        ecs_delete_with(world, ecs_pair_t(EcsScript, e));
        result = -1;
    }

    ecs_set_with(world, prev);

    if (is_defer) {
        flecs_resume_readonly(real_world, &srs);
    }

    return result;
}

ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc)
{
    const char *script = NULL;
    ecs_entity_t e = desc->entity;
    
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!e) {
        if (desc->filename) {
            e = ecs_new_from_path_w_sep(world, 0, desc->filename, "/", NULL);
        } else {
            e = ecs_new(world);
        }
    }

    script = desc->str;
    if (!script && desc->filename) {
        script = flecs_load_from_file(desc->filename);
        if (!script) {
            goto error;
        }
    }

    if (ecs_script_update(world, e, 0, script, NULL)) {
        goto error;
    }

    if (script != desc->str) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }

    return e;
error:
    if (script != desc->str) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }
    if (!desc->entity) {
        ecs_delete(world, e);
    }
    return 0;
}

int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *code)
{
    return flecs_script_run(world, 0, 0, name, code);
}

int ecs_script_run_file(
    ecs_world_t *world,
    const char *filename) 
{
    char *script = flecs_load_from_file(filename);
    if (!script) {
        return -1;
    }

    int result = ecs_script_run(world, filename, script);
    ecs_os_free(script);
    return result;
}

void FlecsScriptImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsScript);
    ECS_IMPORT(world, FlecsMeta);
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
    ecs_doc_set_brief(world, ecs_id(FlecsScript), 
        "Module with components for managing Flecs scripts");
#endif

    ecs_set_name_prefix(world, "Ecs");
    ECS_COMPONENT_DEFINE(world, EcsScript);

    ecs_set_hooks(world, EcsScript, {
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsScript),
        .dtor = ecs_dtor(EcsScript)
    });

    ecs_add_id(world, ecs_id(EcsScript), EcsPairIsTag);
    ecs_add_id(world, ecs_id(EcsScript), EcsPrivate);
}
