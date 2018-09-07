#ifndef ECS_ENTITY_PRIVATE_H
#define ECS_ENTITY_PRIVATE_H

#include <ecs/map.h>
#include <ecs/vector.h>
#include "world.h"

#define ECS_INITIAL_TABLE_COUNT (32)
#define ECS_INITIAL_CHUNK_COUNT (64)


/* -- Private types -- */

typedef struct EcsComponentElement {
    EcsEntity *component;         /* Type of component */
    void *data;                   /* Pointer to component data */
} EcsComponentElement;


/* -- Public (opaque) types -- */

struct EcsEntity {
    uint64_t id_hash;             /* Hashed id */
    uint64_t components_hash;     /* Hashed components list */
    char *id;                     /* String-based id */
    EcsVector *components;        /* vector<EcsComponentElement> */
    EcsEntity *base;              /* Base of entity (optional) */
    EcsWorld *world;              /* Reference to the world */
};

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from);

#endif
