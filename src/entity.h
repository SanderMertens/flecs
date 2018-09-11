#ifndef REFLECS_ENTITY_PRIVATE_H
#define REFLECS_ENTITY_PRIVATE_H

#include <reflecs/map.h>
#include <reflecs/vector.h>
#include "world.h"

#define REFLECS_INITIAL_TABLE_COUNT (8)
#define REFLECS_INITIAL_ENTITY_COUNT (8)
#define REFLECS_INITIAL_CHUNK_COUNT (64)
#define REFLECS_INITIAL_COMPONENT_SET_COUNT (8)

/* -- Public (opaque) types -- */

struct EcsEntity {
    char *id;
    uint64_t stage_hash;
    uint64_t table_hash;
    void *row;
    EcsEntity *base;
    EcsWorld *world;
};

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from);

#endif
