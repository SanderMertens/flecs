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
    EcsEntity *type;              /* Component type entity */
};

extern const EcsVectorParams entities_vec_params;
extern const EcsVectorParams tables_vec_params;
extern const EcsArrayParams entityptr_arr_params;


/* -- Private functions -- */

EcsArray* ecs_world_get_components(
    EcsWorld *world,
    uint64_t components_hash);

uint64_t ecs_world_components_hash(
    EcsWorld *world,
    EcsArray *set,
    EcsEntity *to_add);

EcsTable *ecs_world_lookup_table(
    EcsWorld *world,
    uint64_t components_hash);

EcsTable *ecs_world_create_table(
    EcsWorld *world,
    uint64_t components_hash);


#endif
