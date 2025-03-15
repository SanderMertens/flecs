/**
 * @file addons/script/builtin_functions.c
 * @brief Flecs functions for flecs script.
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
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    *(char**)result->ptr = ecs_os_strdup(ecs_get_name(ctx->world, entity));
}

static
void flecs_meta_entity_path(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
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
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    *(ecs_entity_t*)result->ptr = ecs_get_parent(ctx->world, entity);
}

static
void flecs_meta_entity_has(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    ecs_id_t id = *(ecs_id_t*)argv[1].ptr;
    *(ecs_bool_t*)result->ptr = ecs_has_id(ctx->world, entity, id);
}

static
void flecs_meta_core_pair(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    (void)ctx;
    ecs_entity_t first = *(ecs_entity_t*)argv[0].ptr;
    ecs_entity_t second = *(ecs_entity_t*)argv[1].ptr;
    *(ecs_id_t*)result->ptr = ecs_pair(first, second);
}

#ifdef FLECS_DOC

#define FLECS_DOC_FUNC(name)\
    static\
    void flecs_meta_entity_doc_##name(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result)\
    {\
        (void)argc;\
        ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;\
        *(char**)result->ptr = \
            ecs_os_strdup(ecs_doc_get_##name(ctx->world, entity));\
    }

FLECS_DOC_FUNC(name)
FLECS_DOC_FUNC(uuid)
FLECS_DOC_FUNC(brief)
FLECS_DOC_FUNC(detail)
FLECS_DOC_FUNC(link)
FLECS_DOC_FUNC(color)

#undef FLECS_DOC_FUNC

static
void flecs_script_register_builtin_doc_functions(
    ecs_world_t *world)
{
    {
        ecs_entity_t m = ecs_method(world, {
            .name = "doc_name",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_name
        });

        ecs_doc_set_brief(world, m, "Returns entity doc name");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "doc_uuid",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_uuid
        });

        ecs_doc_set_brief(world, m, "Returns entity doc uuid");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "doc_brief",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_brief
        });

        ecs_doc_set_brief(world, m, "Returns entity doc brief description");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "doc_detail",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_detail
        });

        ecs_doc_set_brief(world, m, "Returns entity doc detailed description");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "doc_link",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_link
        });

        ecs_doc_set_brief(world, m, "Returns entity doc link");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "doc_color",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_doc_color
        });

        ecs_doc_set_brief(world, m, "Returns entity doc color");
    }
}

#else

static
void flecs_script_register_builtin_doc_functions(
    ecs_world_t *world)
{
    (void)world;
}

#endif

void flecs_script_register_builtin_functions(
    ecs_world_t *world)
{
    {
        ecs_entity_t m = ecs_method(world, {
            .name = "name",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_name
        });

        ecs_doc_set_brief(world, m, "Returns entity name");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "path",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_meta_entity_path
        });

        ecs_doc_set_brief(world, m, "Returns entity path");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "parent",
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_entity_t),
            .callback = flecs_meta_entity_parent
        });

        ecs_doc_set_brief(world, m, "Returns entity parent");
    }

    {
        ecs_entity_t m = ecs_method(world, {
            .name = "has",
            .parent = ecs_id(ecs_entity_t),
            .params = {
                { .name = "component", .type = ecs_id(ecs_id_t) }
            },
            .return_type = ecs_id(ecs_bool_t),
            .callback = flecs_meta_entity_has
        });

        ecs_doc_set_brief(world, m, "Returns whether entity has component");
    }

    {
        ecs_entity_t m = ecs_function(world, {
            .name = "pair",
            .parent = ecs_entity(world, { .name = "core"}),
            .params = {
                { .name = "first", .type = ecs_id(ecs_entity_t) },
                { .name = "second", .type = ecs_id(ecs_entity_t) }
            },
            .return_type = ecs_id(ecs_id_t),
            .callback = flecs_meta_core_pair
        });

        ecs_doc_set_brief(world, m, "Returns a pair identifier");
    }

    flecs_script_register_builtin_doc_functions(world);
}

#endif
