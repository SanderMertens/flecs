
#include "../private_api.h"

#ifdef FLECS_DOC

static ECS_COPY(EcsDescription, dst, src, {
    ecs_os_strset((char**)&dst->value, src->value);

})

static ECS_MOVE(EcsDescription, dst, src, {
    ecs_os_free((char*)dst->value);
    dst->value = src->value;
    src->value = NULL;
})

static ECS_DTOR(EcsDescription, ptr, { 
    ecs_os_free((char*)ptr->value);
})

void ecs_doc_set_brief(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description)
{
    ecs_set_pair(world, entity, EcsDescription, EcsBrief, {
        .value = description
    });
}

void ecs_doc_set_detail(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description)
{
    ecs_set_pair(world, entity, EcsDescription, EcsDetail, {
        .value = description
    });
}

const char* ecs_doc_get_brief(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    EcsDescription *ptr = ecs_get_pair(world, entity, EcsDescription, EcsBrief);
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
    EcsDescription *ptr = ecs_get_pair(
        world, entity, EcsDescription, EcsDetail);
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

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsDescription);
    flecs_bootstrap_tag(world, EcsBrief);
    flecs_bootstrap_tag(world, EcsDetail);

    ecs_set_component_actions(world, EcsDescription, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsDescription),
        .copy = ecs_copy(EcsDescription),
        .dtor = ecs_dtor(EcsDescription)
    });

    /* Initialize reflection data for Description component */
    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(EcsDescription),
        .members = {
            {.name = "value", .type = ecs_id(ecs_string_t)}
        }
    });
}

#endif
