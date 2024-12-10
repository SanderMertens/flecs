/**
 * @file addons/script/function.c
 * @brief Script function API.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static
void ecs_script_params_free(ecs_vec_t *params) {
    ecs_script_parameter_t *array = ecs_vec_first(params);
    int32_t i, count = ecs_vec_count(params);
    for (i = 0; i < count; i ++) {
        /* Safe, component owns string */
        ecs_os_free(ECS_CONST_CAST(char*, array[i].name));
    }
    ecs_vec_fini_t(NULL, params, ecs_script_parameter_t);
}

static
ECS_MOVE(EcsScriptFunction, dst, src, {
    ecs_script_params_free(&dst->params);
    *dst = *src;
    ecs_os_zeromem(src);
})

static
ECS_DTOR(EcsScriptFunction, ptr, {
    ecs_script_params_free(&ptr->params);
})

static
ECS_MOVE(EcsScriptMethod, dst, src, {
    ecs_script_params_free(&dst->params);
    *dst = *src;
    ecs_os_zeromem(src);
})

static
ECS_DTOR(EcsScriptMethod, ptr, {
    ecs_script_params_free(&ptr->params);
})

ecs_entity_t ecs_function_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->return_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = ecs_entity(world, { 
        .name = desc->name,
        .parent = desc->parent
    });

    EcsScriptFunction *f = ecs_ensure(world, result, EcsScriptFunction);
    f->return_type = desc->return_type;
    f->callback = desc->callback;
    f->ctx = desc->ctx;

    int32_t i;
    for (i = 0; i < FLECS_SCRIPT_FUNCTION_ARGS_MAX; i ++) {
        if (!desc->params[i].name) {
            break;
        }

        if (!i) {
            ecs_vec_init_t(NULL, &f->params, ecs_script_parameter_t, 0);
        }

        ecs_script_parameter_t *p = ecs_vec_append_t(
            NULL, &f->params, ecs_script_parameter_t);
        p->type = desc->params[i].type;
        p->name = ecs_os_strdup(desc->params[i].name);
    }

    ecs_modified(world, result, EcsScriptFunction);

    return result;
}

ecs_entity_t ecs_method_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->parent != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->return_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = ecs_entity(world, { 
        .name = desc->name,
        .parent = desc->parent
    });

    EcsScriptMethod *f = ecs_ensure(world, result, EcsScriptMethod);
    f->return_type = desc->return_type;
    f->callback = desc->callback;
    f->ctx = desc->ctx;

    int32_t i;
    for (i = 0; i < FLECS_SCRIPT_FUNCTION_ARGS_MAX; i ++) {
        if (!desc->params[i].name) {
            break;
        }

        if (!i) {
            ecs_vec_init_t(NULL, &f->params, ecs_script_parameter_t, 0);
        }

        ecs_script_parameter_t *p = ecs_vec_append_t(
            NULL, &f->params, ecs_script_parameter_t);
        p->type = desc->params[i].type;
        p->name = ecs_os_strdup(desc->params[i].name);
    }

    ecs_modified(world, result, EcsScriptMethod);

    return result;
}

void flecs_function_import(
    ecs_world_t *world)
{
    ecs_set_name_prefix(world, "EcsScript");
    ECS_COMPONENT_DEFINE(world, EcsScriptFunction);
    ECS_COMPONENT_DEFINE(world, EcsScriptMethod);

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptFunction),
        .members = {
            { .name = "return_type", .type = ecs_id(ecs_entity_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptMethod),
        .members = {
            { .name = "return_type", .type = ecs_id(ecs_entity_t) }
        }
    });

    ecs_set_hooks(world, EcsScriptFunction, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsScriptFunction),
        .move = ecs_move(EcsScriptFunction),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_set_hooks(world, EcsScriptMethod, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsScriptMethod),
        .move = ecs_move(EcsScriptMethod),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    flecs_script_register_builtin_functions(world);
}

#endif
