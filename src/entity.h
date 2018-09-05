#ifndef ECS_ENTITY_PRIVATE_H
#define ECS_ENTITY_PRIVATE_H

#include <ecs/map.h>
#include <ecs/vector.h>
#include "world.h"

#define ECS_INITIAL_TABLE_COUNT (32)
#define ECS_INITIAL_CHUNK_COUNT (64)


/* -- Private types -- */

typedef struct EcsComponentElement {
    EcsEntity *type;              /* Type of component */
    void *data_ptr;               /* Pointer to component data */
} EcsComponentElement;


/* -- Public (opaque) types -- */

struct EcsEntity {
    char *id;                     /* String-based id */
    uint64_t id_hash;             /* Hashed id */
    EcsVector *components;        /* vector<EcsComponentElement> */
    EcsEntity *base;              /* Base of entity (optional) */
    bool edit_mode;               /* Enable to efficiently add/remove components */
};

#endif
