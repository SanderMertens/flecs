#include <ecs/entity.h>
#include <ecs/vector.h>
#include <ecs/hash.h>
#include <ecs/components.h>
#include "entity.h"

EcsEntity *EcsType_e;

EcsEntity* ecs_entity_new(
    EcsWorld *world,
    const char *id,
    EcsEntity *base)
{
    EcsEntity *result = ecs_world_add_entity(world);
    result->id = strdup(id);
    result->id_hash = 0;
    ecs_hash(id, strlen(id), &result->id_hash);
    result->base = base;
    result->components = NULL;
    return result;
}

void ecs_entity_add_component(
    EcsEntity *entity,
    EcsEntity *component)
{
}

void ecs_init(void)
{
}

void ecs_fini(void)
{

}
