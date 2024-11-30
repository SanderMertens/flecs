/**
 * @file addons/script/builtin_functions.c
 * @brief Builtin script functions.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static
void flecs_meta_entity_name(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    *(char**)result->ptr = ecs_get_name(ctx->world, entity);
}

static
void flecs_meta_entity_path(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    *(char**)result->ptr = ecs_get_path(ctx->world, entity);
}

static
void flecs_meta_entity_parent(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    *(ecs_entity_t*)result->ptr = ecs_get_parent(ctx->world, entity);
}

#ifdef FLECS_DOC

static
void flecs_meta_entity_doc_name(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    *(char**)result->ptr = ecs_doc_get_name(ctx->world, entity);
}

void flecs_script_register_builtin_doc_functions(
    ecs_world_t *world)
{
    ecs_entity_t name = ecs_entity(world, {
        .name = "doc_name",
        .parent = ecs_id(ecs_entity_t),
        .set = ecs_values(ecs_value(EcsScriptMethod, { 
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_name
        }))
    });

    ecs_doc_set_brief(world, name, "Returns entity doc name");
}

#else
void flecs_script_register_builtin_doc_functions(
    ecs_world_t *world)
{
    (void)world;
}
#endif

void flecs_script_register_builtin_functions(
    ecs_world_t *world)
{
    ecs_entity_t name = ecs_entity(world, {
        .name = "name",
        .parent = ecs_id(ecs_entity_t),
        .set = ecs_values(ecs_value(EcsScriptMethod, { 
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_name
        }))
    });

    ecs_doc_set_brief(world, name, "Returns entity name");

    ecs_entity_t path = ecs_entity(world, {
        .name = "path",
        .parent = ecs_id(ecs_entity_t),
        .set = ecs_values(ecs_value(EcsScriptMethod, { 
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_path
        }))
    });

    ecs_doc_set_brief(world, path, "Returns entity path");

    ecs_entity_t parent = ecs_entity(world, {
        .name = "parent",
        .parent = ecs_id(ecs_entity_t),
        .set = ecs_values(ecs_value(EcsScriptMethod, { 
            .return_type = ecs_id(ecs_entity_t),
            .callback = flecs_meta_entity_parent
        }))
    });

    ecs_doc_set_brief(world, parent, "Returns entity parent");

    flecs_script_register_builtin_doc_functions(world);
}

#endif
