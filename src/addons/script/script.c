/**
 * @file addons/script/script.c
 * @brief Script API.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ECS_COMPONENT_DECLARE(EcsScript);

static
ECS_MOVE(EcsScript, dst, src, {
    if (dst->script && (dst->script != src->script)) {
        if (dst->assembly && (dst->assembly != src->assembly)) {
            flecs_script_assembly_fini(dst->script, dst->assembly);
        }
        ecs_script_free(dst->script);
    }
    dst->script = src->script;
    dst->assembly = src->assembly;
    src->script = NULL;
    src->assembly = NULL;
})

static
ECS_DTOR(EcsScript, ptr, {
    if (ptr->assembly) {
        flecs_script_assembly_fini(ptr->script, ptr->assembly);
    }
    if (ptr->script) {
        ecs_script_free(ptr->script);
    }
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

ecs_script_t* flecs_script_new(
    ecs_world_t *world) 
{
    ecs_script_t *result = ecs_os_calloc_t(ecs_script_t);
    flecs_allocator_init(&result->allocator);
    ecs_script_parser_t parser = { .script = result };
    result->root = flecs_script_scope_new(&parser);
    result->world = world;
    result->refcount = 1;
    return result;
}

void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance)
{
    ecs_delete_with(world, flecs_script_tag(script, instance));
}

int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *code)
{
    ecs_script_t *script = ecs_script_parse(world, name, code);
    if (!script) {
        goto error;
    }

    ecs_entity_t prev_scope = ecs_set_scope(world, 0);

    if (ecs_script_eval(script)) {
        goto error_free;
    }

    ecs_set_scope(world, prev_scope);

    ecs_script_free(script);
    return 0;
error_free:
    ecs_script_free(script);
error:
    return -1;
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

void ecs_script_free(
    ecs_script_t *script)
{
    ecs_check(script->refcount > 0, ECS_INVALID_OPERATION, NULL);
    if (!--script->refcount) {
        flecs_script_visit_free(script);
        flecs_free(&script->allocator, 
            script->token_buffer_size, script->token_buffer);
        flecs_allocator_fini(&script->allocator);
        ecs_os_free(script);
    }
error:
    return;
}

int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(code != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *name = ecs_get_name(world, e);
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    if (s->assembly) {
        char *assembly_name = ecs_get_fullpath(world, s->assembly->entity);
        ecs_err("cannot update scripts for individual assemblies, "
            "update parent script instead (tried to update '%s')",
                assembly_name);
        ecs_os_free(assembly_name);
        return -1;
    }

    if (s->script) {
        ecs_script_free(s->script);
    }

    s->script = ecs_script_parse(world, name, code);
    if (!s->script) {
        return -1;
    }

    int result = 0;
    bool is_defer = ecs_is_deferred(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
        ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);
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

    script = desc->code;
    if (!script && desc->filename) {
        script = flecs_load_from_file(desc->filename);
        if (!script) {
            goto error;
        }
    }

    if (ecs_script_update(world, e, 0, script)) {
        goto error;
    }

    if (script != desc->code) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }

    return e;
error:
    if (script != desc->code) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }
    if (!desc->entity) {
        ecs_delete(world, e);
    }
    return 0;
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
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScript),
        .dtor = ecs_dtor(EcsScript)
    });

    ecs_add_id(world, ecs_id(EcsScript), EcsPairIsTag);
    ecs_add_id(world, ecs_id(EcsScript), EcsPrivate);
}

#endif
