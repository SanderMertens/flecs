/**
 * @file addons/script/functions_builtin.c
 * @brief Built-in functions for flecs script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static void flecs_meta_collection_count(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    const EcsType *type = ecs_get(ctx->world, argv[0].type, EcsType);
    int32_t count = 0;
    switch (type->kind) {
    case EcsArrayType:
        count = ecs_get(ctx->world, argv[0].type, EcsArray)->count;
        break;
    case EcsVectorType:
        count = ecs_vec_count(argv[0].ptr);
        break;
    case EcsMapType:
        count = ecs_map_count((ecs_map_t*)argv[0].ptr);
        break;
    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
    *(ecs_i32_t*)result->ptr = count;
}

static bool flecs_meta_entity_valid(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t entity)
{
    if (entity && ecs_is_alive(ctx->world, entity)) {
        return true;
    }

    ecs_err("invalid entity %u passed to method", (uint32_t)entity);
    flecs_script_runtime_get(ctx->world)->error = true;
    return false;
}

static void flecs_meta_entity_name(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    if (!flecs_meta_entity_valid(ctx, entity)) {
        return;
    }

    *(char**)result->ptr = ecs_os_strdup(ecs_get_name(ctx->world, entity));
}

static void flecs_meta_entity_path(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    if (!flecs_meta_entity_valid(ctx, entity)) {
        return;
    }

    *(char**)result->ptr = ecs_get_path(ctx->world, entity);
}

static void flecs_meta_entity_parent(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    if (!flecs_meta_entity_valid(ctx, entity)) {
        return;
    }

    *(ecs_entity_t*)result->ptr = ecs_get_parent(ctx->world, entity);
}

static void flecs_meta_entity_has(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    if (!flecs_meta_entity_valid(ctx, entity)) {
        return;
    }

    ecs_id_t id = *(ecs_id_t*)argv[1].ptr;
    *(ecs_bool_t*)result->ptr = ecs_has_id(ctx->world, entity, id);
}

static void flecs_meta_entity_lookup(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    (void)ctx;
    ecs_entity_t entity = *(ecs_entity_t*)argv[0].ptr;
    const char *path = *(char**)argv[1].ptr;
    if (!path) {
        *(ecs_entity_t*)result->ptr = 0;
    } else if (entity) {
        *(ecs_entity_t*)result->ptr = ecs_lookup_child(ctx->world, entity, path);
    } else {
        *(ecs_entity_t*)result->ptr = ecs_lookup(ctx->world, path);
    }
}

static void flecs_meta_core_pair(
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
        if (!flecs_meta_entity_valid(ctx, entity)) {\
            return;\
        }\
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

static const struct {
    const char *name;
    ecs_function_callback_t callback;
} flecs_script_doc_methods[] = {
    { "doc_name", flecs_meta_entity_doc_name },
    { "doc_uuid", flecs_meta_entity_doc_uuid },
    { "doc_brief", flecs_meta_entity_doc_brief },
    { "doc_detail", flecs_meta_entity_doc_detail },
    { "doc_link", flecs_meta_entity_doc_link },
    { "doc_color", flecs_meta_entity_doc_color }
};

#endif

void flecs_script_register_builtin_functions(
    ecs_world_t *world)
{
    ecs_method(world, {
        .name = "count",
        .parent = ecs_entity(world, { .name = "collection" }),
        .return_type = ecs_id(ecs_i32_t),
        .callback = flecs_meta_collection_count
    });

    ecs_function_desc_t entity_methods[] = {{
        .name = "name",
        .return_type = ecs_id(ecs_string_t),
        .callback = flecs_meta_entity_name
    }, {
        .name = "path",
        .return_type = ecs_id(ecs_string_t),
        .callback = flecs_meta_entity_path
    }, {
        .name = "parent",
        .return_type = ecs_id(ecs_entity_t),
        .callback = flecs_meta_entity_parent
    }, {
        .name = "has",
        .params = {{ .name = "component", .type = ecs_id(ecs_id_t) }},
        .return_type = ecs_id(ecs_bool_t),
        .callback = flecs_meta_entity_has
    }, {
        .name = "lookup",
        .params = {{ .name = "path", .type = ecs_id(ecs_string_t) }},
        .return_type = ecs_id(ecs_entity_t),
        .callback = flecs_meta_entity_lookup
    }};

    int32_t i, count = (int32_t)(
        sizeof(entity_methods) / sizeof(entity_methods[0]));
    for (i = 0; i < count; i ++) {
        entity_methods[i].parent = ecs_id(ecs_entity_t);
        ecs_method_init(world, &entity_methods[i]);
    }

    ecs_function(world, {
        .name = "pair",
        .parent = ecs_entity(world, { .name = "core" }),
        .params = {
            { .name = "first", .type = ecs_id(ecs_entity_t) },
            { .name = "second", .type = ecs_id(ecs_entity_t) }
        },
        .return_type = ecs_id(ecs_id_t),
        .callback = flecs_meta_core_pair
    });

#ifdef FLECS_DOC
    count = (int32_t)(sizeof(flecs_script_doc_methods) /
        sizeof(flecs_script_doc_methods[0]));
    for (i = 0; i < count; i ++) {
        ecs_method(world, {
            .name = flecs_script_doc_methods[i].name,
            .parent = ecs_id(ecs_entity_t),
            .return_type = ecs_id(ecs_string_t),
            .callback = flecs_script_doc_methods[i].callback
        });
    }
#endif
}

#endif
