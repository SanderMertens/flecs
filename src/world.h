#ifndef ECS_WORLD_PRIVATE_H
#define ECS_WORLD_PRIVATE_H

#include <ecs/entity.h>
#include <ecs/vector.h>
#include <ecs/map.h>
#include "table.h"


/* -- Public (opaque) types -- */

struct EcsWorld {
    EcsVector *entities;          /* vector<EcsEntity> */
    EcsVector *tables;            /* vector<EcsTable> */
    EcsMap *entities_map;         /* Map for quick entity lookups */
    EcsMap *tables_map;           /* Map for quick table lookups */
};


/* -- Private functions -- */

EcsEntity* ecs_world_alloc_entity(
    EcsWorld *world);

void ecs_world_add_entity(
    EcsWorld *world,
    EcsEntity *entity);

EcsTable *ecs_world_alloc_table(
    EcsWorld *world);

void ecs_world_add_table(
    EcsWorld *world,
    EcsTable *table);

void ecs_world_remove_table(
    EcsWorld *world,
    EcsTable *table);

EcsTable* ecs_world_lookup_table(
    EcsWorld *world,
    uint64_t component_hash);

#endif
