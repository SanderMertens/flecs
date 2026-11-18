/**
 * @file addons/doc.c
 * @brief Doc addon.
 */

#include "../private_api.h"

#ifdef FLECS_DOC

static ECS_COPY(EcsDocDescription, dst, src, {
    ecs_os_strset((char**)&dst->value, src->value);

})

static ECS_MOVE(EcsDocDescription, dst, src, {
    ecs_os_free((char*)dst->value);
    dst->value = src->value;
    src->value = NULL;
})

static ECS_DTOR(EcsDocDescription, ptr, {
    ecs_os_free((char*)ptr->value);
})

static ECS_COPY(EcsDocSourceLocation, dst, src, {
    ecs_os_strset(&dst->file, src->file);
    dst->line = src->line;
})

static ECS_MOVE(EcsDocSourceLocation, dst, src, {
    ecs_os_free(dst->file);
    dst->file = src->file;
    dst->line = src->line;
    src->file = NULL;
})

static ECS_DTOR(EcsDocSourceLocation, ptr, {
    ecs_os_free(ptr->file);
})

static void flecs_doc_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t kind,
    const char *value)
{
    if (value) {
        ecs_set_pair(world, entity, EcsDocDescription, kind, {
            /* Safe, value gets copied by copy hook */
            .value = ECS_CONST_CAST(char*, value)
        });
    } else {
        ecs_remove_pair(world, entity, ecs_id(EcsDocDescription), kind);
    }
}

void ecs_doc_set_uuid(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    flecs_doc_set(world, entity, EcsDocUuid, name);
}

void ecs_doc_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    flecs_doc_set(world, entity, EcsName, name);
}

void ecs_doc_set_brief(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *brief)
{
    flecs_doc_set(world, entity, EcsDocBrief, brief);
}

void ecs_doc_set_detail(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *detail)
{
    flecs_doc_set(world, entity, EcsDocDetail, detail);
}

void ecs_doc_set_link(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *link)
{
    flecs_doc_set(world, entity, EcsDocLink, link);
}

void ecs_doc_set_color(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *color)
{
    flecs_doc_set(world, entity, EcsDocColor, color);
}

void ecs_doc_set_source_location(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *file,
    int32_t line)
{
    if (file) {
        ecs_set(world, entity, EcsDocSourceLocation, {
            /* Safe, file gets copied by copy hook */
            .file = ECS_CONST_CAST(char*, file),
            .line = line
        });
    } else {
        ecs_remove(world, entity, EcsDocSourceLocation);
    }
}

const char* ecs_doc_get_uuid(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocUuid);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsName);
    if (ptr) {
        return ptr->value;
    } else {
        return ecs_get_name(world, entity);
    }
}

const char* ecs_doc_get_brief(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocBrief);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_detail(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocDetail);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_link(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocLink);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_color(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocColor);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const EcsDocSourceLocation* ecs_doc_get_source_location(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    return ecs_get(world, entity, EcsDocSourceLocation);
}

void FlecsDocImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsDoc);

    ecs_set_name_prefix(world, "EcsDoc");

    flecs_bootstrap_component(world, EcsDocDescription);
    flecs_bootstrap_tag(world, EcsDocUuid);
    flecs_bootstrap_tag(world, EcsDocBrief);
    flecs_bootstrap_tag(world, EcsDocDetail);
    flecs_bootstrap_tag(world, EcsDocLink);
    flecs_bootstrap_tag(world, EcsDocColor);
    flecs_bootstrap_component(world, EcsDocSourceLocation);

    ecs_set_hooks(world, EcsDocDescription, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsDocDescription),
        .copy = ecs_copy(EcsDocDescription),
        .dtor = ecs_dtor(EcsDocDescription)
    });

    ecs_set_hooks(world, EcsDocSourceLocation, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsDocSourceLocation),
        .copy = ecs_copy(EcsDocSourceLocation),
        .dtor = ecs_dtor(EcsDocSourceLocation)
    });

#ifdef FLECS_META
    ecs_struct(world, {
        .entity = ecs_id(EcsDocDescription),
        .members = {
            { .name = "value", .type = ecs_id(ecs_string_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsDocSourceLocation),
        .members = {
            { .name = "file", .type = ecs_id(ecs_string_t) },
            { .name = "line", .type = ecs_id(ecs_i32_t) }
        }
    });
#endif

    ecs_add_pair(world, ecs_id(EcsDocDescription), EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, ecs_id(EcsDocSourceLocation), EcsOnInstantiate, EcsDontInherit);
#ifdef FLECS_CONSTRAINT_TRAITS
    ecs_add_id(world, ecs_id(EcsDocDescription), EcsRelationship);
#endif
}

#endif
