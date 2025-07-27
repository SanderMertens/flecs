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
ECS_MOVE(EcsScriptConstVar, dst, src, {
    if (dst->value.ptr) {
        ecs_assert(dst->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

        if (dst->type_info->hooks.dtor) {
            dst->type_info->hooks.dtor(dst->value.ptr, 1, dst->type_info);
        }

        ecs_os_free(dst->value.ptr);
    }
    
    *dst = *src;

    src->value.ptr = NULL;
    src->value.type = 0;
    src->type_info = NULL;
})

static
ECS_DTOR(EcsScriptConstVar, ptr, {
    if (ptr->value.ptr) {
        ecs_assert(ptr->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
        if (ptr->type_info->hooks.dtor) {
            ptr->type_info->hooks.dtor(ptr->value.ptr, 1, ptr->type_info);
        }
    }
    ecs_os_free(ptr->value.ptr);
})

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

ecs_entity_t ecs_const_var_init(
    ecs_world_t *world,
    ecs_const_var_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->type != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->value != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (ecs_lookup_child(world, desc->parent, desc->name) != 0) {
        ecs_err("cannot redeclare const variable entity '%s' in parent '%s'", 
            desc->name, flecs_errstr(ecs_get_path(world, desc->parent)));
        return 0;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, desc->type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "ecs_const_var_desc_t::type is not a valid type");

    ecs_entity_t result = ecs_entity(world, { 
        .name = desc->name,
        .parent = desc->parent
    });

    if (!result) {
        goto error;
    }

    EcsScriptConstVar *v = ecs_ensure(world, result, EcsScriptConstVar);
    v->value.ptr = ecs_os_malloc(ti->size);
    v->value.type = desc->type;
    v->type_info = ti;
    ecs_value_init(world, desc->type, v->value.ptr);
    ecs_value_copy(world, desc->type, v->value.ptr, desc->value);
    ecs_modified(world, result, EcsScriptConstVar);

    return result;
error:
    return 0;
}

ecs_value_t ecs_const_var_get(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const EcsScriptConstVar *v = ecs_get(world, entity, EcsScriptConstVar);
    if (!v) {
        goto error;
    }

    return v->value;
error:
    return (ecs_value_t){0};
}

ecs_entity_t ecs_function_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->return_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = ecs_entity(world, { 
        .name = desc->name,
        .parent = desc->parent
    });

    if (!result) {
        goto error;
    }

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
error:
    return 0;
}

ecs_entity_t ecs_method_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->parent != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->return_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = ecs_entity(world, { 
        .name = desc->name,
        .parent = desc->parent
    });

    if (!result) {
        goto error;
    }

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
error:
    return 0;
}

void flecs_function_import(
    ecs_world_t *world)
{
    ecs_set_name_prefix(world, "EcsScript");
    ECS_COMPONENT_DEFINE(world, EcsScriptConstVar);
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

    ecs_set_hooks(world, EcsScriptConstVar, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsScriptConstVar),
        .move = ecs_move(EcsScriptConstVar),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
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
