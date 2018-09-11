#ifndef REFLECS_WORLD_PRIVATE_H
#define REFLECS_WORLD_PRIVATE_H

#include <reflecs/reflecs.h>
#include <reflecs/vector.h>
#include <reflecs/map.h>
#include "table.h"


/* -- Public (opaque) types -- */

struct EcsWorld {
    EcsVector *entities;          /* vector<EcsEntity> */
    EcsVector *tables;            /* vector<EcsTable> */
    EcsMap *entities_map;         /* Map for quick entity lookups */
    EcsMap *tables_map;           /* Map for quick table lookups */
    EcsMap *components_map;       /* Map that stores component sets */
};

extern const EcsVectorParams entities_vec_params;
extern const EcsVectorParams tables_vec_params;
extern const EcsArrayParams entities_arr_params;

uint64_t ecs_world_component_set_hash(
    EcsWorld *world,
    EcsArray *set,
    EcsEntity *to_add);


#endif
