
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

void ecs_doc_set_brief(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description)
{
    ecs_set_pair(world, entity, EcsDocDescription, EcsDocBrief, {
        .value = description
    });
}

void ecs_doc_set_detail(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description)
{
    ecs_set_pair(world, entity, EcsDocDescription, EcsDocDetail, {
        .value = description
    });
}

void ecs_doc_set_link(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *link)
{
    ecs_set_pair(world, entity, EcsDocDescription, EcsDocLink, {
        .value = link
    });
}

const char* ecs_doc_get_brief(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    EcsDocDescription *ptr = ecs_get_pair(
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
    EcsDocDescription *ptr = ecs_get_pair(
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
    EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocLink);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

void FlecsDocImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsDoc);

    ecs_set_name_prefix(world, "EcsDoc");

    flecs_bootstrap_component(world, EcsDocDescription);
    flecs_bootstrap_tag(world, EcsDocBrief);
    flecs_bootstrap_tag(world, EcsDocDetail);
    flecs_bootstrap_tag(world, EcsDocLink);

    ecs_set_component_actions(world, EcsDocDescription, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsDocDescription),
        .copy = ecs_copy(EcsDocDescription),
        .dtor = ecs_dtor(EcsDocDescription)
    });
}

#endif
